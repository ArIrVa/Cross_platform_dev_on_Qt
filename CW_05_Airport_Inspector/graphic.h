#ifndef GRAPHIC_H
#define GRAPHIC_H

#include <QWidget>
#include <QDebug>
#include <QGridLayout>
#include <QtCharts>
#include <QChart>
#include <QChartView>
#include <QBarSet>


namespace Ui {
class Graphic;
}

enum Tab{
    TabYear,
    TabMonth
};

class Graphic : public QWidget
{
    Q_OBJECT

public:
    explicit Graphic(QWidget *parent = nullptr);
    ~Graphic();
    void addData(Tab idx, QMap<QDate, int> &statistic, QString &airportName);

public slots:
    void choiceTab();

signals:
    void sig_requestData(int currTab);

private slots:
    void choiceMonth(int dateIdx);
    void closeEvent(QCloseEvent *event) override;    
    void on_pB_close_clicked();
    void on_cb_month_activated(int index);

private:
    Ui::Graphic *ui;
    QChart *chartBar_, *chartLine_;
    QChartView *viewBar_, *viewLine_;
    QBarSeries *barSer_;
    QLineSeries *lineSer_;
    QBarCategoryAxis *axisBarX_;
    QValueAxis *axisBarY_;
    QValueAxis *axisLineX_;
    QValueAxis *axisLineY_;

    const QStringList MONTH{
        "Январь",
        "Февраль",
        "Март",
        "Апрель",
        "Май",
        "Июнь",
        "Июль",
        "Август",
        "Сентябрь",
        "Октябрь",
        "Ноябрь",
        "Декабрь" };
    QMap<QDate, int> statYearOfDay_;
    QList<QMap<QDate, int>::ConstIterator> listDataIt_;
    QStringList categories_;

};

#endif // GRAPHIC_H
