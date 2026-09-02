#include "platform/win/win_hotkey_mapper.h"

#include <QtTest>

class WinHotkeyMapperTest : public QObject {
    Q_OBJECT

private slots:
    void alt_space_maps_correctly();
    void ctrl_shift_v_maps_correctly();
    void reverse_empty_sequence_fails();
    void reverse_modifier_only_fails();
    void reverse_unsupported_key_fails();
};

void WinHotkeyMapperTest::alt_space_maps_correctly()
{
    const quickdeck::Result<quickdeck::WinHotkeyParts> result =
        quickdeck::parse_qkey_sequence(QKeySequence(QStringLiteral("Alt+Space")));
    QVERIFY(result.is_ok());
    QCOMPARE(result.value().modifiers, quint32(0x4001));
    QCOMPARE(result.value().vk, quint32(0x20));
}

void WinHotkeyMapperTest::ctrl_shift_v_maps_correctly()
{
    const quickdeck::Result<quickdeck::WinHotkeyParts> result =
        quickdeck::parse_qkey_sequence(QKeySequence(QStringLiteral("Ctrl+Shift+V")));
    QVERIFY(result.is_ok());
    QCOMPARE(result.value().modifiers, quint32(0x4006));
    QCOMPARE(result.value().vk, quint32('V'));
}

void WinHotkeyMapperTest::reverse_empty_sequence_fails()
{
    const quickdeck::Result<quickdeck::WinHotkeyParts> result =
        quickdeck::parse_qkey_sequence(QKeySequence());
    QVERIFY(result.is_err());
}

void WinHotkeyMapperTest::reverse_modifier_only_fails()
{
    const quickdeck::Result<quickdeck::WinHotkeyParts> result =
        quickdeck::parse_qkey_sequence(QKeySequence(QStringLiteral("Ctrl+Shift")));
    QVERIFY(result.is_err());
}

void WinHotkeyMapperTest::reverse_unsupported_key_fails()
{
    const quickdeck::Result<quickdeck::WinHotkeyParts> result =
        quickdeck::parse_qkey_sequence(QKeySequence(QStringLiteral("Ctrl+Return")));
    QVERIFY(result.is_err());
}

QTEST_MAIN(WinHotkeyMapperTest)
#include "win_hotkey_mapper_test.moc"
