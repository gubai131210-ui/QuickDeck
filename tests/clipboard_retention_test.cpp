#include "test_helpers.h"

#include <QCoreApplication>
#include <QTemporaryDir>
#include <QtTest>

class ClipboardRetentionTest : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void enforce_retention_trims_oldest_unpinned_entries();
};

void ClipboardRetentionTest::initTestCase()
{
    static int argc = 1;
    static char arg0[] = "clipboard_retention_test";
    static char *argv[] = {arg0, nullptr};
    if (QCoreApplication::instance() == nullptr) {
        new QCoreApplication(argc, argv);
    }
    QCoreApplication::setOrganizationName(QStringLiteral("QuickDeckTests"));
    QCoreApplication::setApplicationName(QStringLiteral("ClipboardRetentionTest"));
}

void ClipboardRetentionTest::enforce_retention_trims_oldest_unpinned_entries()
{
    QTemporaryDir temp_dir;
    QVERIFY(temp_dir.isValid());

    quickdeck::ApplicationContext context;
    QVERIFY(quickdeck::test::initialize_isolated_context(context, temp_dir.path() + QStringLiteral("/retention.db"))
                .is_ok());

    for (int i = 0; i < 5; ++i) {
        QVERIFY(quickdeck::test::seed_clipboard_text(context.database().database(),
                                                     QStringLiteral("entry-%1").arg(i))
                    .is_ok());
    }

    const quickdeck::Result<int> trimmed = context.database().clipboard().enforce_retention(2, 0);
    QVERIFY(trimmed.is_ok());
    QCOMPARE(trimmed.value(), 3);

    const quickdeck::Result<int> count = context.database().clipboard().count();
    QVERIFY(count.is_ok());
    QCOMPARE(count.value(), 2);
}

QTEST_MAIN(ClipboardRetentionTest)
#include "clipboard_retention_test.moc"
