#include "services/app_scanner.h"

#include <QDir>
#include <QFile>
#include <QTemporaryDir>
#include <QtTest>

class AppScannerTest : public QObject {
    Q_OBJECT

private slots:
    void empty_directory_returns_no_entries();
    void ignores_non_lnk_files();
};

void AppScannerTest::empty_directory_returns_no_entries()
{
    QTemporaryDir temp_dir;
    QVERIFY(temp_dir.isValid());

    const quickdeck::Result<QList<quickdeck::AppEntry>> result =
        quickdeck::scan_lnk_roots({temp_dir.path()});
    QVERIFY(result.is_ok());
    QCOMPARE(result.value().size(), 0);
}

void AppScannerTest::ignores_non_lnk_files()
{
    QTemporaryDir temp_dir;
    QVERIFY(temp_dir.isValid());

    QFile file(temp_dir.path() + QStringLiteral("/not-a-shortcut.txt"));
    QVERIFY(file.open(QIODevice::WriteOnly));
    file.write("x");
    file.close();

    const quickdeck::Result<QList<quickdeck::AppEntry>> result =
        quickdeck::scan_lnk_roots({temp_dir.path()});
    QVERIFY(result.is_ok());
    QCOMPARE(result.value().size(), 0);
}

QTEST_MAIN(AppScannerTest)
#include "app_scanner_test.moc"
