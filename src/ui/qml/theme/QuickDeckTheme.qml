pragma Singleton
import QtQuick

QtObject {
    readonly property int radiusWindow: 24
    readonly property int radiusPanel: 18
    readonly property int radiusControl: 14
    readonly property int radiusPill: 999

    readonly property color primary: "#38BDF8"
    readonly property color primarySoft: "#E0F2FE"
    readonly property color accent: "#5EEAD4"
    readonly property color textPrimary: "#0F172A"
    readonly property color textSecondary: "#64748B"
    readonly property color textMuted: "#94A3B8"
    readonly property color glassFill: "#D9FFFFFF"
    readonly property color glassBorder: "#CCFFFFFF"
    readonly property color glassShadow: "#1A38BDF8"
    readonly property color highlight: "#2638BDF8"
    readonly property color surface: "#F8FAFC"
    readonly property color fieldFill: "#F1F5F9"
    readonly property color divider: "#E2E8F0"

    readonly property int spaceXs: 6
    readonly property int spaceSm: 10
    readonly property int spaceMd: 16
    readonly property int spaceLg: 22

    readonly property int animFast: 140
    readonly property int animNormal: 220
}
