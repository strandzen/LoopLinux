#include "snapcalculator.h"

#include <cmath>

QString snapDirectionName(SnapDirection direction)
{
    switch (direction) {
    case SnapDirection::Left:
        return QStringLiteral("left");
    case SnapDirection::Right:
        return QStringLiteral("right");
    case SnapDirection::Top:
        return QStringLiteral("top");
    case SnapDirection::Bottom:
        return QStringLiteral("bottom");
    case SnapDirection::TopLeft:
        return QStringLiteral("topLeft");
    case SnapDirection::TopRight:
        return QStringLiteral("topRight");
    case SnapDirection::BottomLeft:
        return QStringLiteral("bottomLeft");
    case SnapDirection::BottomRight:
        return QStringLiteral("bottomRight");
    case SnapDirection::None:
        break;
    }
    return QStringLiteral("none");
}

SnapDirection snapDirectionForDelta(const QPointF &delta, qreal deadzone, bool enableTopBottomHalves)
{
    const qreal absX = std::abs(delta.x());
    const qreal absY = std::abs(delta.y());

    const bool hasHorizontal = absX >= deadzone;
    const bool hasVertical = absY >= deadzone;
    if (!hasHorizontal && !hasVertical) {
        return SnapDirection::None;
    }

    const bool movingRight = delta.x() > 0;
    const bool movingDown = delta.y() > 0;

    if (hasHorizontal && hasVertical) {
        if (movingRight) {
            return movingDown ? SnapDirection::BottomRight : SnapDirection::TopRight;
        }
        return movingDown ? SnapDirection::BottomLeft : SnapDirection::TopLeft;
    }

    if (hasHorizontal) {
        return movingRight ? SnapDirection::Right : SnapDirection::Left;
    }

    if (!enableTopBottomHalves) {
        return SnapDirection::None;
    }
    return movingDown ? SnapDirection::Bottom : SnapDirection::Top;
}

QRectF snapTargetGeometry(SnapDirection direction, const QRectF &area)
{
    const qreal x = area.x();
    const qreal y = area.y();
    const qreal halfW = area.width() / 2.0;
    const qreal halfH = area.height() / 2.0;

    switch (direction) {
    case SnapDirection::Left:
        return QRectF(x, y, halfW, area.height());
    case SnapDirection::Right:
        return QRectF(x + halfW, y, halfW, area.height());
    case SnapDirection::Top:
        return QRectF(x, y, area.width(), halfH);
    case SnapDirection::Bottom:
        return QRectF(x, y + halfH, area.width(), halfH);
    case SnapDirection::TopLeft:
        return QRectF(x, y, halfW, halfH);
    case SnapDirection::TopRight:
        return QRectF(x + halfW, y, halfW, halfH);
    case SnapDirection::BottomLeft:
        return QRectF(x, y + halfH, halfW, halfH);
    case SnapDirection::BottomRight:
        return QRectF(x + halfW, y + halfH, halfW, halfH);
    case SnapDirection::None:
        break;
    }
    return QRectF();
}
