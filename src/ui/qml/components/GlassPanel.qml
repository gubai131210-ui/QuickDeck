import QtQuick
import QtQuick.Effects
import QuickDeckLauncher

Item {
    id: root
    property alias radius: panel.radius
    default property alias content: contentSlot.data

    implicitWidth: contentSlot.implicitWidth
    implicitHeight: contentSlot.implicitHeight

    Rectangle {
        id: shadow
        anchors.fill: panel
        anchors.topMargin: 8
        radius: panel.radius
        color: QuickDeckTheme.glassShadow
        opacity: 0.35
    }

    Rectangle {
        id: panel
        anchors.fill: parent
        radius: QuickDeckTheme.radiusWindow
        color: QuickDeckTheme.glassFill
        border.color: QuickDeckTheme.glassBorder
        border.width: 1
        layer.enabled: true
        layer.effect: MultiEffect {
            shadowEnabled: true
            shadowColor: QuickDeckTheme.glassShadow
            shadowBlur: 0.55
            shadowVerticalOffset: 10
            shadowHorizontalOffset: 0
        }

        Item {
            id: contentSlot
            anchors.fill: parent
        }
    }
}
