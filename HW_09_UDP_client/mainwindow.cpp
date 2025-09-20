#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    portTime = 12345;
    portMes = 13000;
    udpWorker = new UDPworker(this);
    udpMes = new UDPworker(this);
    udpWorker->InitSocket(portTime);
    udpMes->InitSocket(portMes);

    connect(udpWorker, &UDPworker::sig_sendTimeToGUI, this, &MainWindow::DisplayTime);
    connect(udpMes, &UDPworker::sig_sendUserText, this, &MainWindow::DisplayText);   // добавлен слот для приема пользовательской датаграммы

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [&]{

        QDateTime dateTime = QDateTime::currentDateTime();

        QByteArray dataToSend;
        QDataStream outStr(&dataToSend, QIODevice::WriteOnly);

        outStr << dateTime;

        udpWorker->SendDatagram(dataToSend);

    });

}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_pb_start_clicked()
{
    timer->start(TIMER_DELAY);
}


void MainWindow::DisplayTime(QDateTime data)
{    
    counterPck++;
    if(counterPck % 20 == 0){
        ui->te_result->clear();
    }    
    ui->te_result->append("Текущее время: " + data.toString() + ". "
                "Принято пакетов " + QString::number(counterPck));
}

// Добавлен слот для приема данных датаграммы от пользователя
void MainWindow::DisplayText(QString text, QHostAddress addr, int s)
{

    ui->te_result->append("\nПринято сообщение от " + addr.toString() + ", " +
                          "размер сообщения (байт) " + QString::number(s) + "\n" +
                          "Сообщение: " + text + "\n");

}

void MainWindow::on_pb_stop_clicked()
{
    timer->stop();
}

// Отправка текста по нажатию на кнопку "Отправить датаграмму"
void MainWindow::on_pb_send_dg_clicked()
{    
    QByteArray sendText;
    QString t(ui->le_userText->text());    
    sendText.append(t.toUtf8());
    udpMes->SendDatagram(sendText);
}

