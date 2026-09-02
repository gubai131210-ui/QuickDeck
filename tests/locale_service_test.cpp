#include "services/locale_service.h"

#include <QtTest>

class LocaleServiceTest : public QObject {
    Q_OBJECT

private slots:
    void normalize_language_maps_chinese_variants();
    void normalize_language_defaults_to_english();
    void display_name_returns_localized_label();
};

void LocaleServiceTest::normalize_language_maps_chinese_variants()
{
    QCOMPARE(quickdeck::LocaleService::normalize_language(QStringLiteral("zh")), QStringLiteral("zh_CN"));
    QCOMPARE(quickdeck::LocaleService::normalize_language(QStringLiteral("zh-CN")), QStringLiteral("zh_CN"));
    QCOMPARE(quickdeck::LocaleService::normalize_language(QStringLiteral("zh_CN")), QStringLiteral("zh_CN"));
}

void LocaleServiceTest::normalize_language_defaults_to_english()
{
    QCOMPARE(quickdeck::LocaleService::normalize_language(QStringLiteral("en")), QStringLiteral("en"));
    QCOMPARE(quickdeck::LocaleService::normalize_language(QStringLiteral("fr")), QStringLiteral("en"));
}

void LocaleServiceTest::display_name_returns_localized_label()
{
    QCOMPARE(quickdeck::LocaleService::display_name(QStringLiteral("en")), QStringLiteral("English"));
    QCOMPARE(quickdeck::LocaleService::display_name(QStringLiteral("zh_CN")), QStringLiteral("简体中文"));
}

QTEST_MAIN(LocaleServiceTest)
#include "locale_service_test.moc"
