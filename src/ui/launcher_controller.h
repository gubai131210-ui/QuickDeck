#pragma once

#include "app/application_context.h"
#include "core/domain/types.h"
#include "ui/models/app_search_model.h"
#include "ui/models/clipboard_history_model.h"

#include <QObject>

namespace quickdeck {

class LauncherController : public QObject {
    Q_OBJECT
    Q_PROPERTY(int modeValue READ mode_value NOTIFY modeChanged)
    Q_PROPERTY(bool visible READ visible NOTIFY visibleChanged)
    Q_PROPERTY(QString query READ query WRITE setQuery NOTIFY queryChanged)
    Q_PROPERTY(AppSearchModel *appModel READ app_model CONSTANT)
    Q_PROPERTY(ClipboardHistoryModel *clipboardModel READ clipboard_model CONSTANT)

public:
    explicit LauncherController(ApplicationContext &context, QObject *parent = nullptr);

    [[nodiscard]] int mode_value() const { return static_cast<int>(mode_); }
    [[nodiscard]] bool visible() const { return visible_; }
    [[nodiscard]] QString query() const { return query_; }
    [[nodiscard]] AppSearchModel *app_model() { return &app_model_; }
    [[nodiscard]] ClipboardHistoryModel *clipboard_model() { return &clipboard_model_; }

    Q_INVOKABLE void show_search();
    Q_INVOKABLE void show_clipboard();
    Q_INVOKABLE void hide();
    Q_INVOKABLE void activate_selected(int index);
    Q_INVOKABLE void register_hotkeys();

    void setQuery(const QString &query);

signals:
    void modeChanged();
    void visibleChanged();
    void queryChanged();

private:
    void refresh_results();
    void setup_hotkeys();

    ApplicationContext &context_;
    AppSearchModel app_model_;
    ClipboardHistoryModel clipboard_model_;
    QList<AppEntry> app_results_;
    QList<ClipboardEntry> clipboard_results_;
    LauncherMode mode_ = LauncherMode::Search;
    bool visible_ = false;
    QString query_;
};

} // namespace quickdeck
