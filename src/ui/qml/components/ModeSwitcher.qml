import QtQuick
import QtQuick.Controls
import QuickDeckLauncher

Row {
    id: root
    spacing: QuickDeckTheme.spaceSm

    property int modeValue: 0

    signal modeSelected(int modeValue)

    Repeater {
        model: [
            { label: qsTr("Search"), value: 0, icon: "⌕" },
            { label: qsTr("Clipboard"), value: 1, icon: "⎘" }
        ]

        delegate: ItemDelegate {
            required property var modelData
            width: 120
            height: 36
            padding: 8

            background: Rectangle {
                radius: QuickDeckTheme.radiusPill
                color: root.modeValue === modelData.value ? QuickDeckTheme.primarySoft
                                                          : QuickDeckTheme.fieldFill
                Behavior on color { ColorAnimation { duration: QuickDeckTheme.animFast } }
            }

            contentItem: Row {
                spacing: 8
                anchors.centerIn: parent

                Label {
                    text: modelData.icon
                    color: root.modeValue === modelData.value ? QuickDeckTheme.primary
                                                              : QuickDeckTheme.textSecondary
                    font.pixelSize: 14
                }

                Label {
                    text: modelData.label
                    color: root.modeValue === modelData.value ? QuickDeckTheme.primary
                                                              : QuickDeckTheme.textSecondary
                    font.pixelSize: 13
                    font.weight: root.modeValue === modelData.value ? Font.DemiBold : Font.Normal
                }
            }

            onClicked: root.modeSelected(modelData.value)
        }
    }
}
