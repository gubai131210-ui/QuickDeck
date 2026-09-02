#include "services/search_service.h"

#include <QCoreApplication>
#include <QtTest>

class PathResolverTest : public QObject {
    Q_OBJECT

private slots:
    void detects_windows_drive_paths();
    void resolves_home_relative_paths();
};

void PathResolverTest::detects_windows_drive_paths()
{
    QVERIFY(quickdeck::PathResolver::looks_like_path(QStringLiteral("C:/Users/test/file.txt")));
    QVERIFY(!quickdeck::PathResolver::looks_like_path(QStringLiteral("notepad")));
}

void PathResolverTest::resolves_home_relative_paths()
{
    static int argc = 1;
    static char arg0[] = "test";
    static char *argv[] = {arg0, nullptr};
    QCoreApplication app(argc, argv);

    const quickdeck::Result<QString> resolved =
        quickdeck::PathResolver::resolve(QStringLiteral("~/"));
    QVERIFY(resolved.is_ok());
    QVERIFY(resolved.value().contains(QStringLiteral("Users")) ||
           resolved.value().contains(QStringLiteral("home")));
}

QTEST_MAIN(PathResolverTest)
#include "path_resolver_test.moc"
