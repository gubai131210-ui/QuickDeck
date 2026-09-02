#include "core/search_ranking.h"

#include <QDateTime>
#include <QtTest>

class SearchRankingTest : public QObject {
    Q_OBJECT

private slots:
    void prefix_match_scores_higher_than_contains();
    void final_score_weights_match_spec();
};

void SearchRankingTest::prefix_match_scores_higher_than_contains()
{
    QCOMPARE(quickdeck::match_score(QStringLiteral("Visual Studio Code"), QStringLiteral("vis")), 100);
    QCOMPARE(quickdeck::match_score(QStringLiteral("My Visual Studio"), QStringLiteral("vis")), 60);
    QCOMPARE(quickdeck::match_score(QStringLiteral("Notepad"), QStringLiteral("vis")), 0);
}

void SearchRankingTest::final_score_weights_match_spec()
{
    const double prefix_heavy = quickdeck::final_search_score(100, 10, QDateTime::currentSecsSinceEpoch());
    const double contains_only = quickdeck::final_search_score(60, 0, 0);
    QVERIFY(prefix_heavy > contains_only);
}

QTEST_MAIN(SearchRankingTest)
#include "search_ranking_test.moc"
