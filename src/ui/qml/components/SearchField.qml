import QtQuick
import QtQuick.Controls
import QuickDeckLauncher

TextField {
    id: root
    leftPadding: 16
    rightPadding: 16
    topPadding: 12
    bottomPadding: 12
    color: QuickDeckTheme.textPrimary
    placeholderTextColor: QuickDeckTheme.textMuted
    selectionColor: QuickDeckTheme.primarySoft
    selectedTextColor: QuickDeckTheme.textPrimary
    font.pixelSize: 15

    background: Rectangle {
        radius: QuickDeckTheme.radiusControl
        color: hovered ? "#FFFFFFFF" : QuickDeckTheme.fieldFill
        border.color: root.activeFocus ? QuickDeckTheme.primary : QuickDeckTheme.divider
        border.width: root.activeFocus ? 2 : 1
        Behavior on color { ColorAnimation { duration: QuickDeckTheme.animFast } }
        Behavior on border.color { ColorAnimation { duration: QuickDeckTheme.animFast } }
    }
}
