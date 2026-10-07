import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Layouts
import QtQuick.Dialogs
import org.kde.kirigami as Kirigami

// There are more settings here than fit in the dialog's default size
// (confirmed live — everything from "Indicator text" down was being
// silently clipped, not hidden by a layout bug), so the form needs to be
// inside a scroll view rather than assuming it all fits.
QQC2.ScrollView {
    id: root
    implicitWidth: Kirigami.Units.gridUnit * 30
    implicitHeight: Kirigami.Units.gridUnit * 24
    contentWidth: availableWidth

    Kirigami.FormLayout {
        width: root.availableWidth

        Kirigami.Heading {
            Layout.fillWidth: true
            level: 1
            text: "Settings — Loop Lite"
        }

        Kirigami.Separator {
            Kirigami.FormData.isSection: true
            Kirigami.FormData.label: "Mouse movement deadzone"
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
            Kirigami.FormData.label: "Padding"
        }

        QQC2.SpinBox {
            Kirigami.FormData.label: "Horizontal (px):"
            from: 0
            to: 200
            value: kcm.paddingHorizontal
            onValueModified: kcm.paddingHorizontal = value
        }

        QQC2.SpinBox {
            Kirigami.FormData.label: "Vertical (px):"
            from: 0
            to: 200
            value: kcm.paddingVertical
            onValueModified: kcm.paddingVertical = value
        }

        Kirigami.Separator {
            Kirigami.FormData.isSection: true
            Kirigami.FormData.label: "Indicator"
        }

        QQC2.Switch {
            Kirigami.FormData.label: "Visible:"
            text: "Show the center direction indicator"
            checked: kcm.showIndicator
            onToggled: kcm.showIndicator = checked
        }

        QQC2.Switch {
            Kirigami.FormData.label: "Text:"
            text: "Show direction name in the center indicator"
            checked: kcm.showIndicatorText
            onToggled: kcm.showIndicatorText = checked
        }

        QQC2.Switch {
            Kirigami.FormData.label: "Position:"
            text: "Spawn at the mouse cursor instead of the screen's center"
            checked: kcm.indicatorFollowsMouse
            onToggled: kcm.indicatorFollowsMouse = checked
        }

        QQC2.SpinBox {
            Kirigami.FormData.label: "Size (px):"
            from: 60
            to: 400
            value: kcm.indicatorSize
            onValueModified: kcm.indicatorSize = value
        }

        QQC2.SpinBox {
            Kirigami.FormData.label: "Corner radius (px):"
            from: 0
            to: kcm.indicatorSize / 2
            value: kcm.indicatorCornerRadius
            onValueModified: kcm.indicatorCornerRadius = value
        }

        QQC2.SpinBox {
            Kirigami.FormData.label: "Line thickness (px):"
            from: 2
            to: 60
            value: kcm.indicatorRingWidth
            onValueModified: kcm.indicatorRingWidth = value
        }

        Kirigami.Separator {
            Kirigami.FormData.isSection: true
            Kirigami.FormData.label: "Window Outline"
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

        Kirigami.Separator {
            Kirigami.FormData.isSection: true
            Kirigami.FormData.label: "Colors"
        }

        RowLayout {
            Kirigami.FormData.label: "Outline / indicator color:"

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

            // ColorDialog's own panel includes a hex/RGB entry field
            // alongside the color wheel, covering "choose via hex".
            QQC2.Button {
                text: "Choose…"
                onClicked: colorDialog.open()
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
            Kirigami.FormData.label: "Movement keys:"
            model: kcm.keybindSchemes
            currentIndex: kcm.keybindSchemes.indexOf(kcm.keybindScheme)
            onActivated: kcm.keybindScheme = kcm.keybindSchemes[currentIndex]
        }

        Kirigami.Separator {
            Kirigami.FormData.isSection: true
        }

        // Settings here are instant-apply on disk, but the already-running
        // effect (kwin_wayland, a separate process from this KCM) only
        // picks up the change once it's reloaded.
        Kirigami.InlineMessage {
            Layout.fillWidth: true
            visible: true
            type: Kirigami.MessageType.Information
            text: "Settings apply automatically the next time you hold the hyperkey — no need to log out or toggle the effect."
        }
    }
}
