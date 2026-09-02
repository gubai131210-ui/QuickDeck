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

        Rectangle {
            width: 34
            height: 34
            radius: QuickDeckTheme.radiusPill
            color: QuickDeckTheme.primarySoft
            anchors.verticalCenter: parent.verticalCenter

            Label {
                anchors.centerIn: parent
                text: titleLabel.text.length > 0 ? titleLabel.text.charAt(0).toUpperCase() : "?"
                color: QuickDeckTheme.primary
                font.bold: true
                font.pixelSize: 14
            }
        }

        Column {
            spacing: 2
            anchors.verticalCenter: parent.verticalCenter
            width: parent.width - 46

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
    }

    property string title: ""
    property string subtitle: ""
}
