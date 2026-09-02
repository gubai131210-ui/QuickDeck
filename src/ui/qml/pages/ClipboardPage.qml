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
            title: model.preview
            subtitle: ""
            iconSource: ""
            isPinned: model.isPinned
            actionHint: qsTr("Enter · Ctrl+Enter paste")
            highlighted: ListView.isCurrentItem
            onClicked: resultListView.currentIndex = index

            opacity: 0
            transform: Translate { y: 8 }

            SequentialAnimation on opacity {
                running: true
                PauseAnimation { duration: Math.min(index * 24, 240) }
                NumberAnimation { to: 1; duration: QuickDeckTheme.animFast; easing.type: Easing.OutCubic }
            }

            SequentialAnimation on transform.y {
                running: true
                PauseAnimation { duration: Math.min(index * 24, 240) }
                NumberAnimation { to: 0; duration: QuickDeckTheme.animFast; easing.type: Easing.OutCubic }
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
