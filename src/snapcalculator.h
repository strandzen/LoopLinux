#pragma once

#include <QRectF>

enum class SnapDirection {
    None,
    Left,
    Right,
    Top,
    Bottom,
    TopLeft,
    TopRight,
    BottomLeft,
    BottomRight,
};

QString snapDirectionName(SnapDirection direction);

// Decides which direction the pointer has dragged towards, given its total
// displacement since the hyperkey was pressed. Horizontal and vertical
// components are each "armed" independently once they clear the deadzone,
// so a diagonal drag can select a corner. enableTopBottomHalves gates a
// pure vertical drag mapping to the top/bottom half (see spec: "not yet
// decided" whether straight up/down should do that).
SnapDirection snapDirectionForDelta(const QPointF &delta, qreal deadzone, bool enableTopBottomHalves);

// Maps a direction onto a concrete target geometry within the given usable
// screen area (the active window's screen only — windows never move across
// screens). Returns an empty rect for SnapDirection::None.
QRectF snapTargetGeometry(SnapDirection direction, const QRectF &area);
