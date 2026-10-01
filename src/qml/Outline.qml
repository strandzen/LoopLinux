import QtQuick
import org.kde.kirigami as Kirigami

// Preview of where the window will land. Lives in its own fully transparent
// QQuickWindow sized/positioned to exactly the target rect (see
// LoopLiteEffect::setDirection()), so only the border is visible — the
// live desktop shows through everywhere inside and outside it.
// TODO: thickness/color/radius configurable per Todo.md's KCM wishlist.
Item {
    anchors.fill: parent

    Rectangle {
        anchors.fill: parent
        anchors.margins: 1
        color: "transparent"
        border.color: Kirigami.Theme.highlightColor
        border.width: 3
        radius: 6
    }
}
