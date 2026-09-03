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

        delegate: Item {
            id: rowHost
            width: resultListView.width
            height: 56
            opacity: 0
            property real slideOffset: 8
            y: slideOffset

            ResultRow {
                anchors.fill: parent
                title: model.preview
                subtitle: ""
                iconSource: ""
                isPinned: model.isPinned
                actionHint: qsTr("Enter · Ctrl+Enter paste")
                highlighted: ListView.isCurrentItem
                onClicked: resultListView.currentIndex = index
            }

            SequentialAnimation {
                running: true
                PauseAnimation { duration: Math.min(index * 24, 240) }
                ParallelAnimation {
                    NumberAnimation {
                        target: rowHost
                        property: "opacity"
                        to: 1
                        duration: QuickDeckTheme.animFast
                        easing.type: Easing.OutCubic
                    }
                    NumberAnimation {
                        target: rowHost
                        property: "slideOffset"
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
