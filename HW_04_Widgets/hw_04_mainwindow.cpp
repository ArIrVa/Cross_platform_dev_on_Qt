#include "hw_04_mainwindow.h"
#include "./ui_hw_04_mainwindow.h"

HW_04_MainWindow::HW_04_MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::HW_04_MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("Виджеты");

    ui->lbl_1->setText("Коэффициент");

    ui->cb_coefficient->setFixedSize(100,30);

    ui->pb_send->toggle();

    ui->progressBar->setFormat("%p%");
    ui->progressBar->setRange(0, 100);
    ui->progressBar->setValue(0);
    currentValue_ = ui->progressBar->value();
}

HW_04_MainWindow::~HW_04_MainWindow()
{
    delete ui;
}

void HW_04_MainWindow::on_pb_send_clicked()
{
    currentValue_ = currentValue_ + step_;
    if (ui->progressBar->value() < ui->progressBar->maximum())
    {
        ui->progressBar->setValue(currentValue_);
    }
    else
    {
        currentValue_ = 0;
        ui->progressBar->setValue(currentValue_);
    }
}

