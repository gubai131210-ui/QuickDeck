#include "app/application_context.h"
#include "core/domain/types.h"
#include "test_helpers.h"
#include "ui/launcher_controller.h"
#include "ui/models/app_search_model.h"

#include <QDateTime>
#include <QDir>
#include <QGuiApplication>
#include <QTemporaryDir>
#include <QtTest>

class LauncherControllerTest : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void show_search_resets_selection_and_visible();
    void dismiss_and_confirm_hide_cycle();
    void move_selection_wraps_with_seeded_apps();
    void toggle_pin_updates_pinned_state();
    void reload_settings_does_not_crash();
};

namespace {

QString isolated_database_path(QTemporaryDir &temp_dir)
{
    return temp_dir.path() + QStringLiteral("/launcher_test.db");
}

quickdeck::AppEntry make_app(const QString &name, const QString &path)
{
    const qint64 now = QDateTime::currentSecsSinceEpoch();
    quickdeck::AppEntry entry;
    entry.name = name;
    entry.executable_path = path;
    entry.platform_id = QStringLiteral("win");
    entry.indexed_at = now;
    entry.created_at = now;
    entry.updated_at = now;
    return entry;
}

} // namespace

void LauncherControllerTest::initTestCase()
{
    static int argc = 1;
    static char arg0[] = "launcher_controller_test";
    static char *argv[] = {arg0, nullptr};
    if (QGuiApplication::instance() == nullptr) {
        new QGuiApplication(argc, argv);
    }
    QGuiApplication::setOrganizationName(QStringLiteral("QuickDeckTests"));
    QGuiApplication::setApplicationName(QStringLiteral("LauncherControllerTest"));
}

void LauncherControllerTest::show_search_resets_selection_and_visible()
{
    QTemporaryDir temp_dir;
    QVERIFY(temp_dir.isValid());

    quickdeck::ApplicationContext context;
    QVERIFY(quickdeck::test::initialize_isolated_context(context, isolated_database_path(temp_dir))
                .is_ok());

    quickdeck::LauncherController controller(context);
    QSignalSpy visible_spy(&controller, &quickdeck::LauncherController::visibleChanged);

    controller.show_search();
    QVERIFY(controller.visible());
    QCOMPARE(controller.selected_index(), 0);
    QVERIFY(visible_spy.count() >= 1);
}

void LauncherControllerTest::dismiss_and_confirm_hide_cycle()
{
    QTemporaryDir temp_dir;
    QVERIFY(temp_dir.isValid());

    quickdeck::ApplicationContext context;
    QVERIFY(quickdeck::test::initialize_isolated_context(context, isolated_database_path(temp_dir))
                .is_ok());

    quickdeck::LauncherController controller(context);
    QSignalSpy hide_requested(&controller, &quickdeck::LauncherController::hideRequested);

    controller.show_search();
    controller.dismiss();
    QCOMPARE(hide_requested.count(), 1);
    QVERIFY(controller.visible());

    controller.confirm_hide();
    QVERIFY(!controller.visible());
}

void LauncherControllerTest::move_selection_wraps_with_seeded_apps()
{
    QTemporaryDir temp_dir;
    QVERIFY(temp_dir.isValid());

    quickdeck::ApplicationContext context;
    QVERIFY(quickdeck::test::initialize_isolated_context(context, isolated_database_path(temp_dir))
                .is_ok());

    QVERIFY(context.database()
                .apps()
                .upsert(make_app(QStringLiteral("Alpha"), QStringLiteral("C:/apps/alpha.exe")))
                .is_ok());
    QVERIFY(context.database()
                .apps()
                .upsert(make_app(QStringLiteral("Beta"), QStringLiteral("C:/apps/beta.exe")))
                .is_ok());

    quickdeck::LauncherController controller(context);
    controller.show_search();
    QCOMPARE(controller.item_count(), 2);

    controller.set_selected_index(0);
    controller.move_selection(-1);
    QCOMPARE(controller.selected_index(), 1);

    controller.move_selection(1);
    QCOMPARE(controller.selected_index(), 0);
}

void LauncherControllerTest::toggle_pin_updates_pinned_state()
{
    QTemporaryDir temp_dir;
    QVERIFY(temp_dir.isValid());

    quickdeck::ApplicationContext context;
    QVERIFY(quickdeck::test::initialize_isolated_context(context, isolated_database_path(temp_dir))
                .is_ok());

    quickdeck::AppEntry entry = make_app(QStringLiteral("PinMe"), QStringLiteral("C:/apps/pin.exe"));
    QVERIFY(context.database().apps().upsert(entry).is_ok());

    quickdeck::LauncherController controller(context);
    controller.show_search();
    QCOMPARE(controller.item_count(), 1);
    QVERIFY(!controller.app_model()->data(controller.app_model()->index(0),
                                          quickdeck::AppSearchModel::IsPinnedRole)
                 .toBool());

    controller.toggle_pin_at(0);
    controller.show_search();
    QVERIFY(controller.app_model()->data(controller.app_model()->index(0),
                                       quickdeck::AppSearchModel::IsPinnedRole)
                .toBool());
}

void LauncherControllerTest::reload_settings_does_not_crash()
{
    QTemporaryDir temp_dir;
    QVERIFY(temp_dir.isValid());

    quickdeck::ApplicationContext context;
    QVERIFY(quickdeck::test::initialize_isolated_context(context, isolated_database_path(temp_dir))
                .is_ok());

    quickdeck::LauncherController controller(context);
    controller.reload_settings();
    QVERIFY(true);
}

QTEST_MAIN(LauncherControllerTest)
#include "launcher_controller_test.moc"
