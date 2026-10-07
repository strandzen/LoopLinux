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
// components are each "armed" independently once they clear their own
// deadzone, so a diagonal drag can select a corner, and a pure vertical
// drag selects the top/bottom half.
SnapDirection snapDirectionForDelta(const QPointF &delta, qreal horizontalDeadzone, qreal verticalDeadzone);

// Maps a direction onto a concrete target geometry within the given usable
// screen area (the active window's screen only — windows never move across
// screens), inset by `paddingHorizontal` on the left/right edges and
// `paddingVertical` on the top/bottom edges. Returns an empty rect for
// SnapDirection::None.
QRectF snapTargetGeometry(SnapDirection direction, const QRectF &area, qreal paddingHorizontal = 0, qreal paddingVertical = 0);
