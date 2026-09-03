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
        model: launcher.commandMode ? launcher.commandModel : launcher.appModel

        NumberAnimation on opacity {
            id: fadeIn
            from: 0
            to: 1
            duration: QuickDeckTheme.animNormal
            easing.type: Easing.OutCubic
        }

        property string listKey: launcher.commandMode ? "command" : "app"
        onListKeyChanged: fadeIn.restart()

        delegate: ResultRow {
            id: row
            required property int index
            required property var model
            width: resultListView.width
            title: launcher.commandMode ? (model.title ?? "") : (model.name ?? "")
            subtitle: model.subtitle ?? ""
            iconSource: launcher.commandMode ? "" : (model.iconPath ?? "")
            isPinned: launcher.commandMode ? false : (model.isPinned ?? false)
            actionHint: launcher.commandMode ? (model.actionHint ?? "") : qsTr("Enter")
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
