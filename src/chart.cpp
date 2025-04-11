#include "../include/chart.h"
#include "../include/statemgr.h"

Chart::Chart(QWidget *parent, QString type, core::StateMgr* ptrStateMgr)
    : QWidget{parent}
    , ptrStateMgr_(ptrStateMgr)
{

    // Prüfung ob StateMgr gesetzt wurde
    if (!ptrStateMgr_) return;

    // QHash für Monate initialisieren
    QList<QString> months = {"JAN", "FEB", "MÄR",
                             "APR", "MAI", "JUN",
                             "JUL", "AUG", "SEP",
                             "OKT", "NOV", "DEZ"};
    for (int i = 0; i < months.count(); i++){
        months_[i+1] = months[i];
    }

    // QHash für Kategorien und Unterkategorien initialisieren
    QSqlQuery q;
    q = ptrStateMgr->getCategories();
    while (q.next()){
        category_group_[q.value("id").toInt()] = q.value("name").toString();
    }
    q = ptrStateMgr->getSubCategories();
    QPair<int, QString> inner;
    while (q.next()){
        // int (Id), Pair (1. Eintrag = Id der Parent Category, 2. Eintrag = Name)
        inner.first = q.value("parent_group_id").toInt();
        inner.second = q.value("name").toString();
        category_[q.value("id").toInt()] = inner;
    }

    // Typ des Charts
    // MoM -> Month over Month (Monatsvergleich)
    // YoY -> Year over Year (Jahresvergleich)
    // Jahresvergleich wird nur dann aufgerufen, wenn Nutzer *alle Datensätze* anzeigen lässt
    chart_type_ = "MoM";

    // Currency setzen
    currency_ = this->ptrStateMgr_->getCurrency();

    // Prüfung auf Typ des Diagramms
    // Bar -> Balkendiagramm, Pie -> Kreisdiagramm
    if (type == "bar") {
        this->generateBarChart_();
    }
    else if (type == "pie") {
        this->generatePieChart_();
    }
    else { // Warnhinweis für Entwicklung falls falscher Parameter gesetzt wurde
        qWarning() << "Wrong type specified at Chart::Chart Constructor. Possible types: 'bar' or 'pie'";
        QMessageBox::critical(nullptr, "Hinweis", "Diagramme konnten nicht geladen werden");
        return;
    }
}

QWidget *Chart::generateContainer()
{
    return this->container_;
}

Chart::~Chart()
{
    // Chart sauber löschen da kein Parent gesetzt wurde
    // Nicht notwendig, qt räumt auf
    // delete chart_;
}

void Chart::generateBarChart_()
{
    QMap<int, QPair<double, double>> data = getDataForBarChart_();
    QList<qreal> income;
    QList<qreal> expense;
    QList<qreal> profit_loss;
    const QList<int>& keys = data.keys();

    for (int i: keys){
        income.append(data[i].first);
        expense.append(data[i].second);
        profit_loss.append(data[i].first - data[i].second);
    }

    // BarSeries (Balkendiagramm) erstellen
    QBarSet* setIncome = new QBarSet("Einnahmen", this);
    QBarSet* setExpense = new QBarSet("Ausgaben", this);
    QBarSet* setProfit_loss = new QBarSet("Über- / Unterdeckung", this);
    // Werte der Listen hinzufügen
    setIncome->append(income);
    setExpense->append(expense);
    setProfit_loss->append(profit_loss);

    // Signal-Slot-Verbindung für Hover-Effekt
    connect(setIncome, &QBarSet::hovered, [this, setIncome](bool status, int index) {
        if (status) {
            QToolTip::showText(QCursor::pos(), "Einnahmen: " + QString::number(setIncome->at(index)) + " " + this->currency_);
        }
    });
    connect(setExpense, &QBarSet::hovered, [this, setExpense](bool status, int index) {
        if (status) {
            QToolTip::showText(QCursor::pos(), "Ausgaben: " + QString::number(setExpense->at(index)) + " " + this->currency_);
        }
    });
    connect(setProfit_loss, &QBarSet::hovered, [this, setProfit_loss](bool status, int index) {
        if (status) {
            QString t;
            (setProfit_loss->at(index) < 0) ? t = "Fehlbetrag: " : t = "Überschuss: ";
            QToolTip::showText(QCursor::pos(), t + QString::number(setProfit_loss->at(index)) + " " + this->currency_);
        }
    });

    QBarSeries* series = new QBarSeries(this);
    series->append(setIncome);
    series->append(setExpense);
    series->append(setProfit_loss);



    // Erstelle das Diagramm
    chart_ = new QChart();
    chart_->addSeries(series);

    if (chart_type_ == "MoM"){
        chart_->setTitle("Monatsvergleich Einnahmen vs. Ausgaben");
    }
    else {
        chart_->setTitle("Jahresvergleich Einnahmen vs. Ausgaben");
    }

    chart_->setContentsMargins(0, 0, 0, 0);
    chart_->setAnimationOptions(QChart::AllAnimations);
    chart_->setTheme(QChart::ChartThemeBlueIcy);
    chart_->setBackgroundBrush(Qt::NoBrush);

    // Kategorien für die X-Achse
    QStringList categories;

    for (int i: keys){
        if (chart_type_ == "MoM") categories.append(this->months_[i]);
        if (chart_type_ == "YoY") categories.append(QString::number(i));
    }

    QBarCategoryAxis *axisX = new QBarCategoryAxis(this);

    axisX->append(categories);
    chart_->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);
    // Maximum für die Y Achse festlegen
    double max = 0;
    double min = 0;
    for (const auto& pair: data) {
        max = std::max({max, pair.first, pair.second});
    }
    for (int i = 0; i < profit_loss.count(); i++){
        if (profit_loss[i] < min){
            min = profit_loss[i];
        }
    }

    // Wertebereich für die Y-Achse
    QValueAxis *axisY = new QValueAxis(this);
    axisY->setRange(min, max);
    axisY->setTitleText(currency_);
    chart_->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    chartView_ = new QChartView(chart_, this);
    chartView_->setStyleSheet("background: transparent");
    chartView_->setRenderHint(QPainter::Antialiasing);

    container_layout_ = new QHBoxLayout();
    container_layout_->addWidget(chartView_);
    container_ = new QWidget(this);
    container_->setLayout(container_layout_);
}

void Chart::generatePieChart_()
{
    // Hier aufgehört am 03.04
    // ToDo Pie Chart implementieren
    using Category = QMap<QString, QMap<int, double>>; // first
    using SubCategory = QMap<QString, QMap<int, double>>; // second
    QPair<Category, SubCategory> data;
    // Daten für Kreisdiagramm generieren
    // data.first --> QString 'income' oder 'expense', QMap int = Kategorie-ID, double = Total (Betrag)
    // data.second --> QString 'income' oder 'expense', QMap int = Unterkategorie-ID, double = Total (Betrag)
    data = getDataForPieChart_();
    QMap<QString, QMap<int, QMap<QString, double>>> sub_category_data;
    // Sub Category Data aufbereiten
    for (QString key: data.second.keys()){
        for (int i: data.second[key].keys()){
            int parent_id = this->category_[i].first;
            QString name = this->category_[i].second;
            sub_category_data[key][parent_id][name] += data.second[key][i];
        }
    }
    // Objekte definieren
    container_layout_ = new QHBoxLayout();

    for (QString key: data.first.keys()){ // "income" oder "expense"?
        QPieSeries* pie_chart_series = new QPieSeries;
        QMap<int, double> category_data = data.first[key]; //QMap Element mit int = Category id und double = Betrag
        for (int i: category_data.keys()){ // Iteration über das Kategorie-Element und hinzufügen der Series
            // Hinzufügen als Kategorie in Kreisdiagramm
            // Erstes Element Name der Kategorie, zweites Element Summe der Kategorie
            QPieSlice* slice = new QPieSlice();
            slice->setLabel(category_group_[i]);
            slice->setValue(category_data[i]);
            slice->setProperty("id", i);
            pie_chart_series->append(slice);
        }
        // ToolTips setzen
        // werden ausgelöst, wenn Nutzer auf Slice im Kreisdiagramm klickt
        // -> Nutzer werden dann die Unterkategorien aufgelistet
        // hover effekt funktioniert leider nicht, Anzeigedauer lässt sich nicht überschreiben da sonst das
        // hover Signal überschrieben werden müsste...
        for (QPieSlice* slice : pie_chart_series->slices()) {

            const int parent_id = slice->property("id").toInt();
            QString label = slice->label();
            QString value = QString::number(slice->value());
            QString toolTipText = "<html><strong>" + label + ": " + value + " " + this->currency_ + "</strong><br><hr><br>";
            toolTipText += "<b>Zugeordnete Unterkategorien:</b><br><br>";

            for (QString sub_name: sub_category_data[key][parent_id].keys()){
                toolTipText += sub_name + ": " + "<em>" + QString::number(sub_category_data[key][parent_id][sub_name]) + this->currency_ + "</em><br>";
            }
            toolTipText += "</html>";

            QObject::connect(slice, &QPieSlice::clicked, [this, slice, toolTipText]() {
                QToolTip::showText(QCursor::pos(), toolTipText, this, QRect(), 100000);
            });
            QObject::connect(slice, &QPieSlice::hovered, [slice](bool state) {
                if (state){
                    QApplication::setOverrideCursor(Qt::PointingHandCursor);
                    slice->setExploded(true);
                    slice->setLabelVisible(true);
                }
                if (!state) {
                    slice->setExploded(false);
                    slice->setLabelVisible(false);
                    QToolTip::hideText();
                    QApplication::restoreOverrideCursor();
                }
            });
        }
        // Neues Chart Objekt initialisieren und Daten hinzufügen
        chart_ = new QChart();
        chart_->addSeries(pie_chart_series);
        (key == "income") ? chart_->setTitle("Einnahmen") : chart_->setTitle("Ausgaben");

        chart_->legend()->setAlignment(Qt::AlignLeft);
        chart_->setContentsMargins(0, 0, 0, 0);
        chart_->setAnimationOptions(QChart::AllAnimations);
        chart_->setTheme(QChart::ChartThemeBlueIcy);
        chart_->setBackgroundBrush(Qt::NoBrush);

        chartView_ = new QChartView(chart_, this);
        chartView_->setRenderHint(QPainter::Antialiasing);
        chartView_->setStyleSheet("background: transparent");

        container_layout_->addWidget(chartView_);
    }

    container_ = new QWidget(this);
    container_->setLayout(container_layout_);

}

// Bereitet die Daten für das Balkendiagramm vor
// Rückgabewert: QMap<Periode(int), <Summe_Einnahmen (dbl), Summe_Ausgaben (dbl)>>
QMap<int, QPair<double, double>> Chart::getDataForBarChart_()
{
    // Outputobjekt definieren
    // out.first = Einnahmen, out.second = Ausgaben (absolut)
    QMap<int, QPair<double, double>> out;
    // Query der Transaktionen abrufen
    QSqlQuery q = this->ptrStateMgr_->getTransactions(); // Hier ist aktueller Zeitraum schon gesetzt
    // Zeitraum abrufen
    QPair<QDate, QDate> time_period = this->ptrStateMgr_->getTimePeriod();
    QDate date_from = time_period.first;
    QDate date_to = time_period.second;
    int time_delta = date_from.daysTo(date_to);

    // Property des Diagrammtyps setzen
    // > 365 Tage ? Diagrammtyp = Vergleiche Jahre untereinander, ansonsten Monatsvergleich (12 Monate oder weniger)
    (time_delta > 365) ? chart_type_ = "YoY" : chart_type_ = "MoM";

    // Daten aufbereiten für Diagrammtyp "Monatsvergleich"
    int hash_key = 0; // kann Monat 1-12 oder Jahr 2024, 2025, etc. annehmen
    double amount = 0; // Betrag aus Datenbank
    QString cost_type = ""; // kann 'income' oder 'expense' annehmen

    while (q.next()){
        QString db_date = q.value("transaction_date").toString();
        QDateTime dateTime = QDateTime::fromString(db_date, "yyyy-MM-dd HH:mm:ss.zzzzzz"); // Versuch 1
        if (!dateTime.isValid()) dateTime = QDateTime::fromString(db_date, "yyyy-MM-dd HH:mm:ss"); // Versuch 2
        if (dateTime.isValid()) {
            // Extrahieren des Monats oder des Jahres
            (chart_type_ == "MoM") ? hash_key = dateTime.date().month()
                                   : hash_key = dateTime.date().year();
        } else {
            // hash_key auf 0 setzen
            // eventuell handling für diese Datensätze einbauen
            // sollte allerdings nicht passieren da Validierung in Datenbank eingebaut ist
            hash_key = 0;
        }
        bool ok;
        amount = q.value("amount").toDouble(&ok);
        // Hat Konvertierung geklappt?
        if (ok){
            // Prüfen ob es sich um eine Einnahme oder Ausgabe handelt
            // Dafür wird einfach das Vorzeichen analysiert
            // Einnahme- und Ausgabekostenarten sind nicht vorgesehen
            (amount >= 0) ? cost_type = "income" : cost_type = "expense";
            // Jetzt werden die Daten in das QHash hinzugefügt
            if (cost_type == "income"){
                out[hash_key].first += amount;
            }
            else if (cost_type == "expense"){
                amount = amount *-1; // Vorzeichen rumdrehen
                out[hash_key].second += amount;
            }
        }
    }
    qDebug() << out;

    return out;

}

// Bereitet die Daten für das Kreisdiagramm vor
QPair<QMap<QString, QMap<int, double>>, QMap<QString, QMap<int, double>>> Chart::getDataForPieChart_()
{
    QSqlQuery q = this->ptrStateMgr_->getTransactions(); // Aktuelle Transaktionen abrufen

    // QMap --> QString (Income oder Expense), QMap (Daten über Kategoriegruppe -> int id, double amount)
    using Category = QMap<QString, QMap<int, double>>;
    // QMap --> int (Income oder Expense, QMap (Daten über Kategoriegruppe -> int Unterkategorie, double amount)
    using SubCategory = QMap<QString, QMap<int, double>>;

    Category category_group_output;
    SubCategory sub_category_output;

    // Output als Pair
    QPair<Category, SubCategory> out;

    // Lokale Variablen für q.next
    QString cost_type;
    double amount = 0;

    while (q.next()){
        amount = q.value("amount").toDouble();
        (amount >= 0) ? cost_type = "income" : cost_type = "expense";
        amount = std::abs(amount);

        // Kategorien
        int category_id = q.value("category_group_id").toInt();
        auto& category_group = category_group_output[cost_type]; // wird angelegt falls nicht vorhande
        category_group[category_id] += amount;

        // Unterkategorien
        int sub_category_id = q.value("category_id").toInt();
        auto& sub_category = sub_category_output[cost_type];
        sub_category[sub_category_id] += amount;
    }

    out.first = category_group_output;
    out.second = sub_category_output;
    return out;
    // for (QString key: out.first.keys()){
    //     qDebug() << "Parent / Kategorie" << key;
    //     for (int i: out.first[key].keys()){
    //         qDebug() << category_group_[i] << "Total:" << out.first[key][i];
    //     }
    // }
    // for (QString key: out.second.keys()){
    //     qDebug() << "Parent / Unterkategorie" << key;
    //     for (int i: out.second[key].keys()){
    //         qDebug() << category_[i] << "Total:" << out.second[key][i];
    //     }
    // }

}
