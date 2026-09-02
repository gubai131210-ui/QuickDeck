import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QuickDeckLauncher

Window {
    id: root
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
        property real scaleValue: launcher.visible ? 1.0 : 0.97
    }

    Item {
        id: chromeRoot
        anchors.fill: parent
        scale: chrome.scaleValue
        transformOrigin: Item.Center

        Behavior on scale { NumberAnimation { duration: QuickDeckTheme.animFast; easing.type: Easing.OutBack } }

        GlassPanel {
            anchors.fill: parent

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: QuickDeckTheme.spaceLg
                spacing: QuickDeckTheme.spaceMd

                RowLayout {
                    Layout.fillWidth: true
                    spacing: QuickDeckTheme.spaceSm

                    Rectangle {
                        Layout.preferredWidth: 36
                        Layout.preferredHeight: 36
                        radius: QuickDeckTheme.radiusPill
                        color: QuickDeckTheme.primarySoft

                        Label {
                            anchors.centerIn: parent
                            text: launcher.modeValue === 0 ? "⌕" : "⎘"
                            color: QuickDeckTheme.primary
                            font.pixelSize: 16
                        }
                    }

                    ColumnLayout {
                        Layout.fillWidth: true
                        spacing: 2

                        Label {
                            text: launcher.modeValue === 0
                                  ? qsTr("Search Mode")
                                  : qsTr("Clipboard Mode")
                            color: QuickDeckTheme.textPrimary
                            font.pixelSize: 16
                            font.weight: Font.DemiBold
                        }

                        Label {
                            text: launcher.modeValue === 0 ? qsTr("Applications") : qsTr("Clipboard")
                            color: QuickDeckTheme.textSecondary
                            font.pixelSize: 12
                        }
                    }

                    Label {
                        text: launcher.itemCount
                        color: QuickDeckTheme.textMuted
                        font.pixelSize: 12
                        padding: 8
                        background: Rectangle {
                            radius: QuickDeckTheme.radiusPill
                            color: QuickDeckTheme.fieldFill
                        }
                    }
                }

                SearchField {
                    id: searchField
                    Layout.fillWidth: true
                    placeholderText: launcher.modeValue === 0
                        ? qsTr("Search apps or paste a path...")
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
                    Keys.onReturnPressed: launcher.activate_selected(launcher.selectedIndex)
                    Keys.onEscapePressed: launcher.dismiss()
                }

                ListView {
                    id: resultList
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    clip: true
                    spacing: 6
                    model: launcher.modeValue === 0 ? launcher.appModel : launcher.clipboardModel
                    currentIndex: launcher.selectedIndex
                    onCurrentIndexChanged: launcher.selectedIndex = currentIndex

                    delegate: ResultRow {
                        title: launcher.modeValue === 0 ? model.name : model.preview
                        subtitle: launcher.modeValue === 0 ? model.subtitle : ""
                        iconSource: launcher.modeValue === 0 ? model.iconPath : ""
                        isPinned: model.isPinned
                        highlighted: ListView.isCurrentItem
                        onClicked: resultList.currentIndex = index
                    }

                    Label {
                        anchors.centerIn: parent
                        visible: resultList.count === 0
                        text: launcher.modeValue === 0
                              ? qsTr("No applications found")
                              : qsTr("No clipboard entries")
                        color: QuickDeckTheme.textMuted
                        font.pixelSize: 14
                    }
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
        enabled: launcher.visible && resultList.count > 0
        onActivated: launcher.toggle_pin_at(resultList.currentIndex)
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
                resultList.currentIndex = launcher.selectedIndex
                root.blurGraceActive = true
                blurGraceTimer.restart()
            } else {
                chrome.opacityValue = 0.0
                chrome.scaleValue = 0.97
            }
        }

        function onHideRequested() {
            chrome.opacityValue = 0.0
            chrome.scaleValue = 0.97
            hideTimer.restart()
        }

        function onSelectedIndexChanged() {
            resultList.currentIndex = launcher.selectedIndex
        }
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
