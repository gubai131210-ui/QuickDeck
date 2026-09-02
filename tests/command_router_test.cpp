#include "core/command_router.h"

#include <QCoreApplication>
#include <QtTest>

class CommandRouterTest : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void detects_command_query_prefix();
    void lists_all_commands_for_bare_prefix();
    void filters_settings_command();
    void includes_lang_variants();
};

void CommandRouterTest::initTestCase()
{
    static int argc = 1;
    static char arg0[] = "command_router_test";
    static char *argv[] = {arg0, nullptr};
    if (QCoreApplication::instance() == nullptr) {
        new QCoreApplication(argc, argv);
    }
}

void CommandRouterTest::detects_command_query_prefix()
{
    QVERIFY(quickdeck::CommandRouter::is_command_query(QStringLiteral(">settings")));
    QVERIFY(!quickdeck::CommandRouter::is_command_query(QStringLiteral("settings")));
}

void CommandRouterTest::lists_all_commands_for_bare_prefix()
{
    const QVector<quickdeck::CommandItem> matches =
        quickdeck::CommandRouter::match_commands(QStringLiteral(">"));
    QVERIFY(matches.size() >= 6);
}

void CommandRouterTest::filters_settings_command()
{
    const QVector<quickdeck::CommandItem> matches =
        quickdeck::CommandRouter::match_commands(QStringLiteral(">set"));
    bool found_settings = false;
    for (const quickdeck::CommandItem &item : matches) {
        if (item.id == QStringLiteral("settings")) {
            found_settings = true;
            break;
        }
    }
    QVERIFY(found_settings);
}

void CommandRouterTest::includes_lang_variants()
{
    const QVector<quickdeck::CommandItem> matches =
        quickdeck::CommandRouter::match_commands(QStringLiteral(">lang"));
    QVERIFY(matches.size() >= 3);
}

QTEST_MAIN(CommandRouterTest)
#include "command_router_test.moc"
