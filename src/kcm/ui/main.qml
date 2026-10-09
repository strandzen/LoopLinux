import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Layouts
import org.kde.kirigami as Kirigami

// Tabbed instead of one long scrolling page — with every setting added over
// time (indicator, outline, colors, keybinds...) a single FormLayout no
// longer fit the dialog's default size at all (confirmed live: everything
// past "Indicator text" was being silently clipped). Each tab's own content
// is still wrapped in a ScrollView in case a future addition overflows a
// single tab too.
ColumnLayout {
    id: root
    implicitWidth: Kirigami.Units.gridUnit * 32
    implicitHeight: Kirigami.Units.gridUnit * 26
    spacing: 0

    Kirigami.Heading {
        Layout.fillWidth: true
        Layout.margins: Kirigami.Units.smallSpacing
        level: 1
        text: "Settings — Sirkel"
    }

    QQC2.TabBar {
        id: tabBar
        Layout.fillWidth: true

        // TabBar doesn't stretch its buttons to fill the bar on its own —
        // each one just sizes to its own text, left-aligned, leaving the
        // rest of the bar blank. Dividing the bar's width evenly spans it.
        QQC2.TabButton { text: "Behavior"; width: tabBar.width / tabBar.count }
        QQC2.TabButton { text: "Indicator"; width: tabBar.width / tabBar.count }
        QQC2.TabButton { text: "Outline"; width: tabBar.width / tabBar.count }
        QQC2.TabButton { text: "Colors"; width: tabBar.width / tabBar.count }
    }

    StackLayout {
        Layout.fillWidth: true
        Layout.fillHeight: true
        currentIndex: tabBar.currentIndex

        // --- Behavior -------------------------------------------------
        QQC2.ScrollView {
            contentWidth: availableWidth

            Kirigami.FormLayout {
                width: parent.width

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
                    Kirigami.FormData.label: "Keybinds"
                }

                QQC2.ComboBox {
                    Kirigami.FormData.label: "Movement keys:"
                    textRole: "text"
                    // "none" disables direction-key bindings entirely for
                    // users who only want to drag the mouse — see
                    // axisForArmedKey() in sirkeleffect.cpp.
                    model: kcm.keybindSchemes.map(scheme => ({
                        value: scheme,
                        text: ({ arrows: "Arrows", wasd: "WASD", hjkl: "HJKL", none: "None (mouse only)" })[scheme] || scheme
                    }))
                    currentIndex: kcm.keybindSchemes.indexOf(kcm.keybindScheme)
                    onActivated: kcm.keybindScheme = kcm.keybindSchemes[currentIndex]
                }
            }
        }

        // --- Indicator --------------------------------------------------
        QQC2.ScrollView {
            contentWidth: availableWidth

            Kirigami.FormLayout {
                width: parent.width

                QQC2.Switch {
                    Kirigami.FormData.label: "Visible:"
                    text: "Show the indicator"
                    checked: kcm.showIndicator
                    onToggled: kcm.showIndicator = checked
                }

                QQC2.Switch {
                    Kirigami.FormData.label: "Text:"
                    text: "Show direction name in indicator"
                    checked: kcm.showIndicatorText
                    onToggled: kcm.showIndicatorText = checked
                }

                QQC2.Switch {
                    Kirigami.FormData.label: "Position:"
                    text: "Spawn at cursor instead of screen center"
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

                QQC2.SpinBox {
                    Kirigami.FormData.label: "Pointer length (px):"
                    from: 6
                    to: 200
                    value: kcm.indicatorPointerLength
                    onValueModified: kcm.indicatorPointerLength = value
                }
            }
        }

        // --- Outline ------------------------------------------------------
        QQC2.ScrollView {
            contentWidth: availableWidth

            Kirigami.FormLayout {
                width: parent.width

                QQC2.Switch {
                    Kirigami.FormData.label: "Visible:"
                    text: "Show the window placement preview"
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

                QQC2.SpinBox {
                    Kirigami.FormData.label: "Fill opacity (%):"
                    from: 0
                    to: 100
                    value: kcm.outlineFillOpacity
                    onValueModified: kcm.outlineFillOpacity = value
                }

                QQC2.SpinBox {
                    Kirigami.FormData.label: "Animation duration (ms):"
                    from: 0
                    to: 1000
                    stepSize: 10
                    value: kcm.outlineAnimationDuration
                    onValueModified: kcm.outlineAnimationDuration = value

                    QQC2.ToolTip.text: "0 = instant (snaps straight to the new position/size). Higher values glide/grow the outline, and grow/shrink the indicator pointer into and out of maximize, more slowly."
                    QQC2.ToolTip.visible: hovered
                }
            }
        }

        // --- Colors ---------------------------------------------------
        QQC2.ScrollView {
            contentWidth: availableWidth

            Kirigami.FormLayout {
                width: parent.width

                Kirigami.Separator {
                    Kirigami.FormData.isSection: true
                    Kirigami.FormData.label: "Current color"
                }

                RowLayout {
                    Kirigami.FormData.label: "Outline / indicator:"
                    spacing: Kirigami.Units.largeSpacing

                    // A checkerboard behind the swatch so a transparent or
                    // semi-transparent pick is actually visible, not just a
                    // flat color that happens to look "washed out".
                    Item {
                        Layout.preferredWidth: Kirigami.Units.gridUnit * 3
                        Layout.preferredHeight: Kirigami.Units.gridUnit * 2

                        Grid {
                            anchors.fill: parent
                            columns: 6
                            rows: 3
                            Repeater {
                                model: 18
                                Rectangle {
                                    width: parent.width / 6
                                    height: parent.height / 3
                                    color: (Math.floor(index / 6) + (index % 6)) % 2 === 0 ? "#cccccc" : "#ffffff"
                                }
                            }
                        }

                        Rectangle {
                            anchors.fill: parent
                            color: kcm.outlineColor
                            border.width: 1
                            border.color: Kirigami.Theme.textColor
                        }

                        MouseArea {
                            anchors.fill: parent
                            onClicked: colorPopup.open()
                        }
                    }

                    QQC2.Button {
                        text: "Customize…"
                        onClicked: colorPopup.open()
                    }
                }

                Kirigami.Separator {
                    Kirigami.FormData.isSection: true
                    Kirigami.FormData.label: "Theme colors"
                }

                QQC2.ScrollView {
                    Kirigami.FormData.label: "Quick pick:"
                    implicitWidth: Kirigami.Units.gridUnit * 22
                    implicitHeight: Kirigami.Units.gridUnit * 11
                    clip: true

                    Column {
                        width: parent.width

                        // Kirigami.Theme's own color properties — these
                        // track the active Plasma color scheme live
                        // (they're not a one-time snapshot), so switching
                        // the system theme while this page is open updates
                        // the rows immediately.
                        Repeater {
                            model: [
                                { label: "Accent", color: Kirigami.Theme.highlightColor },
                                { label: "Text", color: Kirigami.Theme.textColor },
                                { label: "Disabled text", color: Kirigami.Theme.disabledTextColor },
                                { label: "Active text", color: Kirigami.Theme.activeTextColor },
                                { label: "Link", color: Kirigami.Theme.linkColor },
                                { label: "Visited link", color: Kirigami.Theme.visitedLinkColor },
                                { label: "Positive", color: Kirigami.Theme.positiveTextColor },
                                { label: "Neutral", color: Kirigami.Theme.neutralTextColor },
                                { label: "Negative", color: Kirigami.Theme.negativeTextColor },
                                { label: "Background", color: Kirigami.Theme.backgroundColor },
                                { label: "Alternate background", color: Kirigami.Theme.alternateBackgroundColor },
                                { label: "Hover", color: Kirigami.Theme.hoverColor },
                                { label: "Focus", color: Kirigami.Theme.focusColor },
                            ]

                            delegate: Rectangle {
                                id: delegateRoot
                                width: parent ? parent.width : 0
                                height: Kirigami.Units.gridUnit * 2
                                radius: 4
                                readonly property bool isSelected: kcm.outlineColor.toString() === modelData.color.toString()
                                color: isSelected ? Qt.rgba(Kirigami.Theme.highlightColor.r, Kirigami.Theme.highlightColor.g, Kirigami.Theme.highlightColor.b, 0.25)
                                     : rowHover.containsMouse ? Kirigami.Theme.hoverColor
                                     : "transparent"

                                RowLayout {
                                    anchors.fill: parent
                                    anchors.margins: Kirigami.Units.smallSpacing
                                    anchors.leftMargin: Kirigami.Units.largeSpacing
                                    anchors.rightMargin: Kirigami.Units.largeSpacing
                                    spacing: Kirigami.Units.largeSpacing

                                    Rectangle {
                                        Layout.preferredWidth: Kirigami.Units.gridUnit * 1.5
                                        Layout.preferredHeight: Kirigami.Units.gridUnit * 1.5
                                        radius: 4
                                        color: modelData.color
                                        border.width: delegateRoot.isSelected ? 2 : 1
                                        border.color: Kirigami.Theme.textColor
                                    }

                                    QQC2.Label {
                                        Layout.fillWidth: true
                                        text: modelData.label
                                        font.bold: delegateRoot.isSelected
                                    }

                                    QQC2.Label {
                                        text: modelData.color.toString().toUpperCase()
                                        opacity: 0.6
                                        font.pointSize: Kirigami.Theme.smallFont.pointSize
                                    }
                                }

                                MouseArea {
                                    id: rowHover
                                    anchors.fill: parent
                                    hoverEnabled: true
                                    onClicked: kcm.outlineColor = modelData.color
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    // Custom color picker — deliberately not QtQuick.Dialogs' ColorDialog,
    // which relies on a native/platform dialog backend that didn't reliably
    // show up at all when opened from inside an embedded KCM view. A Popup
    // renders directly inside this page's own window, so it always works
    // regardless of platform dialog support.
    QQC2.Popup {
        id: colorPopup
        parent: root
        x: (root.width - width) / 2
        y: (root.height - height) / 2
        width: Kirigami.Units.gridUnit * 18
        modal: true
        focus: true
        closePolicy: QQC2.Popup.CloseOnEscape | QQC2.Popup.CloseOnPressOutside

        property int red: 0
        property int green: 0
        property int blue: 0
        property int alpha: 255

        function syncFromConfig() {
            red = Math.round(kcm.outlineColor.r * 255);
            green = Math.round(kcm.outlineColor.g * 255);
            blue = Math.round(kcm.outlineColor.b * 255);
            alpha = Math.round(kcm.outlineColor.a * 255);
            hexField.text = toHex();
        }

        function commit() {
            kcm.outlineColor = Qt.rgba(red / 255, green / 255, blue / 255, alpha / 255);
        }

        function toHex() {
            function h(v) { return v.toString(16).padStart(2, "0"); }
            return "#" + h(red) + h(green) + h(blue) + h(alpha);
        }

        function fromHex(text) {
            const m = /^#?([0-9a-fA-F]{6}([0-9a-fA-F]{2})?)$/.exec(text.trim());
            if (!m) {
                return false;
            }
            const hex = m[1];
            red = parseInt(hex.substring(0, 2), 16);
            green = parseInt(hex.substring(2, 4), 16);
            blue = parseInt(hex.substring(4, 6), 16);
            alpha = hex.length === 8 ? parseInt(hex.substring(6, 8), 16) : 255;
            return true;
        }

        onOpened: syncFromConfig()

        ColumnLayout {
            anchors.fill: parent
            spacing: Kirigami.Units.smallSpacing

            Kirigami.Heading {
                level: 4
                text: "Custom color"
            }

            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: Kirigami.Units.gridUnit * 2
                radius: 4
                border.width: 1
                border.color: Kirigami.Theme.textColor
                color: Qt.rgba(colorPopup.red / 255, colorPopup.green / 255, colorPopup.blue / 255, colorPopup.alpha / 255)
            }

            RowLayout {
                Layout.fillWidth: true

                QQC2.Label { text: "Hex:" }

                QQC2.TextField {
                    id: hexField
                    Layout.fillWidth: true
                    placeholderText: "#RRGGBBAA"
                    onEditingFinished: {
                        if (colorPopup.fromHex(text)) {
                            colorPopup.commit();
                        } else {
                            text = colorPopup.toHex();
                        }
                    }
                }
            }

            RowLayout {
                Layout.fillWidth: true
                QQC2.Label { text: "R"; Layout.preferredWidth: Kirigami.Units.gridUnit }
                QQC2.Slider {
                    Layout.fillWidth: true
                    from: 0; to: 255; stepSize: 1
                    value: colorPopup.red
                    onMoved: {
                        colorPopup.red = Math.round(value);
                        colorPopup.commit();
                        hexField.text = colorPopup.toHex();
                    }
                }
                QQC2.Label { text: colorPopup.red; Layout.preferredWidth: Kirigami.Units.gridUnit * 1.5; horizontalAlignment: Text.AlignRight }
            }

            RowLayout {
                Layout.fillWidth: true
                QQC2.Label { text: "G"; Layout.preferredWidth: Kirigami.Units.gridUnit }
                QQC2.Slider {
                    Layout.fillWidth: true
                    from: 0; to: 255; stepSize: 1
                    value: colorPopup.green
                    onMoved: {
                        colorPopup.green = Math.round(value);
                        colorPopup.commit();
                        hexField.text = colorPopup.toHex();
                    }
                }
                QQC2.Label { text: colorPopup.green; Layout.preferredWidth: Kirigami.Units.gridUnit * 1.5; horizontalAlignment: Text.AlignRight }
            }

            RowLayout {
                Layout.fillWidth: true
                QQC2.Label { text: "B"; Layout.preferredWidth: Kirigami.Units.gridUnit }
                QQC2.Slider {
                    Layout.fillWidth: true
                    from: 0; to: 255; stepSize: 1
                    value: colorPopup.blue
                    onMoved: {
                        colorPopup.blue = Math.round(value);
                        colorPopup.commit();
                        hexField.text = colorPopup.toHex();
                    }
                }
                QQC2.Label { text: colorPopup.blue; Layout.preferredWidth: Kirigami.Units.gridUnit * 1.5; horizontalAlignment: Text.AlignRight }
            }

            RowLayout {
                Layout.fillWidth: true
                QQC2.Label { text: "A"; Layout.preferredWidth: Kirigami.Units.gridUnit }
                QQC2.Slider {
                    Layout.fillWidth: true
                    from: 0; to: 255; stepSize: 1
                    value: colorPopup.alpha
                    onMoved: {
                        colorPopup.alpha = Math.round(value);
                        colorPopup.commit();
                        hexField.text = colorPopup.toHex();
                    }
                }
                QQC2.Label { text: colorPopup.alpha; Layout.preferredWidth: Kirigami.Units.gridUnit * 1.5; horizontalAlignment: Text.AlignRight }
            }

            QQC2.DialogButtonBox {
                Layout.fillWidth: true
                standardButtons: QQC2.DialogButtonBox.Close
                onRejected: colorPopup.close()
                onAccepted: colorPopup.close()
            }
        }
    }

    // Settings here are instant-apply on disk, but the already-running
    // effect (kwin_wayland, a separate process from this KCM) only picks up
    // the change once it's reloaded. Shown under every tab, not just one,
    // since it's general information rather than tied to a category.
    Kirigami.InlineMessage {
        Layout.fillWidth: true
        Layout.margins: Kirigami.Units.smallSpacing
        visible: true
        type: Kirigami.MessageType.Information
        text: "Settings apply automatically the next time you hold the hyperkey — no need to log out or toggle the effect."
    }
}
