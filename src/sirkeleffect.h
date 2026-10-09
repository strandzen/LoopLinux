#pragma once

#include "snapcalculator.h"

#include <effect/effect.h>

#include <QPointF>
#include <QRectF>

#include <memory>

class QAction;
class QQuickView;

namespace KWin
{
class EffectWindow;
class LogicalOutput;
struct PointerMotionEvent;
}

// Loop-style directional window snapping. Hold the hyperkey (Caps Lock,
// remapped to F24 by keyd) to arm the effect, move the mouse (or press an
// arrow key) to pick a direction, release to snap the active window on its
// own screen.
//
// Activation is a single global shortcut (F24 press) handled the ordinary
// KWin-effect way via KGlobalAccel. Everything from press to release is then
// driven by an exclusive keyboard grab + pointer interception
// (effects->grabKeyboard / startMouseInterception).
//
// Visuals are two plain QQuickWindows (m_outline, m_indicator), not a
// QuickSceneEffect scene. KWin runs its own UI (decorations, tooltips, the
// task switcher) as ordinary internal QQuickWindows composited through the
// normal per-window path, which supports real alpha blending — unlike
// QuickSceneEffect, whose backing surface is always opaque (confirmed live:
// a Window.color="transparent" binding had zero visible effect there).
// Using plain QQuickWindow here is standard public Qt API, not a KWin-
// specific mechanism, so it carries no linking/ABI risk.
class SirkelEffect : public KWin::Effect
{
    Q_OBJECT

public:
    explicit SirkelEffect();
    ~SirkelEffect() override;

    static bool supported();
    bool isActive() const override;

protected:
    void grabbedKeyboardEvent(QKeyEvent *event) override;
    void pointerMotion(KWin::PointerMotionEvent *event) override;
    void reconfigure(ReconfigureFlags flags) override;

private:
    void arm();
    void finish(bool applyPendingDirection);
    void engage();
    void updateDirectionFromPointer();
    void setDirection(SnapDirection direction);
    void applyIndicatorStyle();
    void repositionIndicator();

    // Hyperkey+<key> actions available while armed. To add a new binding,
    // add a case in grabbedKeyboardEvent's key-action table in the .cpp.
    void actionMaximize();
    void actionMinimize();

    QAction *m_toggleAction = nullptr;
    std::unique_ptr<QQuickView> m_outline;
    std::unique_ptr<QQuickView> m_indicator;
    KWin::EffectWindow *m_targetWindow = nullptr;
    KWin::LogicalOutput *m_targetScreen = nullptr;
    // The target screen's usable area for this session, computed once in
    // arm(). m_outline's own window is sized to cover this area for the
    // whole session (not resized per direction change any more) so that
    // moving between directions can be an animated glide of an inner QML
    // rect instead of an instant window move/resize — also reused by
    // finish() and actionMaximize() instead of each re-querying it.
    QRectF m_outlineArea;
    // The indicator's own drawn center on screen for this session (screen
    // center, or the cursor position at arm() time, per
    // IndicatorFollowsMouse) — set once in repositionIndicator(). Direction
    // is picked from the cursor's *current absolute position relative to
    // this point*, not a running delta accumulated since arm(); otherwise,
    // whenever the indicator is drawn away from the press point (i.e. it's
    // centered on the screen, not following the mouse), the hit-testing
    // would stay anchored to the original press position instead of the
    // indicator everyone can actually see on screen.
    QPointF m_indicatorCenter;
    QPointF m_accumulatedDelta;
    SnapDirection m_direction = SnapDirection::None;
    bool m_active = false;
    // Holding the hyperkey alone must not show the indicator or grab the
    // screen's attention — only once real input arrives (mouse movement, or
    // a direction key) does the session "engage" and the UI appear. Until
    // then, grabKeyboard()/startMouseInterception() are already in effect
    // (needed to detect that first input at all) but nothing is visible.
    bool m_engaged = false;

    // Currently-held direction keys, composed into a single SnapDirection
    // (see composeDirection() in the .cpp) so holding two at once — e.g.
    // Up+Left — selects a corner instead of only the latest key winning.
    bool m_keyUp = false;
    bool m_keyDown = false;
    bool m_keyLeft = false;
    bool m_keyRight = false;
};
