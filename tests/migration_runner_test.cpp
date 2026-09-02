#include "data/migration_runner.h"
#include "test_helpers.h"

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QTemporaryDir>
#include <QtTest>

class MigrationRunnerTest : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void applies_initial_migration_idempotently();
    void reverse_missing_directory_fails();
    void reverse_invalid_sql_fails_without_partial_version();
};

void MigrationRunnerTest::initTestCase()
{
    static int argc = 1;
    static char arg0[] = "migration_runner_test";
    static char *argv[] = {arg0, nullptr};
    if (QCoreApplication::instance() == nullptr) {
        new QCoreApplication(argc, argv);
    }
}

void MigrationRunnerTest::applies_initial_migration_idempotently()
{
    QTemporaryDir temp_dir;
    QVERIFY(temp_dir.isValid());

    const QString db_path = temp_dir.path() + QStringLiteral("/migration_test.db");
    QSqlDatabase db = QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"), QStringLiteral("migration_test"));
    db.setDatabaseName(db_path);
    QVERIFY2(db.open(), qPrintable(db.lastError().text()));

    const QString migrations = quickdeck::test::migrations_directory();
    const quickdeck::Result<void> first = quickdeck::MigrationRunner::apply_all(db, migrations);
    QVERIFY2(first.is_ok(), qPrintable(first.error()));

    const quickdeck::Result<void> second = quickdeck::MigrationRunner::apply_all(db, migrations);
    QVERIFY2(second.is_ok(), qPrintable(second.error()));

    QSqlQuery query(db);
    QVERIFY(query.exec(QStringLiteral("SELECT COUNT(*) FROM settings")));
    QVERIFY(query.next());
    QVERIFY(query.value(0).toInt() >= 1);

    QVERIFY(query.exec(QStringLiteral("SELECT COUNT(*) FROM sqlite_master WHERE type='trigger'")));
    QVERIFY(query.next());
    QCOMPARE(query.value(0).toInt(), 3);

    db.close();
    QSqlDatabase::removeDatabase(QStringLiteral("migration_test"));
}

void MigrationRunnerTest::reverse_missing_directory_fails()
{
    QSqlDatabase db = QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"),
                                                QStringLiteral("migration_missing"));
    db.setDatabaseName(QStringLiteral(":memory:"));
    QVERIFY(db.open());

    const quickdeck::Result<void> result =
        quickdeck::MigrationRunner::apply_all(db, QStringLiteral("/path/does/not/exist"));
    QVERIFY(result.is_err());

    db.close();
    QSqlDatabase::removeDatabase(QStringLiteral("migration_missing"));
}

void MigrationRunnerTest::reverse_invalid_sql_fails_without_partial_version()
{
    QTemporaryDir temp_dir;
    QVERIFY(temp_dir.isValid());

    QDir(temp_dir.path()).mkdir(QStringLiteral("migrations"));
    QFile bad_file(temp_dir.path() + QStringLiteral("/migrations/002_bad.sql"));
    QVERIFY(bad_file.open(QIODevice::WriteOnly | QIODevice::Text));
    bad_file.write("CREATE TABLE bad_table (id INTEGER);\nTHIS IS NOT SQL;\n");
    bad_file.close();

    const QString db_path = temp_dir.path() + QStringLiteral("/bad_migration.db");
    QSqlDatabase db =
        QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"), QStringLiteral("migration_bad"));
    db.setDatabaseName(db_path);
    QVERIFY(db.open());

    const quickdeck::Result<void> first =
        quickdeck::MigrationRunner::apply_all(db, quickdeck::test::migrations_directory());
    QVERIFY2(first.is_ok(), qPrintable(first.error()));

    const quickdeck::Result<void> bad =
        quickdeck::MigrationRunner::apply_all(db, temp_dir.path() + QStringLiteral("/migrations"));
    QVERIFY(bad.is_err());

    QSqlQuery query(db);
    QVERIFY(query.exec(QStringLiteral("SELECT MAX(version) FROM schema_version")));
    QVERIFY(query.next());
    QCOMPARE(query.value(0).toInt(), 1);

    db.close();
    QSqlDatabase::removeDatabase(QStringLiteral("migration_bad"));
}

QTEST_MAIN(MigrationRunnerTest)
#include "migration_runner_test.moc"
