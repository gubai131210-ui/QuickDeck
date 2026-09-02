#include "data/database_manager.h"

#ifndef QUICKDECK_SOURCE_DIR
#define QUICKDECK_SOURCE_DIR "."
#endif

#include <QCoreApplication>
#include <QTemporaryDir>
#include <QtTest>

class ClipboardDeduplicationTest : public QObject {
    Q_OBJECT

private slots:
    void duplicate_content_is_detected();
};

void ClipboardDeduplicationTest::duplicate_content_is_detected()
{
    static int argc = 1;
    static char arg0[] = "test";
    static char *argv[] = {arg0, nullptr};
    QCoreApplication app(argc, argv);
    QCoreApplication::setOrganizationName(QStringLiteral("QuickDeckTest"));
    QCoreApplication::setApplicationName(QStringLiteral("QuickDeckTest"));

    QTemporaryDir temp_dir;
    QVERIFY(temp_dir.isValid());

    quickdeck::DatabaseManager database;
    const QString db_path = temp_dir.path() + QStringLiteral("/test.db");
    const QString migrations = QStringLiteral(QUICKDECK_SOURCE_DIR "/migrations");

    const quickdeck::Result<void> open_result =
        database.open(db_path, migrations);
    QVERIFY2(open_result.is_ok(), qPrintable(open_result.error()));

    quickdeck::ClipboardEntry entry;
    entry.content = QStringLiteral("duplicate text");
    entry.preview = entry.content;

    const quickdeck::Result<qint64> first = database.clipboard().insert(entry);
    QVERIFY(first.is_ok());

    const quickdeck::Result<bool> duplicate = database.clipboard().is_duplicate(entry.content);
    QVERIFY(duplicate.is_ok());
    QVERIFY(duplicate.value());
}

QTEST_MAIN(ClipboardDeduplicationTest)
#include "clipboard_deduplication_test.moc"
