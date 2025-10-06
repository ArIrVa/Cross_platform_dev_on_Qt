#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    dataBase_ = new DataBase(this);
    graphic_ = new Graphic();
    msg_ = new QMessageBox(this);

    dataForConnect_.resize(NUM_DATA_FOR_CONNECT_TO_DB);
    dataBase_->addDataBase(POSTGRE_DRIVER);

    connect(dataBase_, &DataBase::sig_SendStatusConnection, this, &MainWindow::receiveStatusConnectionToDB);
    connect(dataBase_, &DataBase::sig_SendStatusRequest, this, &MainWindow::receiveStatusRequestToDB);
    connect(graphic_, &Graphic::sig_requestData, this, &MainWindow::receiveReqData);

    ui->dateEdit->setDateRange(QDate(2016, 8, 15), QDate(2017, 9, 14));
    ui->lb_date->setFixedWidth(110);
    ui->rb_departure->toggle();    
    ui->lb_statusConnect->setText("Отключено");
    ui->lb_statusConnect->setStyleSheet("color:red");
    setEnable(false);

    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->tableView->horizontalHeader()->setStretchLastSection(true);

    dataForConnect_[hostName] = "981757-ca08998.tmweb.ru";
    dataForConnect_[dbName] = "demo";
    dataForConnect_[login] = "netology_usr_cpp";
    dataForConnect_[pass] = "CppNeto3";
    dataForConnect_[port] = "5432";

    connectToDB();
}

MainWindow::~MainWindow()
{
    dataBase_->disconnectFromDataBase();
    delete msg_;
    delete ui;
}

void MainWindow::setEnable(bool enable)
{
    ui->frame_radioB->setEnabled(enable);
    ui->frame_date->setEnabled(enable);
    ui->pb_reciveListFlights->setEnabled(enable);
    ui->pb_airportActivity->setEnabled(enable);
}

void MainWindow::connectToDB()
{
    auto connectToDb = [this]{ dataBase_->connectToDataBase(dataForConnect_); };
    QFuture<void> futConn = QtConcurrent::run(connectToDb);
}

void MainWindow::screenDataFromDB()
{
    QSqlQueryModel *queryModel(dataBase_->getQueryModel(reqType_));

    switch (reqType_) {
    case reqListAirports:
        setEnable(true);
        ui->cb_Airport->setModel(queryModel);
        break;

    case reqArrivalAirplans:
        queryModel->setHeaderData(0, Qt::Horizontal, tr("Номер\nрейса"));
        queryModel->setHeaderData(1, Qt::Horizontal, tr("Время\nприбытия"));
        queryModel->setHeaderData(2, Qt::Horizontal, tr("Аэропорт\nотправления"));
        ui->tableView->setModel(queryModel);
        break;

    case reqDepartureAirplans:
        queryModel->setHeaderData(0, Qt::Horizontal, tr("Номер\nрейса"));
        queryModel->setHeaderData(1, Qt::Horizontal, tr("Время\nвылета"));
        queryModel->setHeaderData(2, Qt::Horizontal, tr("Аэропорт\nназначения"));
        ui->tableView->setModel(queryModel);
        break;

    case reqStatEveryMonth:
    case reqStatEveryDay:
    {
        QMap<QDate, int> statistic;

        int rowMax(queryModel->rowCount());
        for (int row(0); row < rowMax; ++row){
            auto keyIdx = queryModel->index(row, 1);
            auto dataIdx = queryModel->index(row, 0);

            QDate key = queryModel->data(keyIdx).toDate();
            int data = queryModel->data(dataIdx).toInt();

            statistic.insert(key, data);
        }

        QString name = ui->cb_Airport->currentText();
        Tab idx = (reqType_ == reqStatEveryMonth) ? TabYear : TabMonth;
        graphic_->addData(idx, statistic, name);

        graphic_->choiceTab();
        break;
    }

    default:
        break;
    }
    reqType_ = reqNull;
}

void MainWindow::receiveStatusRequestToDB(const QString &err)
{
    if(err != ""){
        msg_->setText(err);
        msg_->show( );
    }
    else{
        screenDataFromDB();
    }
}

void MainWindow::on_rb_arrival_toggled(bool checked)
{
    ui->lb_date->setText("Дата прилета");
    ui->groupBox->setTitle("Аэропорт прибытия");
}

void MainWindow::on_rb_departure_toggled(bool checked)
{
    ui->lb_date->setText("Дата вылета");
    ui->groupBox->setTitle("Аэропорт отбытия");
}

void MainWindow::receiveStatusConnectionToDB(bool status)
{
    if(status){
        ui->lb_statusConnect->setText("Подключено");
        ui->lb_statusConnect->setStyleSheet("color:green");

        reqType_ = reqListAirports;
        auto reqToDb = [&]{ dataBase_->requestToDB(reqType_); };
        auto runRequest = QtConcurrent::run(reqToDb);

    }
    else{       
        dataBase_->disconnectFromDataBase();
        msg_->setIcon(QMessageBox::Critical);
        msg_->setText(dataBase_->getLastError().text());
        ui->lb_statusConnect->setText("Отключено");
        ui->lb_statusConnect->setStyleSheet("color:red");
        msg_->exec();

        QTimer::singleShot(5000, this, SLOT(connectToDB()));
    }
}

QString MainWindow::getAirportID()
{
    QString airportID;
    if (ui->cb_Airport->count() == 0)
    {
        QMessageBox::critical(0, tr("Ошибка!"), "Список пуст!",
                              QMessageBox::StandardButton::Close);
    }
    else
    {
        auto row = ui->cb_Airport->currentIndex();
        auto model = ui->cb_Airport->model();
        auto idx = model->index(row, 1);
        airportID = model->data(idx).toString();
    }
    return airportID;
}

void MainWindow::receiveReqData(int currTab)
{
    if (currTab == TabYear)
    {
        reqType_ = reqStatEveryMonth;
    }
    else if (currTab == TabMonth)
    {
        reqType_ = reqStatEveryDay;
    }

    QString airportID = getAirportID();

    auto reqToDb = [this](const RequestType req, const QString &str){
        dataBase_->requestToDB(req, str);
    };
    auto runRequest = QtConcurrent::run(reqToDb, reqType_, airportID);
}

void MainWindow::flightSchedule()
{
    reqType_ = (ui->rb_arrival->isChecked()) ? reqArrivalAirplans : reqDepartureAirplans;
    QString airportID = getAirportID();

    auto reqToDb = [this](const RequestType& req, const QString &str, const QDate& date){
        dataBase_->requestToDB(req, str, date);
    };
    auto runRequest = QtConcurrent::run(reqToDb, reqType_, airportID, ui->dateEdit->date());
}

void MainWindow::on_pb_reciveListFlights_clicked()
{
    flightSchedule();
}

void MainWindow::on_pb_airportActivity_clicked()
{
    // Расчет новой позиции для дочернего окна (справа от родительского)
    int offset = 10;
    int posX = geometry().right() + offset;
    int posY = geometry().top() - 37;

    graphic_->move(posX, posY);
    graphic_->choiceTab();
    graphic_->show();
}



