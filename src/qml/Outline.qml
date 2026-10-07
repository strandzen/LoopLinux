import QtQuick

// Preview of where the window will land. Lives in its own fully transparent
// QQuickWindow sized/positioned to exactly the target rect (see
// LoopLiteEffect::setDirection()), so only the border is visible — the
// live desktop shows through everywhere inside and outside it. borderWidth,
// borderColor, and cornerRadius are pushed from
// LoopLiteEffect::applyIndicatorStyle() (LoopLiteConfig::OutlineBorderWidth /
// OutlineColor / OutlineCornerRadius).
Item {
    id: root
    anchors.fill: parent

    property real borderWidth: 3
    property color borderColor: "#3daee9"
    property real cornerRadius: 6

    Rectangle {
        anchors.fill: parent
        anchors.margins: root.borderWidth / 2
        color: "transparent"
        border.color: root.borderColor
        border.width: root.borderWidth
        radius: root.cornerRadius
    }
}
