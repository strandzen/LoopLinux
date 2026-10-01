import QtQuick

// Small always-near-center indicator showing the currently selected
// direction. Lives in its own small, fully transparent QQuickWindow (see
// LoopLiteEffect) so the circle's rounded shape doesn't show up as a black
// square against the desktop.
Item {
    id: root
    anchors.fill: parent

    property string direction: "none"

    Rectangle {
        anchors.fill: parent
        anchors.margins: 2
        radius: width / 2
        color: "#2e3440"
        opacity: 0.85
        border.color: "#88c0d0"
        border.width: 2

        Text {
            anchors.centerIn: parent
            color: "white"
            font.pixelSize: 18
            text: root.direction
        }
    }
}
