#ifndef STOPWATCHWIDGET_H
#define STOPWATCHWIDGET_H

#include <QWidget>
#include "stopwatch.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class StopWatchWidget;
}
QT_END_NAMESPACE

class StopWatchWidget : public QWidget
{
    Q_OBJECT

public:
    StopWatchWidget(QWidget *parent = nullptr);
    ~StopWatchWidget();

private slots:
    void on_pbn_startstop_clicked();
    void on_pbn_reset_clicked();
    void on_pbn_circle_clicked();
    void displayTime(const QString time);

private:
    Ui::StopWatchWidget *ui;
    Stopwatch* stopwatch_;
    int circleCount_{0};
    qreal lastCircleTime_{0.0};
};
#endif // STOPWATCHWIDGET_H
