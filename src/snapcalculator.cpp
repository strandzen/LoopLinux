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

SnapDirection snapDirectionForDelta(const QPointF &delta, qreal horizontalDeadzone, qreal verticalDeadzone)
{
    const qreal absX = std::abs(delta.x());
    const qreal absY = std::abs(delta.y());

    const bool hasHorizontal = absX >= horizontalDeadzone;
    const bool hasVertical = absY >= verticalDeadzone;
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

    return movingDown ? SnapDirection::Bottom : SnapDirection::Top;
}

QRectF snapTargetGeometry(SnapDirection direction, const QRectF &area, qreal paddingHorizontal, qreal paddingVertical)
{
    const qreal x = area.x();
    const qreal y = area.y();
    const qreal halfW = area.width() / 2.0;
    const qreal halfH = area.height() / 2.0;

    QRectF result;
    switch (direction) {
    case SnapDirection::Left:
        result = QRectF(x, y, halfW, area.height());
        break;
    case SnapDirection::Right:
        result = QRectF(x + halfW, y, halfW, area.height());
        break;
    case SnapDirection::Top:
        result = QRectF(x, y, area.width(), halfH);
        break;
    case SnapDirection::Bottom:
        result = QRectF(x, y + halfH, area.width(), halfH);
        break;
    case SnapDirection::TopLeft:
        result = QRectF(x, y, halfW, halfH);
        break;
    case SnapDirection::TopRight:
        result = QRectF(x + halfW, y, halfW, halfH);
        break;
    case SnapDirection::BottomLeft:
        result = QRectF(x, y + halfH, halfW, halfH);
        break;
    case SnapDirection::BottomRight:
        result = QRectF(x + halfW, y + halfH, halfW, halfH);
        break;
    case SnapDirection::None:
        return QRectF();
    }
    return result.adjusted(paddingHorizontal, paddingVertical, -paddingHorizontal, -paddingVertical);
}
