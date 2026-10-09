#include "sirkeleffect.h"
#include "sirkelconfig.h"

#include <effect/effecthandler.h>
#include <effect/effectwindow.h>
#include <effect/globals.h>
#include <core/output.h>
#include <input_event.h>
#include <window.h>

#include <KConfig>
#include <KConfigGroup>
#include <KGlobalAccel>

#include <QAction>
#include <QGuiApplication>
#include <QHash>
#include <QKeyEvent>
#include <QLoggingCategory>
#include <QPalette>
#include <QQuickItem>
#include <QQuickView>
#include <QUrl>

#include <array>

Q_LOGGING_CATEGORY(SIRKEL, "sirkel")

namespace
{
// OutlineColor's own kcfg default is just a static fallback color, not a
// live "theme accent" — KConfigXT has no way to express that in the .kcfg
// itself. So: on a fresh install, nothing has ever been written to the
// "OutlineColor" key under kwinrc's [Effect-sirkel] group at all (checking
// that directly, rather than comparing against the static default value,
// correctly distinguishes "never customized" from "customized to the same
// value the default happens to have"). While that's true, use the live
// Plasma accent/highlight color instead of the static fallback; once the
// user picks any color via the KCM, this key exists and that choice sticks.
QColor resolveOutlineColor()
{
    const bool hasCustomColor = SirkelConfig::self()->config()->group(QStringLiteral("Effect-sirkel")).hasKey(QStringLiteral("OutlineColor"));
    if (!hasCustomColor) {
        return QGuiApplication::palette().color(QPalette::Highlight);
    }
    return SirkelConfig::outlineColor();
}

std::unique_ptr<QQuickView> makeOverlayWindow(const QUrl &source)
{
    auto view = std::make_unique<QQuickView>(source);
    view->setFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::Tool
                   | Qt::WindowDoesNotAcceptFocus | Qt::WindowTransparentForInput);
    view->setColor(Qt::transparent);
    view->setResizeMode(QQuickView::SizeRootObjectToView);
    return view;
}

// Which role a physical key plays in direction selection, independent of
// whether it's currently held — lets two keys (e.g. Up+Left) combine into a
// corner instead of only the most-recently-pressed one winning.
enum class KeyAxis { None, Up, Down, Left, Right };

struct KeyBinding {
    Qt::Key key;
    KeyAxis axis;
};

// Which physical keys map to which axis while armed, selected via the
// KeybindScheme setting (KCM: a scheme dropdown, not arbitrary rebinding).
KeyAxis axisForArmedKey(int key, const QString &scheme)
{
    // "none" means mouse-only: no direction key bindings at all, rather
    // than falling back to the "arrows" default below.
    if (scheme == QStringLiteral("none")) {
        return KeyAxis::None;
    }

    static const QHash<QString, std::array<KeyBinding, 4>> kSchemes{
        {QStringLiteral("arrows"),
         {{{Qt::Key_Left, KeyAxis::Left}, {Qt::Key_Right, KeyAxis::Right}, {Qt::Key_Up, KeyAxis::Up}, {Qt::Key_Down, KeyAxis::Down}}}},
        {QStringLiteral("wasd"),
         {{{Qt::Key_A, KeyAxis::Left}, {Qt::Key_D, KeyAxis::Right}, {Qt::Key_W, KeyAxis::Up}, {Qt::Key_S, KeyAxis::Down}}}},
        {QStringLiteral("hjkl"),
         {{{Qt::Key_H, KeyAxis::Left}, {Qt::Key_L, KeyAxis::Right}, {Qt::Key_K, KeyAxis::Up}, {Qt::Key_J, KeyAxis::Down}}}},
    };
    auto it = kSchemes.constFind(scheme);
    if (it == kSchemes.constEnd()) {
        it = kSchemes.constFind(QStringLiteral("arrows"));
    }
    for (const KeyBinding &binding : it.value()) {
        if (binding.key == key) {
            return binding.axis;
        }
    }
    return KeyAxis::None;
}

// Combines the currently-held Up/Down/Left/Right keys into one of the 8
// directions (or None). Opposite keys held together (e.g. Up+Down) cancel
// out on that axis rather than picking one arbitrarily.
SnapDirection composeDirection(bool up, bool down, bool left, bool right)
{
    const bool vertical = up != down;
    const bool horizontal = left != right;
    if (vertical && horizontal) {
        if (up) {
            return left ? SnapDirection::TopLeft : SnapDirection::TopRight;
        }
        return left ? SnapDirection::BottomLeft : SnapDirection::BottomRight;
    }
    if (vertical) {
        return up ? SnapDirection::Top : SnapDirection::Bottom;
    }
    if (horizontal) {
        return left ? SnapDirection::Left : SnapDirection::Right;
    }
    return SnapDirection::None;
}
}

SirkelEffect::SirkelEffect()
    : m_outline(makeOverlayWindow(QUrl(QStringLiteral("qrc:/qml/Outline.qml"))))
    , m_indicator(makeOverlayWindow(QUrl(QStringLiteral("qrc:/qml/Indicator.qml"))))
{
    m_toggleAction = new QAction(this);
    m_toggleAction->setObjectName(QStringLiteral("SirkelSnap"));
    m_toggleAction->setText(QStringLiteral("Sirkel: hold to snap window"));
    KGlobalAccel::self()->setDefaultShortcut(m_toggleAction, {QKeySequence(Qt::Key_F24)});
    KGlobalAccel::self()->setShortcut(m_toggleAction, {QKeySequence(Qt::Key_F24)});
    connect(m_toggleAction, &QAction::triggered, this, &SirkelEffect::arm);

    qCWarning(SIRKEL) << "constructed, build marker rev5-qquickview";
}

SirkelEffect::~SirkelEffect() = default;

bool SirkelEffect::supported()
{
    return KWin::effects->isOpenGLCompositing();
}

bool SirkelEffect::isActive() const
{
    return m_active;
}

void SirkelEffect::reconfigure(ReconfigureFlags flags)
{
    Q_UNUSED(flags)
    // The KCM runs in a separate process (System Settings/kcmshell6) with
    // its own SirkelConfig::self() instance; it writes to the same
    // kwinrc, but our already-running copy never re-reads the file on its
    // own. reparseConfiguration() forces KConfig to re-read from disk, then
    // load() repopulates the skeleton's cached values from it — without
    // this, settings changed in the KCM would silently keep using whatever
    // was cached from when this effect's SirkelConfig was first touched.
    SirkelConfig::self()->config()->reparseConfiguration();
    SirkelConfig::self()->load();
    qCWarning(SIRKEL) << "reconfigure(): config reloaded";
}

void SirkelEffect::arm()
{
    if (m_targetWindow) {
        // F24 auto-repeats at the OS level while held; ignore re-entrant
        // triggers for a session that's already armed.
        return;
    }

    // SirkelConfig::self() is a process-wide singleton, independent of
    // this Effect object's own lifecycle — toggling the effect off/on in
    // Desktop Effects destroys and recreates the Effect, but calls into
    // the exact same (still stale) config singleton either way, so that
    // alone doesn't help. Rather than depend on KWin calling reconfigure()
    // on us at some point (unverified whether it reliably does), force a
    // fresh read from disk unconditionally on every arm() — cheap (a small
    // ini file), and guarantees up-to-date settings on every hyperkey
    // press regardless of what did or didn't notify us.
    SirkelConfig::self()->config()->reparseConfiguration();
    SirkelConfig::self()->load();

    qCWarning(SIRKEL) << "arm() called";
    m_targetWindow = KWin::effects->activeWindow();
    if (!m_targetWindow) {
        qCWarning(SIRKEL) << "arm(): no active window, aborting";
        return;
    }
    if (m_targetWindow->isSpecialWindow()) {
        // Clicking the bare desktop can make KWin's desktop containment
        // window (the wallpaper layer) the "active window" — calling
        // moveResize() on that isn't a real window move, it visibly shifts
        // the wallpaper and leaves part of the screen black where it used
        // to be. isSpecialWindow() covers the desktop, docks/panels, and
        // similar window types that aren't meant to be moved/resized at
        // all, so refuse to arm against any of them.
        qCWarning(SIRKEL) << "arm(): active window is a special window (desktop/panel/etc), aborting";
        m_targetWindow = nullptr;
        return;
    }
    m_targetScreen = m_targetWindow->screen();
    m_accumulatedDelta = QPointF();
    m_direction = SnapDirection::None;
    m_keyUp = m_keyDown = m_keyLeft = m_keyRight = false;
    m_active = true;
    m_engaged = false;

    applyIndicatorStyle();

    if (m_targetScreen) {
        repositionIndicator();
        if (QQuickItem *root = m_indicator->rootObject()) {
            root->setProperty("direction", snapDirectionName(m_direction));
        }
        // Not shown yet — see engage(). Holding the hyperkey by itself
        // should be a no-op; the indicator only appears once the first real
        // input (mouse movement or a direction key) arrives.

        // The outline's window now stays fixed for the whole session,
        // covering the screen's entire usable area — only the highlighted
        // rect inside it (targetX/Y/Width/Height) moves, so that move can
        // be animated. Reset that rect to zero-size at the area's center
        // while still hidden, so the first direction picked this session
        // grows out from the middle instead of gliding in from wherever
        // the previous session last left it.
        m_outlineArea = KWin::effects->clientArea(KWin::MaximizeArea, m_targetScreen);
        m_outline->setGeometry(m_outlineArea.toRect());
        if (QQuickItem *root = m_outline->rootObject()) {
            const QPointF center = m_outlineArea.center() - m_outlineArea.topLeft();
            root->setProperty("targetX", center.x());
            root->setProperty("targetY", center.y());
            root->setProperty("targetWidth", 0);
            root->setProperty("targetHeight", 0);
        }
    }

    KWin::effects->grabKeyboard(this);
    KWin::effects->startMouseInterception(this, Qt::CrossCursor);
}

void SirkelEffect::engage()
{
    if (m_engaged) {
        return;
    }
    m_engaged = true;
    if (m_targetScreen && SirkelConfig::showIndicator()) {
        m_indicator->show();
    }
}

void SirkelEffect::finish(bool applyPendingDirection)
{
    qCWarning(SIRKEL) << "finish() applyPendingDirection=" << applyPendingDirection
                          << "m_direction=" << snapDirectionName(m_direction);
    KWin::effects->stopMouseInterception(this);
    KWin::effects->ungrabKeyboard();
    m_outline->hide();
    m_indicator->hide();
    m_active = false;

    if (applyPendingDirection && m_targetWindow && m_direction != SnapDirection::None) {
        const QRectF target = snapTargetGeometry(m_direction, m_outlineArea, SirkelConfig::paddingHorizontal(), SirkelConfig::paddingVertical());
        if (KWin::Window *window = m_targetWindow->window()) {
            window->setMaximize(false, false);
            window->moveResize(target);
            qCWarning(SIRKEL) << "finish(): moveResize done, target=" << target;
        } else {
            qCWarning(SIRKEL) << "finish(): m_targetWindow->window() was null!";
        }
    } else {
        qCWarning(SIRKEL) << "finish(): no snap applied (applyPendingDirection=" << applyPendingDirection
                              << "m_targetWindow=" << m_targetWindow << "m_direction=" << snapDirectionName(m_direction) << ")";
    }

    m_targetWindow = nullptr;
    m_targetScreen = nullptr;
    m_outlineArea = QRectF();
    m_direction = SnapDirection::None;
    m_engaged = false;
}

void SirkelEffect::setDirection(SnapDirection direction)
{
    if (direction == m_direction) {
        return;
    }
    qCWarning(SIRKEL) << "setDirection():" << snapDirectionName(m_direction) << "->" << snapDirectionName(direction);
    m_direction = direction;

    if (QQuickItem *root = m_indicator->rootObject()) {
        root->setProperty("direction", snapDirectionName(m_direction));
    }

    if (m_direction == SnapDirection::None || !m_targetScreen) {
        m_outline->hide();
        return;
    }

    // The outline window itself stays fixed (covering m_outlineArea) for
    // the whole session — only the highlighted rect inside it moves, in
    // window-local coordinates, so QML's Behavior animations can glide/grow
    // it between directions instead of an instant window move+resize.
    const QRectF target = snapTargetGeometry(m_direction, m_outlineArea, SirkelConfig::paddingHorizontal(), SirkelConfig::paddingVertical());
    if (QQuickItem *root = m_outline->rootObject()) {
        const QRectF local = target.translated(-m_outlineArea.topLeft());
        root->setProperty("targetX", local.x());
        root->setProperty("targetY", local.y());
        root->setProperty("targetWidth", local.width());
        root->setProperty("targetHeight", local.height());
    }
    if (SirkelConfig::showOutline()) {
        m_outline->show();
    }
}

void SirkelEffect::updateDirectionFromPointer()
{
    const SnapDirection candidate = snapDirectionForDelta(m_accumulatedDelta, SirkelConfig::horizontalDeadzone(), SirkelConfig::verticalDeadzone());
    // The center deadzone is itself a live zone (the maximize gesture), not
    // a "no selection yet" state — so unlike the 8 edge/corner directions,
    // returning to center always re-selects it, even after a real direction
    // was picked. (This does mean the hand easing back toward center right
    // before releasing F24 will switch the pick to maximize — a deliberate
    // trade-off for making center reachable at all after leaving it.)
    setDirection(candidate == SnapDirection::None ? SnapDirection::Maximize : candidate);
}

void SirkelEffect::repositionIndicator()
{
    if (!m_targetScreen) {
        return;
    }
    // Called once at arm() time: either centered on screen, or spawned at
    // wherever the cursor happened to be when the hyperkey was pressed (not
    // live-tracked afterward — confirmed that's the intended behavior).
    // Stored in m_indicatorCenter too: direction hit-testing is anchored to
    // this same point, so it always matches what's actually drawn on screen.
    m_indicatorCenter = SirkelConfig::indicatorFollowsMouse() ? KWin::effects->cursorPos() : m_targetScreen->geometryF().center();
    const int size = SirkelConfig::indicatorSize();
    const QRect indicatorGeometry(QPoint(qRound(m_indicatorCenter.x() - size / 2.0), qRound(m_indicatorCenter.y() - size / 2.0)), QSize(size, size));
    m_indicator->setGeometry(indicatorGeometry);
}

void SirkelEffect::applyIndicatorStyle()
{
    // Read fresh each time a session arms, rather than reacting live to
    // config changes mid-session — settings changed in the KCM take effect
    // the next time Sirkel is triggered.
    const QColor outlineColor = resolveOutlineColor();
    if (QQuickItem *root = m_indicator->rootObject()) {
        root->setProperty("cornerRadius", SirkelConfig::indicatorCornerRadius());
        root->setProperty("ringWidth", SirkelConfig::indicatorRingWidth());
        root->setProperty("pointerLength", SirkelConfig::indicatorPointerLength());
        root->setProperty("showText", SirkelConfig::showIndicatorText());
        root->setProperty("highlightColor", outlineColor);
        root->setProperty("animationDuration", SirkelConfig::outlineAnimationDuration());
    }
    if (QQuickItem *root = m_outline->rootObject()) {
        root->setProperty("borderWidth", SirkelConfig::outlineBorderWidth());
        root->setProperty("borderColor", outlineColor);
        root->setProperty("cornerRadius", SirkelConfig::outlineCornerRadius());
        root->setProperty("fillOpacity", SirkelConfig::outlineFillOpacity());
        root->setProperty("animationDuration", SirkelConfig::outlineAnimationDuration());
    }
}

void SirkelEffect::actionMaximize()
{
    if (!m_targetWindow || !m_targetScreen) {
        return;
    }
    if (KWin::Window *window = m_targetWindow->window()) {
        // Routed through the same padded geometry as the directional snaps
        // (rather than window->setMaximize(true, true), which bypasses
        // padding entirely) so Hyperkey+Enter and the center/maximize
        // gesture both respect PaddingHorizontal/PaddingVertical.
        const QRectF target = snapTargetGeometry(SnapDirection::Maximize, m_outlineArea, SirkelConfig::paddingHorizontal(), SirkelConfig::paddingVertical());
        window->setMaximize(false, false);
        window->moveResize(target);
    }
}

void SirkelEffect::actionMinimize()
{
    if (m_targetWindow) {
        m_targetWindow->setMinimized(true);
    }
}

void SirkelEffect::grabbedKeyboardEvent(QKeyEvent *event)
{
    // Hyperkey+<key> bindings available while armed. Add a row here to
    // extend — the action fires immediately and ends the session without
    // applying a directional snap.
    static const QHash<int, void (SirkelEffect::*)()> keyActions{
        {Qt::Key_Return, &SirkelEffect::actionMaximize},
        {Qt::Key_Enter, &SirkelEffect::actionMaximize},
        {Qt::Key_Backspace, &SirkelEffect::actionMinimize},
    };

    // The configured key scheme (arrows/WASD/HJKL) sets the placement
    // directly. Both press and release are tracked so holding two keys at
    // once (e.g. Up+Left) composes a corner instead of only the latest key
    // winning.
    const KeyAxis axis = axisForArmedKey(event->key(), SirkelConfig::keybindScheme());
    if (axis != KeyAxis::None && (event->type() == QEvent::KeyPress || event->type() == QEvent::KeyRelease)) {
        engage();
        const bool pressed = (event->type() == QEvent::KeyPress);
        switch (axis) {
        case KeyAxis::Up:
            m_keyUp = pressed;
            break;
        case KeyAxis::Down:
            m_keyDown = pressed;
            break;
        case KeyAxis::Left:
            m_keyLeft = pressed;
            break;
        case KeyAxis::Right:
            m_keyRight = pressed;
            break;
        case KeyAxis::None:
            break;
        }
        // Sticky, same as the mouse-drag case: releasing the keys that
        // composed a direction must not cancel it — only pressing a
        // *different* real combination changes the pick. Without this,
        // releasing e.g. Left+Up back to nothing would recompute None and
        // wipe out the selection before F24 is even released.
        const SnapDirection composed = composeDirection(m_keyUp, m_keyDown, m_keyLeft, m_keyRight);
        if (composed != SnapDirection::None) {
            setDirection(composed);
        }
        return;
    }

    if (event->type() == QEvent::KeyPress) {
        if (event->key() == Qt::Key_Escape) {
            finish(false);
            return;
        }
        if (const auto action = keyActions.value(event->key())) {
            (this->*action)();
            finish(false);
            return;
        }
    } else if (event->type() == QEvent::KeyRelease && event->key() == Qt::Key_F24) {
        finish(true);
        return;
    }
}

void SirkelEffect::pointerMotion(KWin::PointerMotionEvent *event)
{
    engage();
    // Absolute cursor position relative to the indicator's own drawn
    // center, recomputed fresh every time — not a running sum of each
    // event's relative delta since arm(). A pure delta-accumulator is
    // anchored to wherever the cursor physically was at the press, which
    // silently drifts away from the visible indicator whenever it's drawn
    // somewhere else (i.e. IndicatorFollowsMouse is off and the press
    // didn't happen to land exactly on the screen's center).
    m_accumulatedDelta = event->position - m_indicatorCenter;
    updateDirectionFromPointer();
}

KWIN_EFFECT_FACTORY(SirkelEffect, "metadata.json")

#include "sirkeleffect.moc"
