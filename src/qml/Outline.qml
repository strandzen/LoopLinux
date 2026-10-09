import QtQuick

// Preview of where the window will land. Fills the shared Overlay.qml
// window (anchors.fill: parent, same as always — that window stays fixed
// for the whole session, sized/positioned to cover the target screen's
// entire usable area, set once by SirkelEffect::arm() via m_outlineArea)
// — only the inner Rectangle's geometry changes as the direction changes,
// in window-local coordinates (targetX/Y/Width/Height, pushed by
// SirkelEffect::setDirection()). That's what lets it glide/grow between
// directions via ordinary QML Behavior animations instead of an instant
// window move+resize, which can't be animated the same way. Declared
// before Indicator in Overlay.qml, so Indicator always paints on top of
// this, not the other way around — see Overlay.qml. Only the border (and,
// optionally, a translucent fill inside it) is visible — the live desktop
// shows through everywhere outside it, and through the inside too when
// fillOpacity is 0. borderWidth, borderColor, cornerRadius, fillOpacity,
// and animationDuration are pushed from
// SirkelEffect::applyIndicatorStyle() (SirkelConfig::OutlineBorderWidth /
// OutlineColor / OutlineCornerRadius / OutlineFillOpacity /
// OutlineAnimationDuration).
Item {
    id: root
    anchors.fill: parent

    property real borderWidth: 3
    property color borderColor: "#3daee9"
    property real cornerRadius: 6
    property real fillOpacity: 0
    property int animationDuration: 0

    // Window-local target rect for the highlighted area. SirkelEffect
    // resets this to a zero-size rect at the target screen's center (while
    // still hidden) at the start of every session, so the very first
    // direction picked grows out from the middle rather than gliding in
    // from wherever the previous session last left it.
    property real targetX: 0
    property real targetY: 0
    property real targetWidth: 0
    property real targetHeight: 0

    Behavior on targetX {
        enabled: root.animationDuration > 0
        NumberAnimation { duration: root.animationDuration; easing.type: Easing.OutCubic }
    }
    Behavior on targetY {
        enabled: root.animationDuration > 0
        NumberAnimation { duration: root.animationDuration; easing.type: Easing.OutCubic }
    }
    Behavior on targetWidth {
        enabled: root.animationDuration > 0
        NumberAnimation { duration: root.animationDuration; easing.type: Easing.OutCubic }
    }
    Behavior on targetHeight {
        enabled: root.animationDuration > 0
        NumberAnimation { duration: root.animationDuration; easing.type: Easing.OutCubic }
    }

    Rectangle {
        x: root.targetX + root.borderWidth / 2
        y: root.targetY + root.borderWidth / 2
        width: Math.max(0, root.targetWidth - root.borderWidth)
        height: Math.max(0, root.targetHeight - root.borderWidth)
        color: Qt.rgba(root.borderColor.r, root.borderColor.g, root.borderColor.b, root.fillOpacity / 100)
        border.color: root.borderColor
        border.width: root.borderWidth
        radius: root.cornerRadius
    }
}
