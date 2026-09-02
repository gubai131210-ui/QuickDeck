import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QuickDeckLauncher

Window {
    id: root
    objectName: "launcherOverlay"
    width: 680
    height: 460
    flags: Qt.FramelessWindowHint | Qt.WindowStaysOnTopHint
    color: "transparent"
    visible: launcher.visible
    x: (Screen.width - width) / 2
    y: Screen.height * 0.18

    property bool blurGraceActive: false

    opacity: chrome.opacityValue

    Behavior on opacity { NumberAnimation { duration: QuickDeckTheme.animFast; easing.type: Easing.OutCubic } }

    QtObject {
        id: chrome
        property real opacityValue: launcher.visible ? 1.0 : 0.0
        property real scaleValue: launcher.visible ? 1.0 : 0.96
    }

    Item {
        id: chromeRoot
        anchors.fill: parent
        scale: chrome.scaleValue
        transformOrigin: Item.Center

        Behavior on scale { NumberAnimation { duration: QuickDeckTheme.animNormal; easing.type: Easing.OutBack } }

        GlassPanel {
            anchors.fill: parent

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: QuickDeckTheme.spaceLg
                spacing: QuickDeckTheme.spaceMd

                ModeSwitcher {
                    Layout.alignment: Qt.AlignHCenter
                    modeValue: launcher.modeValue
                    onModeSelected: function(modeValue) { launcher.switch_mode(modeValue) }
                }

                SearchField {
                    id: searchField
                    Layout.fillWidth: true
                    placeholderText: launcher.modeValue === 0
                        ? qsTr("Search apps or type > for commands...")
                        : qsTr("Filter clipboard history...")
                    text: launcher.query
                    onTextChanged: launcher.query = text

                    Keys.onUpPressed: function(event) {
                        event.accepted = true
                        launcher.move_selection(-1)
                    }
                    Keys.onDownPressed: function(event) {
                        event.accepted = true
                        launcher.move_selection(1)
                    }
                    Keys.onReturnPressed: launcher.activate_selected(launcher.selectedIndex, false)
                    Keys.onEscapePressed: launcher.dismiss()

                    Keys.onPressed: function(event) {
                        if ((event.modifiers & Qt.ControlModifier) && event.key === Qt.Key_Return) {
                            event.accepted = true
                            launcher.activate_selected(launcher.selectedIndex, true)
                        }
                        if (event.key === Qt.Key_Tab) {
                            event.accepted = true
                            const nextMode = launcher.modeValue === 0 ? 1 : 0
                            launcher.switch_mode(nextMode)
                        }
                        if (event.key === Qt.Key_Delete && launcher.modeValue === 1) {
                            event.accepted = true
                            launcher.delete_selected_at(launcher.selectedIndex)
                        }
                    }
                }

                StackLayout {
                    id: pageStack
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    currentIndex: launcher.modeValue

                    SearchPage {}
                    ClipboardPage {}
                }
            }
        }
    }

    Shortcut {
        sequences: [StandardKey.Cancel]
        onActivated: launcher.dismiss()
    }

    Shortcut {
        sequence: "Ctrl+Shift+P"
        enabled: launcher.visible && launcher.itemCount > 0
        onActivated: launcher.toggle_pin_at(launcher.selectedIndex)
    }

    Timer {
        id: blurGraceTimer
        interval: 120
        repeat: false
        onTriggered: root.blurGraceActive = false
    }

    Connections {
        target: launcher
        function onVisibleChanged() {
            if (launcher.visible) {
                chrome.opacityValue = 1.0
                chrome.scaleValue = 1.0
                searchField.text = launcher.query
                root.requestActivate()
                searchField.forceActiveFocus()
                root.blurGraceActive = true
                blurGraceTimer.restart()
            } else {
                chrome.opacityValue = 0.0
                chrome.scaleValue = 0.96
            }
        }

        function onHideRequested() {
            chrome.opacityValue = 0.0
            chrome.scaleValue = 0.96
            hideTimer.restart()
        }

        function onSelectedIndexChanged() {
            // child ListViews bind selectedIndex directly
        }

        function onModeChanged() {
            pageStack.opacity = 0
            pageFade.restart()
        }
    }

    NumberAnimation {
        id: pageFade
        target: pageStack
        property: "opacity"
        from: 0
        to: 1
        duration: QuickDeckTheme.animNormal
        easing.type: Easing.OutCubic
    }

    Timer {
        id: hideTimer
        interval: QuickDeckTheme.animNormal
        repeat: false
        onTriggered: launcher.confirm_hide()
    }

    onActiveChanged: {
        if (!active && launcher.visible && launcher.closeOnBlur && !blurGraceActive) {
            launcher.dismiss()
        }
    }
}
