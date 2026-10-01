#include "loopliteeffect.h"
#include "loopliteconfig.h"

#include <effect/effecthandler.h>
#include <effect/effectwindow.h>
#include <effect/globals.h>
#include <core/output.h>
#include <input_event.h>
#include <window.h>

#include <KGlobalAccel>

#include <QAction>
#include <QHash>
#include <QKeyEvent>
#include <QLoggingCategory>
#include <QQuickItem>
#include <QQuickView>
#include <QUrl>

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

void LoopLiteEffect::arm()
{
    if (m_targetWindow) {
        // F24 auto-repeats at the OS level while held; ignore re-entrant
        // triggers for a session that's already armed.
        return;
    }
    qCWarning(LOOP_LITE) << "arm() called";
    m_targetWindow = KWin::effects->activeWindow();
    if (!m_targetWindow) {
        qCWarning(LOOP_LITE) << "arm(): no active window, aborting";
        return;
    }
    m_targetScreen = m_targetWindow->screen();
    m_accumulatedDelta = QPointF();
    m_direction = SnapDirection::None;
    m_active = true;

    if (m_targetScreen) {
        const QRectF screenGeometry = m_targetScreen->geometryF();
        const QRect indicatorGeometry(
            QPoint(qRound(screenGeometry.center().x() - 110), qRound(screenGeometry.center().y() - 110)),
            QSize(220, 220));
        m_indicator->setGeometry(indicatorGeometry);
        if (QQuickItem *root = m_indicator->rootObject()) {
            root->setProperty("direction", snapDirectionName(m_direction));
        }
        m_indicator->show();
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
        const QRectF target = snapTargetGeometry(m_direction, area);
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
    const QRectF target = snapTargetGeometry(m_direction, area);
    m_outline->setGeometry(target.toRect());
    m_outline->show();
}

void LoopLiteEffect::updateDirectionFromPointer()
{
    const SnapDirection candidate = snapDirectionForDelta(
        m_accumulatedDelta, LoopLiteConfig::directionDeadzone(), LoopLiteConfig::enableTopBottomHalves());
    // Sticky: the pointer drifting back toward the deadzone (hand relaxing
    // before releasing F24) must not undo the current placement — only
    // clearing the deadzone in a *different* direction changes it.
    if (candidate != SnapDirection::None) {
        setDirection(candidate);
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

    if (event->type() == QEvent::KeyPress) {
        if (event->key() == Qt::Key_Escape) {
            finish(false);
            return;
        }
        // Arrow keys set the placement directly (always cardinal, independent
        // of the EnableTopBottomHalves mouse-drag setting — pressing an arrow
        // is unambiguous). Diagonals still require the mouse.
        switch (event->key()) {
        case Qt::Key_Left:
            setDirection(SnapDirection::Left);
            return;
        case Qt::Key_Right:
            setDirection(SnapDirection::Right);
            return;
        case Qt::Key_Up:
            setDirection(SnapDirection::Top);
            return;
        case Qt::Key_Down:
            setDirection(SnapDirection::Bottom);
            return;
        default:
            break;
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
