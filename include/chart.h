#ifndef CHART_H
#define CHART_H

#include <QWidget>
#include <QMap>
#include <QPair>
#include <QHash>
#include <QString>
#include <QSqlQuery>
#include <QDate>
#include <QDateTime>
#include <QtCharts>
#include <QVariantMap>
#include <QVector>

#include <algorithm>

namespace core {
class StateMgr;
}

class Chart : public QWidget
{
    Q_OBJECT
    core::StateMgr* ptrStateMgr_;
    QChart* chart_;
    QHash<int, QString> months_;
    QHash<int, QString> category_group_;
    QHash<int, QPair<int, QString>> category_;
    QString chart_type_;
    QString currency_;
    QWidget* container_;
    QLayout* container_layout_;
    QChartView *chartView_;
public:
    explicit Chart(QWidget *parent = nullptr, QString type = "Bar", core::StateMgr* ptrStateMgr = nullptr);
    QWidget* generateContainer();
    ~Chart();

private:
    void generateBarChart_(); // Generiert ein Balkendiagramm
    void generatePieChart_(); // Generiert ein Kreisdiagramm
    // Bereitet Daten für das Balkendiagramm vor
    QMap<int, QPair<double, double>> getDataForBarChart_();
    // Bereitet Daten für das Kreisdiagramm vor
    QPair<QMap<QString, QMap<int, double>>, QMap<QString, QMap<int, double>>> getDataForPieChart_();
};

#endif // CHART_H
