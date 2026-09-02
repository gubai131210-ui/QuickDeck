import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Window {
    id: root
    width: 640
    height: 420
    flags: Qt.FramelessWindowHint | Qt.WindowStaysOnTopHint
    color: "transparent"
    visible: launcher.visible
    x: (Screen.width - width) / 2
    y: Screen.height * 0.2

    property bool blurGraceActive: false

    opacity: chrome.opacityValue

    Behavior on opacity { NumberAnimation { duration: 140; easing.type: Easing.OutCubic } }

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

        Behavior on scale { NumberAnimation { duration: 140; easing.type: Easing.OutCubic } }

        Rectangle {
            anchors.fill: parent
            radius: 12
            color: Qt.rgba(0.12, 0.12, 0.12, 0.88)
            border.color: Qt.rgba(1, 1, 1, 0.08)

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 16
                spacing: 12

                TextField {
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

                Label {
                    text: launcher.modeValue === 0 ? qsTr("Applications") : qsTr("Clipboard")
                    color: "#cccccc"
                }

                ListView {
                    id: resultList
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    clip: true
                    model: launcher.modeValue === 0 ? launcher.appModel : launcher.clipboardModel
                    currentIndex: launcher.selectedIndex
                    onCurrentIndexChanged: launcher.selectedIndex = currentIndex
                    highlight: Rectangle {
                        color: Qt.rgba(1, 1, 1, 0.08)
                        radius: 6
                    }
                    highlightMoveDuration: 80
                    delegate: ItemDelegate {
                        width: resultList.width
                        highlighted: ListView.isCurrentItem
                        contentItem: Column {
                            spacing: 2
                            Label {
                                text: launcher.modeValue === 0 ? model.name : model.preview
                                color: "white"
                                elide: Text.ElideRight
                                width: parent.width
                            }
                            Label {
                                visible: launcher.modeValue === 0
                                text: model.path
                                color: "#888888"
                                font.pixelSize: 11
                                elide: Text.ElideMiddle
                                width: parent.width
                            }
                        }
                        onClicked: resultList.currentIndex = index
                    }
                }
            }
        }
    }

    Shortcut {
        sequences: [StandardKey.Cancel]
        onActivated: launcher.dismiss()
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
                chrome.scaleValue = 0.96
            }
        }

        function onHideRequested() {
            chrome.opacityValue = 0.0
            chrome.scaleValue = 0.96
            hideTimer.restart()
        }

        function onSelectedIndexChanged() {
            resultList.currentIndex = launcher.selectedIndex
        }
    }

    Timer {
        id: hideTimer
        interval: 150
        repeat: false
        onTriggered: launcher.confirm_hide()
    }

    onActiveChanged: {
        if (!active && launcher.visible && launcher.closeOnBlur && !blurGraceActive) {
            launcher.dismiss()
        }
    }
}
