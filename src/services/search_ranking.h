#pragma once

#include <QString>

namespace quickdeck {

[[nodiscard]] int match_score(const QString &name, const QString &query_lower);
[[nodiscard]] double usage_score(int launch_count);
[[nodiscard]] double recency_score(qint64 last_used_at);
[[nodiscard]] double final_search_score(int match, int launch_count, qint64 last_used_at);

} // namespace quickdeck
