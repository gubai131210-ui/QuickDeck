#include "services/locale_service.h"

#include "services/logger.h"

#include <QApplication>
#include <QCoreApplication>
#include <QLibraryInfo>
#include <QLocale>
#include <QQmlApplicationEngine>
#include <QTranslator>

namespace quickdeck {

LocaleService::LocaleService(QObject *parent)
    : QObject(parent)
{
}

QStringList LocaleService::supported_languages()
{
    return {QStringLiteral("en"), QStringLiteral("zh_CN")};
}

QString LocaleService::display_name(const QString &language_code)
{
    const QString normalized = normalize_language(language_code);
    if (normalized == QStringLiteral("zh_CN")) {
        return QStringLiteral("简体中文");
    }
    return QStringLiteral("English");
}

QString LocaleService::normalize_language(const QString &language_code)
{
    const QString trimmed = language_code.trimmed();
    if (trimmed == QStringLiteral("zh") || trimmed == QStringLiteral("zh-CN") ||
        trimmed == QStringLiteral("zh_CN") || trimmed.startsWith(QStringLiteral("zh"))) {
        return QStringLiteral("zh_CN");
    }
    return QStringLiteral("en");
}

QString LocaleService::detect_system_language()
{
    const QLocale locale = QLocale::system();
    if (locale.language() == QLocale::Chinese) {
        return QStringLiteral("zh_CN");
    }
    return QStringLiteral("en");
}

Result<void> LocaleService::apply_saved_language(ISettingsStore &settings)
{
    const Result<QString> stored =
        settings.get_string(QStringLiteral("general.language"), QString());
    const QString language = stored.is_ok() && !stored.value().isEmpty()
                                 ? normalize_language(stored.value())
                                 : detect_system_language();
    return set_language(settings, language, nullptr);
}

Result<void> LocaleService::set_language(ISettingsStore &settings,
                                         const QString &language_code,
                                         QQmlApplicationEngine *qml_engine)
{
    const QString normalized = normalize_language(language_code);
    if (normalized == current_language_ && app_translator_ != nullptr) {
        return Result<void>::ok();
    }

    remove_translators();
    const Result<void> install_result = install_translators(normalized);
    if (install_result.is_err()) {
        return install_result;
    }

    current_language_ = normalized;
    settings.set_string(QStringLiteral("general.language"), normalized);

    if (qml_engine != nullptr) {
        qml_engine->retranslate();
    }

    if (QCoreApplication::instance() != nullptr) {
        QCoreApplication::postEvent(QCoreApplication::instance(),
                                    new QEvent(QEvent::LanguageChange));
    }

    emit language_changed();
    QD_LOG_INFO(QStringLiteral("Language set to %1").arg(normalized));
    return Result<void>::ok();
}

Result<void> LocaleService::install_translators(const QString &language_code)
{
    app_translator_ = std::make_unique<QTranslator>();
    qt_translator_ = std::make_unique<QTranslator>();

    const QString base_name = QStringLiteral("quickdeck_%1").arg(language_code);
    const QString app_dir = QCoreApplication::applicationDirPath();
    const QString resource_path =
        QStringLiteral(":/i18n/%1.qm").arg(base_name);

    bool app_loaded = app_translator_->load(resource_path);
    if (!app_loaded) {
        app_loaded = app_translator_->load(base_name, app_dir + QStringLiteral("/i18n"));
    }
    if (!app_loaded) {
        QD_LOG_WARN(QStringLiteral("Translation not found for %1, falling back to source")
                        .arg(language_code));
    } else {
        QCoreApplication::installTranslator(app_translator_.get());
    }

    if (qt_translator_->load(QStringLiteral("qt_%1").arg(language_code),
                             QLibraryInfo::path(QLibraryInfo::TranslationsPath))) {
        QCoreApplication::installTranslator(qt_translator_.get());
    } else if (language_code == QStringLiteral("zh_CN") &&
               qt_translator_->load(QStringLiteral("qt_zh_CN"),
                                    QLibraryInfo::path(QLibraryInfo::TranslationsPath))) {
        QCoreApplication::installTranslator(qt_translator_.get());
    } else if (language_code == QStringLiteral("zh_CN") &&
               qt_translator_->load(QStringLiteral(":/qt/i18n/qt_zh_CN.qm"))) {
        QCoreApplication::installTranslator(qt_translator_.get());
    }

    return Result<void>::ok();
}

void LocaleService::remove_translators()
{
    if (app_translator_ != nullptr) {
        QCoreApplication::removeTranslator(app_translator_.get());
        app_translator_.reset();
    }
    if (qt_translator_ != nullptr) {
        QCoreApplication::removeTranslator(qt_translator_.get());
        qt_translator_.reset();
    }
}

} // namespace quickdeck
