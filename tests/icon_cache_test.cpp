#include "services/icon_cache.h"

#include <QCoreApplication>
#include <QFileInfo>
#include <QtTest>

class IconCacheTest : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void caches_icon_for_existing_executable();
};

void IconCacheTest::initTestCase()
{
    static int argc = 1;
    static char arg0[] = "icon_cache_test";
    static char *argv[] = {arg0, nullptr};
    if (QCoreApplication::instance() == nullptr) {
        new QCoreApplication(argc, argv);
    }
}

void IconCacheTest::caches_icon_for_existing_executable()
{
#ifdef Q_OS_WIN
    const QString notepad = QStringLiteral("C:/Windows/notepad.exe");
    if (!QFileInfo::exists(notepad)) {
        QSKIP("notepad.exe not available");
    }

    quickdeck::AppEntry entry;
    entry.name = QStringLiteral("Notepad");
    entry.executable_path = notepad;

    const quickdeck::Result<QString> cached = quickdeck::IconCache::cache_icon_for_entry(entry);
    QVERIFY(cached.is_ok());
    QVERIFY(QFileInfo::exists(cached.value()));

    const quickdeck::Result<QString> cached_again = quickdeck::IconCache::cache_icon_for_entry(entry);
    QVERIFY(cached_again.is_ok());
    QCOMPARE(cached_again.value(), cached.value());
#else
    QSKIP("Windows-only icon cache test");
#endif
}

QTEST_MAIN(IconCacheTest)
#include "icon_cache_test.moc"
