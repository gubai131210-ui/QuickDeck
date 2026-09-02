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
        currentIndex: launcher.selectedIndex
        onCurrentIndexChanged: launcher.selectedIndex = currentIndex

        property string listKey: launcher.commandMode ? "command" : "app"

        NumberAnimation on opacity {
            id: fadeIn
            from: 0
            to: 1
            duration: QuickDeckTheme.animNormal
            easing.type: Easing.OutCubic
        }

        onListKeyChanged: fadeIn.restart()

        model: launcher.commandMode ? launcher.commandModel : launcher.appModel

        delegate: ResultRow {
            title: launcher.commandMode ? model.title : model.name
            subtitle: launcher.commandMode ? model.subtitle : model.subtitle
            iconSource: launcher.commandMode ? "" : model.iconPath
            isPinned: launcher.commandMode ? false : model.isPinned
            actionHint: launcher.commandMode ? model.actionHint : qsTr("Enter")
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
            text: launcher.commandMode ? qsTr("Type > to run commands")
                                       : (launcher.query.length > 0 ? qsTr("No applications found")
                                                                      : qsTr("Search apps or type > for commands"))
            color: QuickDeckTheme.textMuted
            font.pixelSize: 14
            horizontalAlignment: Text.AlignHCenter
            wrapMode: Text.WordWrap
            width: parent.width * 0.8
        }
    }
}
