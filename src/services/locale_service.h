#pragma once

#include "core/interfaces/repositories.h"
#include "core/result.h"

#include <QObject>
#include <QStringList>
#include <QTranslator>
#include <memory>

class QQmlApplicationEngine;

namespace quickdeck {

class LocaleService : public QObject {
    Q_OBJECT

public:
    explicit LocaleService(QObject *parent = nullptr);

    [[nodiscard]] static QStringList supported_languages();
    [[nodiscard]] static QString display_name(const QString &language_code);
    [[nodiscard]] static QString normalize_language(const QString &language_code);
    [[nodiscard]] static QString detect_system_language();

    [[nodiscard]] Result<void> apply_saved_language(ISettingsStore &settings);
    [[nodiscard]] Result<void> set_language(ISettingsStore &settings,
                                            const QString &language_code,
                                            QQmlApplicationEngine *qml_engine = nullptr);

    [[nodiscard]] QString current_language() const { return current_language_; }

signals:
    void language_changed();

private:
    [[nodiscard]] Result<void> install_translators(const QString &language_code);
    void remove_translators();

    QString current_language_;
    std::unique_ptr<QTranslator> app_translator_;
    std::unique_ptr<QTranslator> qt_translator_;
};

} // namespace quickdeck
