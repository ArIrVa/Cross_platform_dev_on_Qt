#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QtConcurrent>
#include <QDate>
#include <QTimer>

#include "database.h"
#include "graphic.h"


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();


public slots:
    void screenDataFromDB();
    void receiveStatusConnectionToDB(bool status);
    void receiveStatusRequestToDB(const QString &err);

signals:
    void sig_RequestToDb(QString request);

private slots:
    void setEnable(bool enable);
    void flightSchedule();    
    void connectToDB();
    void receiveReqData(int currTab);
    void on_rb_arrival_toggled(bool checked);
    void on_rb_departure_toggled(bool checked);
    void on_pb_reciveListFlights_clicked();
    void on_pb_airportActivity_clicked();
    QString getAirportID();

private:    
    Ui::MainWindow *ui;
    QVector<QString> dataForConnect_;
    DataBase* dataBase_;
    QMessageBox* msg_;
    Graphic *graphic_;
    RequestType reqType_;

};
#endif // MAINWINDOW_H
