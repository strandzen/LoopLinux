#include "loopliteeffect.h"
#include "loopliteconfig.h"

#include <effect/effecthandler.h>
#include <effect/effectwindow.h>
#include <effect/globals.h>
#include <core/output.h>
#include <input_event.h>
#include <window.h>

#include <KConfig>
#include <KGlobalAccel>

#include <QAction>
#include <QHash>
#include <QKeyEvent>
#include <QLoggingCategory>
#include <QQuickItem>
#include <QQuickView>
#include <QUrl>

#include <array>

Q_LOGGING_CATEGORY(LOOP_LITE, "loop.lite")

namespace
{
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

LoopLiteEffect::LoopLiteEffect()
    : m_outline(makeOverlayWindow(QUrl(QStringLiteral("qrc:/qml/Outline.qml"))))
    , m_indicator(makeOverlayWindow(QUrl(QStringLiteral("qrc:/qml/Indicator.qml"))))
{
    m_toggleAction = new QAction(this);
    m_toggleAction->setObjectName(QStringLiteral("LoopLiteSnap"));
    m_toggleAction->setText(QStringLiteral("Loop Lite: hold to snap window"));
    KGlobalAccel::self()->setDefaultShortcut(m_toggleAction, {QKeySequence(Qt::Key_F24)});
    KGlobalAccel::self()->setShortcut(m_toggleAction, {QKeySequence(Qt::Key_F24)});
    connect(m_toggleAction, &QAction::triggered, this, &LoopLiteEffect::arm);

    qCWarning(LOOP_LITE) << "constructed, build marker rev5-qquickview";
}

LoopLiteEffect::~LoopLiteEffect() = default;

bool LoopLiteEffect::supported()
{
    return KWin::effects->isOpenGLCompositing();
}

bool LoopLiteEffect::isActive() const
{
    return m_active;
}

void LoopLiteEffect::reconfigure(ReconfigureFlags flags)
{
    Q_UNUSED(flags)
    // The KCM runs in a separate process (System Settings/kcmshell6) with
    // its own LoopLiteConfig::self() instance; it writes to the same
    // kwinrc, but our already-running copy never re-reads the file on its
    // own. reparseConfiguration() forces KConfig to re-read from disk, then
    // load() repopulates the skeleton's cached values from it — without
    // this, settings changed in the KCM would silently keep using whatever
    // was cached from when this effect's LoopLiteConfig was first touched.
    LoopLiteConfig::self()->config()->reparseConfiguration();
    LoopLiteConfig::self()->load();
    qCWarning(LOOP_LITE) << "reconfigure(): config reloaded";
}

void LoopLiteEffect::arm()
{
    if (m_targetWindow) {
        // F24 auto-repeats at the OS level while held; ignore re-entrant
        // triggers for a session that's already armed.
        return;
    }

    // LoopLiteConfig::self() is a process-wide singleton, independent of
    // this Effect object's own lifecycle — toggling the effect off/on in
    // Desktop Effects destroys and recreates the Effect, but calls into
    // the exact same (still stale) config singleton either way, so that
    // alone doesn't help. Rather than depend on KWin calling reconfigure()
    // on us at some point (unverified whether it reliably does), force a
    // fresh read from disk unconditionally on every arm() — cheap (a small
    // ini file), and guarantees up-to-date settings on every hyperkey
    // press regardless of what did or didn't notify us.
    LoopLiteConfig::self()->config()->reparseConfiguration();
    LoopLiteConfig::self()->load();

    qCWarning(LOOP_LITE) << "arm() called";
    m_targetWindow = KWin::effects->activeWindow();
    if (!m_targetWindow) {
        qCWarning(LOOP_LITE) << "arm(): no active window, aborting";
        return;
    }
    m_targetScreen = m_targetWindow->screen();
    m_accumulatedDelta = QPointF();
    m_direction = SnapDirection::None;
    m_keyUp = m_keyDown = m_keyLeft = m_keyRight = false;
    m_active = true;

    applyIndicatorStyle();

    if (m_targetScreen) {
        repositionIndicator();
        if (QQuickItem *root = m_indicator->rootObject()) {
            root->setProperty("direction", snapDirectionName(m_direction));
        }
        if (LoopLiteConfig::showIndicator()) {
            m_indicator->show();
        }
    }

    KWin::effects->grabKeyboard(this);
    KWin::effects->startMouseInterception(this, Qt::CrossCursor);
}

void LoopLiteEffect::finish(bool applyPendingDirection)
{
    qCWarning(LOOP_LITE) << "finish() applyPendingDirection=" << applyPendingDirection
                          << "m_direction=" << snapDirectionName(m_direction);
    KWin::effects->stopMouseInterception(this);
    KWin::effects->ungrabKeyboard();
    m_outline->hide();
    m_indicator->hide();
    m_active = false;

    if (applyPendingDirection && m_targetWindow && m_direction != SnapDirection::None) {
        const QRectF area = KWin::effects->clientArea(KWin::MaximizeArea, m_targetScreen);
        const QRectF target = snapTargetGeometry(m_direction, area, LoopLiteConfig::paddingHorizontal(), LoopLiteConfig::paddingVertical());
        if (KWin::Window *window = m_targetWindow->window()) {
            window->setMaximize(false, false);
            window->moveResize(target);
            qCWarning(LOOP_LITE) << "finish(): moveResize done, target=" << target;
        } else {
            qCWarning(LOOP_LITE) << "finish(): m_targetWindow->window() was null!";
        }
    } else {
        qCWarning(LOOP_LITE) << "finish(): no snap applied (applyPendingDirection=" << applyPendingDirection
                              << "m_targetWindow=" << m_targetWindow << "m_direction=" << snapDirectionName(m_direction) << ")";
    }

    m_targetWindow = nullptr;
    m_targetScreen = nullptr;
    m_direction = SnapDirection::None;
}

void LoopLiteEffect::setDirection(SnapDirection direction)
{
    if (direction == m_direction) {
        return;
    }
    qCWarning(LOOP_LITE) << "setDirection():" << snapDirectionName(m_direction) << "->" << snapDirectionName(direction);
    m_direction = direction;

    if (QQuickItem *root = m_indicator->rootObject()) {
        root->setProperty("direction", snapDirectionName(m_direction));
    }

    if (m_direction == SnapDirection::None || !m_targetScreen) {
        m_outline->hide();
        return;
    }

    // These are genuine top-level windows in global desktop coordinates
    // (unlike QuickSceneView, which was scoped to one screen's own output),
    // so the globally computed target rect can be used directly.
    const QRectF area = KWin::effects->clientArea(KWin::MaximizeArea, m_targetScreen);
    const QRectF target = snapTargetGeometry(m_direction, area, LoopLiteConfig::paddingHorizontal(), LoopLiteConfig::paddingVertical());
    m_outline->setGeometry(target.toRect());
    if (LoopLiteConfig::showOutline()) {
        m_outline->show();
    }
}

void LoopLiteEffect::updateDirectionFromPointer()
{
    const SnapDirection candidate = snapDirectionForDelta(m_accumulatedDelta, LoopLiteConfig::horizontalDeadzone(), LoopLiteConfig::verticalDeadzone());
    // Sticky: the pointer drifting back toward the deadzone (hand relaxing
    // before releasing F24) must not undo the current placement — only
    // clearing the deadzone in a *different* direction changes it.
    if (candidate != SnapDirection::None) {
        setDirection(candidate);
    }
}

void LoopLiteEffect::repositionIndicator()
{
    if (!m_targetScreen) {
        return;
    }
    // Called once at arm() time: either centered on screen, or spawned at
    // wherever the cursor happened to be when the hyperkey was pressed (not
    // live-tracked afterward — confirmed that's the intended behavior).
    const QPointF center = LoopLiteConfig::indicatorFollowsMouse() ? KWin::effects->cursorPos() : m_targetScreen->geometryF().center();
    const int size = LoopLiteConfig::indicatorSize();
    const QRect indicatorGeometry(QPoint(qRound(center.x() - size / 2.0), qRound(center.y() - size / 2.0)), QSize(size, size));
    m_indicator->setGeometry(indicatorGeometry);
}

void LoopLiteEffect::applyIndicatorStyle()
{
    // Read fresh each time a session arms, rather than reacting live to
    // config changes mid-session — settings changed in the KCM take effect
    // the next time Loop Lite is triggered.
    if (QQuickItem *root = m_indicator->rootObject()) {
        root->setProperty("cornerRadius", LoopLiteConfig::indicatorCornerRadius());
        root->setProperty("ringWidth", LoopLiteConfig::indicatorRingWidth());
        root->setProperty("showText", LoopLiteConfig::showIndicatorText());
        root->setProperty("highlightColor", LoopLiteConfig::outlineColor());
    }
    if (QQuickItem *root = m_outline->rootObject()) {
        root->setProperty("borderWidth", LoopLiteConfig::outlineBorderWidth());
        root->setProperty("borderColor", LoopLiteConfig::outlineColor());
        root->setProperty("cornerRadius", LoopLiteConfig::outlineCornerRadius());
    }
}

void LoopLiteEffect::actionMaximize()
{
    if (!m_targetWindow) {
        return;
    }
    if (KWin::Window *window = m_targetWindow->window()) {
        window->setMaximize(true, true);
    }
}

void LoopLiteEffect::actionMinimize()
{
    if (m_targetWindow) {
        m_targetWindow->setMinimized(true);
    }
}

void LoopLiteEffect::grabbedKeyboardEvent(QKeyEvent *event)
{
    // Hyperkey+<key> bindings available while armed. Add a row here to
    // extend — the action fires immediately and ends the session without
    // applying a directional snap.
    static const QHash<int, void (LoopLiteEffect::*)()> keyActions{
        {Qt::Key_Return, &LoopLiteEffect::actionMaximize},
        {Qt::Key_Enter, &LoopLiteEffect::actionMaximize},
        {Qt::Key_Backspace, &LoopLiteEffect::actionMinimize},
    };

    // The configured key scheme (arrows/WASD/HJKL) sets the placement
    // directly. Both press and release are tracked so holding two keys at
    // once (e.g. Up+Left) composes a corner instead of only the latest key
    // winning.
    const KeyAxis axis = axisForArmedKey(event->key(), LoopLiteConfig::keybindScheme());
    if (axis != KeyAxis::None && (event->type() == QEvent::KeyPress || event->type() == QEvent::KeyRelease)) {
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

void LoopLiteEffect::pointerMotion(KWin::PointerMotionEvent *event)
{
    m_accumulatedDelta += event->delta;
    updateDirectionFromPointer();
}

KWIN_EFFECT_FACTORY(LoopLiteEffect, "metadata.json")

#include "loopliteeffect.moc"
