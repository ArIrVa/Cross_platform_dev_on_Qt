#include "database.h"
#include <cstdint>
#include <qcontainerfwd.h>
#include <qsqldatabase.h>
#include <qsqlquery.h>

DataBase::DataBase(QObject *parent)
    : QObject{parent}
{
    /*Выделяем память под объекты классов.
     *Объект QSqlDatabase является основным классом низкого уровня,
     *в котором настраивается подключение к БД.
    */
    dataBase = new QSqlDatabase();
    queryModel = new QSqlQueryModel();
}

DataBase::~DataBase()
{
    delete dataBase;
    delete queryModel;
}

/*!
 * \brief Метод добавляет БД к экземпляру класса QSqlDataBase
 * \param driver драйвер БД
 * \param nameDB имя БД (Если отсутствует Qt задает имя по умолчанию)
 */
void DataBase::AddDataBase(QString driver, QString nameDB)
{
    /*  Класс QSqlDatabase представляет из себя интерфейс между
        кодом и БД. Он организует соединение с БД. Для подключения
        в конструктор класса необходимо передать драйвер БД и имя.
        В объекте может храниться несколько подключений, они различаются именами.
    */
    *dataBase = QSqlDatabase::addDatabase(driver, nameDB);

}

/*!
 * \brief Метод подключается к БД
 * \param для удобства передаем контейнер с данными необходимыми для подключения
 * \return возвращает тип ошибки
 */
void DataBase::ConnectToDataBase(QVector<QString> data)
{
    ///Для подключения необходимо передать параметры БД в объект класса.

    dataBase->database("nameDB"); //Выбор БД из списка QSQLDataBase

    dataBase->setHostName(data[hostName]);
    dataBase->setDatabaseName(data[dbName]);
    dataBase->setUserName(data[login]);
    dataBase->setPassword(data[pass]);
    dataBase->setPort(data[port].toInt());

    bool statusConnect = dataBase->open();

    tableModel = new QSqlTableModel(nullptr, *dataBase);

    emit sig_SendStatusConnection(statusConnect);
}

/*!
 * \brief Метод производит отключение от БД
 * \param Имя БД
 */
void DataBase::DisconnectFromDataBase(QString nameDb)
{
    //Сначала выбираем имя БД
    dataBase->database(nameDb);
    //Метод close() закрывает соединение с БД
    dataBase->close();
}

void DataBase::RequestToDB_allMovies()
{
    tableModel->setTable("film");
    tableModel->select();

    if (tableModel->lastError().isValid()) {
        qDebug() << "Ошибка при выборе данных: " << tableModel->lastError().text();
    }
    emit sig_SendStatusRequest(tableModel->lastError().text());
}

/*!
 * \brief Метод формирует запрос к БД.
 * \param request - SQL запрос
 * \return
 */
void DataBase::RequestToDB(QString request)
{
    queryModel->setQuery(request, *dataBase);

    if(queryModel->lastError().isValid()){
        emit sig_SendStatusRequest(queryModel->lastError().text());
    }
    emit sig_SendStatusRequest(queryModel->lastError().text());
}

/*!
 * \brief Метод читает ответ, полученный от БД при помощи QSqlQuery
 * \param answerType - тип ответа
 */
void DataBase::ReadAnswerFromDB(int requestType)
{   
    switch (requestType) {

    case requestAllFilms:        
        emit sig_SendDataFromDB(tableModel, requestType);
        break;
    case requestComedy:        
        emit sig_SendDataFromDB(queryModel, requestType);
        break;
    case requestHorrors:        
        emit sig_SendDataFromDB(queryModel, requestType);
        break;

    default:
        break;
    }    
}

/*!
 * @brief Метод возвращает последнюю ошибку БД
 */
QSqlError DataBase::GetLastError()
{
    return dataBase->lastError( );
}







