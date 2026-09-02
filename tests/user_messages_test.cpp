#include "core/user_messages.h"

#include <QCoreApplication>
#include <QtTest>

class UserMessagesTest : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void recognizes_error_codes();
    void translates_launch_errors();
    void translates_index_errors();
};

void UserMessagesTest::initTestCase()
{
    static int argc = 1;
    static char arg0[] = "user_messages_test";
    static char *argv[] = {arg0, nullptr};
    if (QCoreApplication::instance() == nullptr) {
        new QCoreApplication(argc, argv);
    }
}

void UserMessagesTest::recognizes_error_codes()
{
    QVERIFY(quickdeck::UserMessages::is_error_code(
        QString::fromLatin1(quickdeck::ErrorCodes::kLaunchFileMissing)));
    QVERIFY(!quickdeck::UserMessages::is_error_code(QStringLiteral("Failed to launch: x")));
}

void UserMessagesTest::translates_launch_errors()
{
    const QString translated = quickdeck::UserMessages::translate_error(
        QString::fromLatin1(quickdeck::ErrorCodes::kLaunchFileMissing));
    QVERIFY(!translated.isEmpty());
    QVERIFY(!translated.contains(QStringLiteral("launch.file_missing")));
}

void UserMessagesTest::translates_index_errors()
{
    const QString translated = quickdeck::UserMessages::translate_error(
        QString::fromLatin1(quickdeck::ErrorCodes::kIndexScanFailed));
    QVERIFY(!translated.isEmpty());
    QVERIFY(!translated.contains(QStringLiteral("index.scan_failed")));
}

QTEST_MAIN(UserMessagesTest)
#include "user_messages_test.moc"
