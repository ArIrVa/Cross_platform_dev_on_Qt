#include "stopwatchwidget.h"
#include "./ui_stopwatchwidget.h"

StopWatchWidget::StopWatchWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::StopWatchWidget), stopwatch_(new Stopwatch(this))
{
    ui->setupUi(this);

    connect(stopwatch_, &Stopwatch::updated, this, &StopWatchWidget::displayTime);

    ui->pbn_circle->setEnabled(false);
    displayTime("0.0");
}

StopWatchWidget::~StopWatchWidget()
{
    delete ui;
    delete stopwatch_;
}

void StopWatchWidget::on_pbn_startstop_clicked()
{
    if(stopwatch_->isRunning())
    {
        stopwatch_->stop();
        ui->pbn_startstop->setText("Старт");
        ui->pbn_circle->setEnabled(false);
    }
    else
    {
        stopwatch_->start();
        ui->pbn_startstop->setText("Стоп");
        ui->pbn_circle->setEnabled(true);
    }
}

void StopWatchWidget::on_pbn_reset_clicked()
{
    stopwatch_->reset();
    ui->tb_circleBrowser->clear();
    circleCount_ = 0;
    lastCircleTime_ = 0.0;
    ui->pbn_circle->setEnabled(false);
    ui->pbn_startstop->setText("Старт");
}

void StopWatchWidget::on_pbn_circle_clicked()
{
    const auto currentTime = stopwatch_->currentTime().toDouble();
    double circleDuration = currentTime - lastCircleTime_;
    lastCircleTime_ = currentTime;
    ++circleCount_;
    QString text = QString("Круг %1, время: %2 сек").arg(circleCount_).arg(circleDuration, 0, 'f', 1);
    ui->tb_circleBrowser->append(text);
}

void StopWatchWidget::displayTime(const QString time)
{
    ui->lbl_time->setText(time);
}




