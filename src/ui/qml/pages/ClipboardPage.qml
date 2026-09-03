import QtQuick
import QtQuick.Controls
import QuickDeckLauncher

Item {
    id: root

    ListView {
        id: resultListView
        anchors.fill: parent
        clip: true
        spacing: 6
        model: launcher.clipboardModel
        currentIndex: launcher.selectedIndex
        onCurrentIndexChanged: launcher.selectedIndex = currentIndex

        NumberAnimation on opacity {
            id: fadeIn
            from: 0
            to: 1
            duration: QuickDeckTheme.animNormal
            easing.type: Easing.OutCubic
        }

        Component.onCompleted: fadeIn.restart()

        delegate: ResultRow {
            id: row
            required property int index
            required property var model
            width: resultListView.width
            title: model.preview ?? ""
            subtitle: ""
            iconSource: ""
            isPinned: model.isPinned ?? false
            actionHint: qsTr("Enter · Ctrl+Enter paste")
            highlighted: resultListView.currentIndex === index
            opacity: 0
            onClicked: resultListView.currentIndex = index

            transform: Translate {
                id: slide
                y: 8
            }

            SequentialAnimation {
                running: true
                PauseAnimation {
                    duration: Math.min(Math.max(row.index, 0) * 24, 240)
                }
                ParallelAnimation {
                    NumberAnimation {
                        target: row
                        property: "opacity"
                        to: 1
                        duration: QuickDeckTheme.animFast
                        easing.type: Easing.OutCubic
                    }
                    NumberAnimation {
                        target: slide
                        property: "y"
                        to: 0
                        duration: QuickDeckTheme.animFast
                        easing.type: Easing.OutCubic
                    }
                }
            }
        }

        Label {
            anchors.centerIn: parent
            visible: resultListView.count === 0
            text: qsTr("No clipboard entries")
            color: QuickDeckTheme.textMuted
            font.pixelSize: 14
        }
    }
}
