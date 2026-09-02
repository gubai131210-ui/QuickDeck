#include "data/database_manager.h"
#include "test_helpers.h"

#include <QCoreApplication>
#include <QDir>
#include <QTemporaryDir>
#include <QtTest>

class ClipboardDeduplicationTest : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void duplicate_content_is_detected();
    void empty_content_is_not_duplicate();
};

void ClipboardDeduplicationTest::initTestCase()
{
    static int argc = 1;
    static char arg0[] = "clipboard_deduplication_test";
    static char *argv[] = {arg0, nullptr};
    if (QCoreApplication::instance() == nullptr) {
        new QCoreApplication(argc, argv);
    }
    QCoreApplication::setOrganizationName(QStringLiteral("QuickDeckTests"));
    QCoreApplication::setApplicationName(QStringLiteral("ClipboardDeduplicationTest"));
}

void ClipboardDeduplicationTest::duplicate_content_is_detected()
{
    QTemporaryDir temp_dir;
    QVERIFY(temp_dir.isValid());

    quickdeck::DatabaseManager database;
    const QString db_path = temp_dir.path() + QStringLiteral("/test.db");

    const quickdeck::Result<void> open_result =
        database.open(db_path, quickdeck::test::migrations_directory());
    QVERIFY2(open_result.is_ok(), qPrintable(open_result.error()));

    const quickdeck::Result<void> seed_result =
        quickdeck::test::seed_clipboard_text(database.database(), QStringLiteral("duplicate text"));
    QVERIFY2(seed_result.is_ok(), qPrintable(seed_result.error()));

    const quickdeck::Result<bool> duplicate =
        database.clipboard().is_duplicate(QStringLiteral("duplicate text"));
    QVERIFY(duplicate.is_ok());
    QVERIFY(duplicate.value());
}

void ClipboardDeduplicationTest::empty_content_is_not_duplicate()
{
    QTemporaryDir temp_dir;
    QVERIFY(temp_dir.isValid());

    quickdeck::DatabaseManager database;
    const QString db_path = temp_dir.path() + QStringLiteral("/empty.db");

    const quickdeck::Result<void> open_result =
        database.open(db_path, quickdeck::test::migrations_directory());
    QVERIFY2(open_result.is_ok(), qPrintable(open_result.error()));

    const quickdeck::Result<bool> duplicate =
        database.clipboard().is_duplicate(QString());
    QVERIFY(duplicate.is_ok());
    QVERIFY(!duplicate.value());
}

QTEST_MAIN(ClipboardDeduplicationTest)
#include "clipboard_deduplication_test.moc"
