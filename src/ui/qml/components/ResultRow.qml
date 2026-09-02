import QtQuick
import QtQuick.Controls
import QuickDeckLauncher

ItemDelegate {
    id: root
    width: ListView.view ? ListView.view.width : implicitWidth
    height: 56
    padding: 12

    background: Rectangle {
        radius: QuickDeckTheme.radiusControl
        color: root.highlighted ? QuickDeckTheme.highlight
                                  : (root.hovered ? "#1438BDF8" : "transparent")
        Behavior on color { ColorAnimation { duration: QuickDeckTheme.animFast } }
    }

    contentItem: Row {
        spacing: QuickDeckTheme.spaceSm

        Item {
            width: 34
            height: 34
            anchors.verticalCenter: parent.verticalCenter

            Image {
                id: iconImage
                anchors.fill: parent
                visible: root.iconSource.length > 0
                source: root.iconSource
                fillMode: Image.PreserveAspectFit
                smooth: true
            }

            Rectangle {
                anchors.fill: parent
                visible: !iconImage.visible
                radius: QuickDeckTheme.radiusPill
                color: QuickDeckTheme.primarySoft

                Label {
                    anchors.centerIn: parent
                    text: titleLabel.text.length > 0 ? titleLabel.text.charAt(0).toUpperCase() : "?"
                    color: QuickDeckTheme.primary
                    font.bold: true
                    font.pixelSize: 14
                }
            }
        }

        Column {
            spacing: 2
            anchors.verticalCenter: parent.verticalCenter
            width: parent.width - 46 - (pinBadge.visible ? 22 : 0) - (actionHintLabel.visible ? 72 : 0)

            Label {
                id: titleLabel
                width: parent.width
                text: root.title
                color: QuickDeckTheme.textPrimary
                font.pixelSize: 14
                elide: Text.ElideRight
            }

            Label {
                width: parent.width
                visible: root.subtitle.length > 0
                text: root.subtitle
                color: QuickDeckTheme.textSecondary
                font.pixelSize: 11
                elide: Text.ElideMiddle
            }
        }

        Label {
            id: actionHintLabel
            anchors.verticalCenter: parent.verticalCenter
            visible: root.actionHint.length > 0
            text: root.actionHint
            color: QuickDeckTheme.textMuted
            font.pixelSize: 10
            padding: 6
            background: Rectangle {
                radius: QuickDeckTheme.radiusPill
                color: QuickDeckTheme.fieldFill
            }
        }

        Label {
            id: pinBadge
            anchors.verticalCenter: parent.verticalCenter
            visible: root.isPinned
            text: "★"
            color: QuickDeckTheme.primary
            font.pixelSize: 14
            ToolTip.visible: pinBadgeHover.hovered
            ToolTip.text: qsTr("Pinned")

            MouseArea {
                id: pinBadgeHover
                anchors.fill: parent
                hoverEnabled: true
            }
        }
    }

    property string title: ""
    property string subtitle: ""
    property string iconSource: ""
    property string actionHint: ""
    property bool isPinned: false
}
