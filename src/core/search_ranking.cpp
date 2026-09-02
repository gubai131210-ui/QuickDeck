#include "core/search_ranking.h"

#include <QDateTime>

namespace quickdeck {

namespace {

constexpr int kPrefixMatchScore = 100;
constexpr int kContainsMatchScore = 60;

} // namespace

int match_score(const QString &name, const QString &query_lower)
{
    const QString name_lower = name.toLower();
    if (name_lower.startsWith(query_lower)) {
        return kPrefixMatchScore;
    }
    if (name_lower.contains(query_lower)) {
        return kContainsMatchScore;
    }
    return 0;
}

double usage_score(int launch_count)
{
    return qMin(100.0, static_cast<double>(launch_count) * 10.0);
}

double recency_score(qint64 last_used_at)
{
    if (last_used_at <= 0) {
        return 0.0;
    }
    const qint64 age_seconds = QDateTime::currentSecsSinceEpoch() - last_used_at;
    const double days = static_cast<double>(age_seconds) / 86400.0;
    return qMax(0.0, 100.0 - days * 5.0);
}

double final_search_score(int match, int launch_count, qint64 last_used_at)
{
    return match * 0.7 + usage_score(launch_count) * 0.2 + recency_score(last_used_at) * 0.1;
}

} // namespace quickdeck
