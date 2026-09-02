import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QuickDeckLauncher

Window {
    id: root
    objectName: "settingsOverlay"
    width: 560
    height: 520
    flags: Qt.FramelessWindowHint | Qt.WindowStaysOnTopHint
    color: "transparent"
    visible: settings.visible
    x: (Screen.width - width) / 2
    y: (Screen.height - height) / 2

    opacity: settings.visible ? 1.0 : 0.0
    scale: settings.visible ? 1.0 : 0.96

    Behavior on opacity { NumberAnimation { duration: QuickDeckTheme.animNormal } }
    Behavior on scale { NumberAnimation { duration: QuickDeckTheme.animNormal; easing.type: Easing.OutBack } }

    GlassPanel {
        anchors.fill: parent

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: QuickDeckTheme.spaceLg
            spacing: QuickDeckTheme.spaceMd

            Label {
                text: qsTr("QuickDeck Settings")
                color: QuickDeckTheme.textPrimary
                font.pixelSize: 18
                font.weight: Font.DemiBold
            }

            Row {
                Layout.fillWidth: true
                spacing: QuickDeckTheme.spaceSm

                Repeater {
                    model: [qsTr("General"), qsTr("Hotkeys"), qsTr("Clipboard")]
                    delegate: ItemDelegate {
                        required property int index
                        required property string modelData
                        width: implicitWidth + 24
                        height: 34
                        padding: 8

                        background: Rectangle {
                            radius: QuickDeckTheme.radiusPill
                            color: settings.currentTab === index ? QuickDeckTheme.primarySoft
                                                                 : QuickDeckTheme.fieldFill
                        }

                        contentItem: Label {
                            text: modelData
                            color: settings.currentTab === index ? QuickDeckTheme.primary
                                                                   : QuickDeckTheme.textSecondary
                            font.pixelSize: 13
                            horizontalAlignment: Text.AlignHCenter
                        }

                        onClicked: settings.currentTab = index
                    }
                }
            }

            StackLayout {
                Layout.fillWidth: true
                Layout.fillHeight: true
                currentIndex: settings.currentTab

                ScrollView {
                    clip: true
                    ColumnLayout {
                        width: parent.width
                        spacing: QuickDeckTheme.spaceMd

                        Label { text: qsTr("Language"); color: QuickDeckTheme.textSecondary; font.pixelSize: 12 }
                        ComboBox {
                            id: languageCombo
                            Layout.fillWidth: true
                            textRole: "display"
                            valueRole: "code"

                            Component.onCompleted: {
                                const codes = settings.supported_languages()
                                languageCombo.model = []
                                const rows = []
                                let selected = 0
                                for (let i = 0; i < codes.length; ++i) {
                                    rows.push({
                                        code: codes[i],
                                        display: settings.language_display_name(codes[i])
                                    })
                                    if (codes[i] === settings.language)
                                        selected = i
                                }
                                languageCombo.model = rows
                                languageCombo.currentIndex = selected
                            }

                            onActivated: function(index) {
                                settings.language = languageCombo.model[index].code
                            }
                        }

                        CheckBox {
                            text: qsTr("Start at login")
                            checked: settings.autoStart
                            onToggled: settings.autoStart = checked
                        }
                        CheckBox {
                            text: qsTr("Close overlay when focus is lost")
                            checked: settings.closeOnBlur
                            onToggled: settings.closeOnBlur = checked
                        }
                        CheckBox {
                            text: qsTr("Enable debug logging")
                            checked: settings.debugLog
                            onToggled: settings.debugLog = checked
                        }

                        Button {
                            text: qsTr("Refresh application index")
                            onClicked: settings.refresh_index()
                        }
                    }
                }

                ScrollView {
                    clip: true
                    ColumnLayout {
                        width: parent.width
                        spacing: QuickDeckTheme.spaceMd

                        Label { text: qsTr("Launcher hotkey"); color: QuickDeckTheme.textSecondary; font.pixelSize: 12 }
                        TextField {
                            Layout.fillWidth: true
                            text: settings.launcherHotkey
                            placeholderText: "Alt+Space"
                            onEditingFinished: settings.launcherHotkey = text
                        }

                        Label { text: qsTr("Clipboard hotkey"); color: QuickDeckTheme.textSecondary; font.pixelSize: 12 }
                        TextField {
                            Layout.fillWidth: true
                            text: settings.clipboardHotkey
                            placeholderText: "Ctrl+Shift+V"
                            onEditingFinished: settings.clipboardHotkey = text
                        }

                        Label { text: qsTr("Quick paste hotkey"); color: QuickDeckTheme.textSecondary; font.pixelSize: 12 }
                        TextField {
                            Layout.fillWidth: true
                            text: settings.quickPasteHotkey
                            placeholderText: "Ctrl+Alt+V"
                            onEditingFinished: settings.quickPasteHotkey = text
                        }
                    }
                }

                ScrollView {
                    clip: true
                    ColumnLayout {
                        width: parent.width
                        spacing: QuickDeckTheme.spaceMd

                        Label { text: qsTr("Max entries"); color: QuickDeckTheme.textSecondary; font.pixelSize: 12 }
                        SpinBox {
                            from: 10
                            to: 10000
                            value: settings.maxEntries
                            onValueChanged: settings.maxEntries = value
                        }

                        Label { text: qsTr("Max characters"); color: QuickDeckTheme.textSecondary; font.pixelSize: 12 }
                        SpinBox {
                            from: 100
                            to: 1000000
                            value: settings.maxChars
                            onValueChanged: settings.maxChars = value
                        }

                        CheckBox {
                            text: qsTr("Enable clipboard monitoring")
                            checked: settings.monitoringEnabled
                            onToggled: settings.monitoringEnabled = checked
                        }
                        CheckBox {
                            text: qsTr("Simulate paste when re-copying from overlay")
                            checked: settings.simulatePasteOnActivate
                            onToggled: settings.simulatePasteOnActivate = checked
                        }
                    }
                }
            }

            RowLayout {
                Layout.fillWidth: true
                Item { Layout.fillWidth: true }
                Button {
                    text: qsTr("Cancel")
                    onClicked: settings.hide()
                }
                Button {
                    text: qsTr("Save")
                    highlighted: true
                    onClicked: settings.save()
                }
            }
        }
    }

    Shortcut {
        sequences: [StandardKey.Cancel]
        onActivated: settings.hide()
    }
}
