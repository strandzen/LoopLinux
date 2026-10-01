#pragma once

#include "snapcalculator.h"

#include <effect/effect.h>

#include <QPointF>

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
class LoopLiteEffect : public KWin::Effect
{
    Q_OBJECT

public:
    explicit LoopLiteEffect();
    ~LoopLiteEffect() override;

    static bool supported();
    bool isActive() const override;

protected:
    void grabbedKeyboardEvent(QKeyEvent *event) override;
    void pointerMotion(KWin::PointerMotionEvent *event) override;

private:
    void arm();
    void finish(bool applyPendingDirection);
    void updateDirectionFromPointer();
    void setDirection(SnapDirection direction);

    // Hyperkey+<key> actions available while armed. To add a new binding,
    // add a case in grabbedKeyboardEvent's key-action table in the .cpp.
    void actionMaximize();
    void actionMinimize();

    QAction *m_toggleAction = nullptr;
    std::unique_ptr<QQuickView> m_outline;
    std::unique_ptr<QQuickView> m_indicator;
    KWin::EffectWindow *m_targetWindow = nullptr;
    KWin::LogicalOutput *m_targetScreen = nullptr;
    QPointF m_accumulatedDelta;
    SnapDirection m_direction = SnapDirection::None;
    bool m_active = false;
};
