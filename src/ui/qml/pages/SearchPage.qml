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

        delegate: Item {
            id: rowHost
            width: resultListView.width
            height: 56
            opacity: 0
            property real slideOffset: 8
            y: slideOffset

            ResultRow {
                anchors.fill: parent
                title: launcher.commandMode ? model.title : model.name
                subtitle: launcher.commandMode ? model.subtitle : model.subtitle
                iconSource: launcher.commandMode ? "" : model.iconPath
                isPinned: launcher.commandMode ? false : model.isPinned
                actionHint: launcher.commandMode ? model.actionHint : qsTr("Enter")
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
