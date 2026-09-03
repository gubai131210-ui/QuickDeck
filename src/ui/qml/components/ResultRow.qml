import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QuickDeckLauncher

ItemDelegate {
    id: root
    height: 56
    padding: 12

    background: Rectangle {
        radius: QuickDeckTheme.radiusControl
        color: root.highlighted ? QuickDeckTheme.highlight
                                  : (root.hovered ? QuickDeckTheme.highlight : "transparent")
        Behavior on color { ColorAnimation { duration: QuickDeckTheme.animFast } }
    }

    contentItem: RowLayout {
        spacing: QuickDeckTheme.spaceSm

        Item {
            Layout.preferredWidth: 34
            Layout.preferredHeight: 34

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

        ColumnLayout {
            Layout.fillWidth: true
            spacing: 2

            Label {
                id: titleLabel
                Layout.fillWidth: true
                text: root.title
                color: QuickDeckTheme.textPrimary
                font.pixelSize: 14
                elide: Text.ElideRight
            }

            Label {
                Layout.fillWidth: true
                visible: root.subtitle.length > 0
                text: root.subtitle
                color: QuickDeckTheme.textSecondary
                font.pixelSize: 11
                elide: Text.ElideMiddle
            }
        }

        Label {
            id: actionHintLabel
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
            visible: root.isPinned
            text: "★"
            color: QuickDeckTheme.primary
            font.pixelSize: 14

            HoverHandler {
                id: pinHover
            }

            ToolTip.visible: pinHover.hovered
            ToolTip.text: qsTr("Pinned")
        }
    }

    property string title: ""
    property string subtitle: ""
    property string iconSource: ""
    property string actionHint: ""
    property bool isPinned: false
}
