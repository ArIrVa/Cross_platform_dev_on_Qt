#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    this->setWindowTitle("Films");

    //Исходное состояние виджетов
    ui->setupUi(this);
    ui->lb_statusConnect->setStyleSheet("color:red");
    ui->pb_request->setEnabled(false);

    /*
     * Выделим память под необходимые объекты. Все они наследники
     * QObject, поэтому воспользуемся иерархией.
    */

    dataDb = new DbData(this);
    dataBase = new DataBase(this);
    msg = new QMessageBox(this);
    // tbView = new QTableView(this);

    //Установим размер вектора данных для подключения к БД
    dataForConnect.resize(NUM_DATA_FOR_CONNECT_TO_DB);

    /*
     * Добавим БД используя стандартный драйвер PSQL и зададим имя.
    */
    dataBase->AddDataBase(POSTGRE_DRIVER, DB_NAME);

    /*
     * Устанавливаем данные для подключениея к БД.
     * Поскольку метод небольшой используем лямбда-функцию.
     */
    connect(dataDb, &DbData::sig_sendData, this, [&](QVector<QString> receivData){
        dataForConnect = receivData;
    });

    /*
     * Соединяем сигнал, который передает ответ от БД с методом, который отображает ответ в ПИ
     */     
    connect(dataBase, &DataBase::sig_SendDataFromDB, this, &MainWindow::ScreenDataFromDB);

    /*
     *  Сигнал для подключения к БД
     */
    connect(dataBase, &DataBase::sig_SendStatusConnection, this, &MainWindow::ReceiveStatusConnectionToDB);
    connect(dataBase, &DataBase::sig_SendStatusRequest, this, &MainWindow::ReceiveStatusRequestToDB);    
}

MainWindow::~MainWindow()
{
    ///Отключиться от БД
    dataBase->DisconnectFromDataBase(DB_NAME);

    // if(tbView){delete tbView;}

    delete ui;
}

/*!
 * @brief Слот отображает форму для ввода данных подключения к БД
 */
void MainWindow::on_act_addData_triggered()
{
    //Отобразим диалоговое окно. Какой метод нужно использовать?
    dataDb->show();
}

/*!
 * @brief Слот выполняет подключение к БД. И отображает ошибки.
 */

void MainWindow::on_act_connect_triggered()
{
    /*
     * Обработчик кнопки у нас должен подключаться и отключаться от БД.
     * Можно привязаться к надписи лейбла статуса. Если он равен
     * "Отключено" мы осуществляем подключение, если "Подключено" то
     * отключаемся
    */

    if(ui->lb_statusConnect->text() == "Отключено"){

       ui->lb_statusConnect->setText("Подключение");
       ui->lb_statusConnect->setStyleSheet("color : black");

       auto connectToDb = [this]{ dataBase->ConnectToDataBase(dataForConnect); };
       QFuture<void> futConn = QtConcurrent::run(connectToDb);
    }
    else{

       ///Отключение от базы данных
        dataBase->DisconnectFromDataBase(DB_NAME);
        ui->lb_statusConnect->setText("Отключено");
        ui->act_connect->setText("Подключиться");
        ui->lb_statusConnect->setStyleSheet("color:red");
        ui->pb_request->setEnabled(false);
    }
}

/*!
 * \brief Обработчик кнопки "Получить"
 */
void MainWindow::on_pb_request_clicked()
{
/*
 * В случае если ответ получен, то мы его читаем
 * в противном случае выводим ошибку. Сейчас мы разберем
 * получение всех фильмов. А дома вы получите отдельно комедии
 * и ужасы
*/
    // ///Запрос к БД        

    if(ui->cb_category->currentText() == "Все")
    {
        auto reqToDb = [this]{dataBase->RequestToDB_allMovies();};
        QFuture<void> futRcvData = QtConcurrent::run(reqToDb);
    }

    if(ui->cb_category->currentText() == "Комедия")
    {
        ui->tbView->setModel(nullptr);
        auto reqToDb = [this]{dataBase->RequestToDB(reqComedy);};
        QFuture<void> futRcvData = QtConcurrent::run(reqToDb);
    }

    if(ui->cb_category->currentText() == "Ужасы")
    {
        ui->tbView->setModel(nullptr);
        auto reqToDb = [this]{dataBase->RequestToDB(reqHorror);};
        QFuture<void> futRcvData = QtConcurrent::run(reqToDb);
    }
}


/*!
 * \brief Слот отображает значение в QTableView
 * \param tView
 * \param typeRequest
 */

void MainWindow::ScreenDataFromDB(QAbstractItemModel*tView, int typeRequest)
{
    int columns = 14;
    switch (typeRequest) {

    case requestAllFilms:        
        ui->tbView->setModel(tView);
        ui->tbView->hideColumn(0);
        for(int i = 0; i < (columns - 3); ++i)
        {
            ui->tbView->hideColumn(i+3);
        }
        break;
    case requestHorrors:
        ui->tbView->setModel(tView);
        break;
    case requestComedy:
        ui->tbView->setModel(tView);
        break;

    default:
        break;
    }
}

/*!
 * \brief Метод изменяет стотояние формы в зависимости от статуса подключения к БД
 * \param status
 */
void MainWindow::ReceiveStatusConnectionToDB(bool status)
{
    if(status){
        ui->act_connect->setText("Отключиться");
        ui->lb_statusConnect->setText("Подключено к БД");
        ui->lb_statusConnect->setStyleSheet("color:green");
        ui->pb_request->setEnabled(true);
    }
    else{
        ///Отключиться  от БД
        dataBase->DisconnectFromDataBase(DB_NAME);
        msg->setIcon(QMessageBox::Critical);
        msg->setText(dataBase->GetLastError().text());
        ui->lb_statusConnect->setText("Отключено");
        ui->lb_statusConnect->setStyleSheet("color:red");
        msg->exec();
    }
}

/*!
 * \brief Метод обрабатывает ответ БД на поступивший запрос
 * \param err
 */
void MainWindow::ReceiveStatusRequestToDB(const QString &err)
{
    if(err != ""){
        msg->setText(err);
        msg->show( );
    }
    else{
        dataBase->ReadAnswerFromDB(ui->cb_category->currentIndex() + 1);
    }
}

void MainWindow::on_pb_clear_clicked()
{
    ui->tbView->setModel(nullptr);
}




