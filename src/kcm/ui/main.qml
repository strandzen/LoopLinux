import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Layouts
import QtQuick.Dialogs
import org.kde.kirigami as Kirigami

Kirigami.FormLayout {
    id: root
    implicitWidth: Kirigami.Units.gridUnit * 30
    implicitHeight: Kirigami.Units.gridUnit * 24

    QQC2.Switch {
        Kirigami.FormData.label: "Top/bottom halves:"
        text: "Straight up/down drag selects a half"
        checked: kcm.enableTopBottomHalves
        onToggled: kcm.enableTopBottomHalves = checked
    }

    Kirigami.Separator {
        Kirigami.FormData.isSection: true
        Kirigami.FormData.label: "Mouse deadzone"
    }

    QQC2.SpinBox {
        Kirigami.FormData.label: "Horizontal (px):"
        from: 0
        to: 500
        value: kcm.horizontalDeadzone
        onValueModified: kcm.horizontalDeadzone = value
    }

    QQC2.SpinBox {
        Kirigami.FormData.label: "Vertical (px):"
        from: 0
        to: 500
        value: kcm.verticalDeadzone
        onValueModified: kcm.verticalDeadzone = value
    }

    Kirigami.Separator {
        Kirigami.FormData.isSection: true
        Kirigami.FormData.label: "Snapped window"
    }

    QQC2.SpinBox {
        Kirigami.FormData.label: "Padding (px):"
        from: 0
        to: 200
        value: kcm.windowPadding
        onValueModified: kcm.windowPadding = value
    }

    Kirigami.Separator {
        Kirigami.FormData.isSection: true
        Kirigami.FormData.label: "Center indicator"
    }

    QQC2.Switch {
        Kirigami.FormData.label: "Visible:"
        text: "Show the center direction indicator"
        checked: kcm.showIndicator
        onToggled: kcm.showIndicator = checked
    }

    QQC2.Switch {
        Kirigami.FormData.label: "Position:"
        text: "Follow the mouse cursor instead of staying centered"
        checked: kcm.indicatorFollowsMouse
        onToggled: kcm.indicatorFollowsMouse = checked
    }

    QQC2.SpinBox {
        Kirigami.FormData.label: "Corner radius (px):"
        from: 0
        to: 110
        value: kcm.indicatorCornerRadius
        onValueModified: kcm.indicatorCornerRadius = value
    }

    QQC2.SpinBox {
        Kirigami.FormData.label: "Ring thickness (px):"
        from: 2
        to: 60
        value: kcm.indicatorRingWidth
        onValueModified: kcm.indicatorRingWidth = value
    }

    QQC2.Switch {
        Kirigami.FormData.label: "Text:"
        text: "Show direction name in the center indicator"
        checked: kcm.showIndicatorText
        onToggled: kcm.showIndicatorText = checked
    }

    Kirigami.Separator {
        Kirigami.FormData.isSection: true
        Kirigami.FormData.label: "Target outline"
    }

    QQC2.Switch {
        Kirigami.FormData.label: "Visible:"
        text: "Show the target-outline preview"
        checked: kcm.showOutline
        onToggled: kcm.showOutline = checked
    }

    QQC2.SpinBox {
        Kirigami.FormData.label: "Border thickness (px):"
        from: 1
        to: 20
        value: kcm.outlineBorderWidth
        onValueModified: kcm.outlineBorderWidth = value
    }

    QQC2.SpinBox {
        Kirigami.FormData.label: "Corner radius (px):"
        from: 0
        to: 60
        value: kcm.outlineCornerRadius
        onValueModified: kcm.outlineCornerRadius = value
    }

    ColumnLayout {
        Kirigami.FormData.label: "Color:"

        RowLayout {
            // Quick-pick swatches from the system theme, alongside a fully
            // custom color (with transparency) via the dialog below.
            Repeater {
                model: [
                    { label: "Accent", color: Kirigami.Theme.highlightColor },
                    { label: "Text", color: Kirigami.Theme.textColor },
                    { label: "Positive", color: Kirigami.Theme.positiveTextColor },
                    { label: "Neutral", color: Kirigami.Theme.neutralTextColor },
                    { label: "Negative", color: Kirigami.Theme.negativeTextColor },
                ]

                Rectangle {
                    Layout.preferredWidth: Kirigami.Units.gridUnit
                    Layout.preferredHeight: Kirigami.Units.gridUnit
                    radius: 4
                    color: modelData.color
                    border.width: kcm.outlineColor === modelData.color ? 2 : 1
                    border.color: Kirigami.Theme.textColor

                    QQC2.ToolTip.text: modelData.label
                    QQC2.ToolTip.visible: swatchArea.containsMouse

                    MouseArea {
                        id: swatchArea
                        anchors.fill: parent
                        hoverEnabled: true
                        onClicked: kcm.outlineColor = modelData.color
                    }
                }
            }

            Rectangle {
                Layout.preferredWidth: Kirigami.Units.gridUnit * 2
                Layout.preferredHeight: Kirigami.Units.gridUnit
                radius: 4
                color: kcm.outlineColor
                border.width: 1
                border.color: Kirigami.Theme.textColor

                MouseArea {
                    anchors.fill: parent
                    onClicked: colorDialog.open()
                }
            }

            QQC2.Button {
                text: "Choose…"
                onClicked: colorDialog.open()
            }
        }
    }

    ColorDialog {
        id: colorDialog
        options: ColorDialog.ShowAlphaChannel
        selectedColor: kcm.outlineColor
        onAccepted: kcm.outlineColor = selectedColor
    }

    Kirigami.Separator {
        Kirigami.FormData.isSection: true
        Kirigami.FormData.label: "Keybinds"
    }

    QQC2.ComboBox {
        Kirigami.FormData.label: "Direction keys while armed:"
        model: kcm.keybindSchemes
        currentIndex: kcm.keybindSchemes.indexOf(kcm.keybindScheme)
        onActivated: kcm.keybindScheme = kcm.keybindSchemes[currentIndex]
    }
}
