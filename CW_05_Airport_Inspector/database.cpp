#include "database.h"

DataBase::DataBase(QObject *parent)
    : QObject{parent}
{    
    dataBase_ = new QSqlDatabase();
}

DataBase::~DataBase()
{
    delete dataBase_;
}

void DataBase::addDataBase(const QString& driver)
{    
    *dataBase_ = QSqlDatabase::addDatabase(driver);
}

void DataBase::connectToDataBase(const QVector<QString>& data)
{    
    dataBase_->setHostName(data[hostName]);
    dataBase_->setDatabaseName(data[dbName]);
    dataBase_->setUserName(data[login]);
    dataBase_->setPassword(data[pass]);
    dataBase_->setPort(data[port].toInt());

    bool statusConnect = dataBase_->open();
    emit sig_SendStatusConnection(statusConnect);
}

void DataBase::disconnectFromDataBase()
{    
    dataBase_->database();
    dataBase_->close();
}

void DataBase::requestToDB(const RequestType& reqType, const QString &airportCode, const QDate& date)
{
    QString request;
    switch (reqType) {
    case reqListAirports:
        request = request_.listAirports();
        break;

    case reqArrivalAirplans:
        request = request_.inAirplans(airportCode, date);
        break;

    case reqDepartureAirplans:
        request = request_.outAirplans(airportCode, date);
        break;

    case reqStatEveryMonth:
        request = request_.statEveryMonth(airportCode);
        break;

    case reqStatEveryDay:
        request = request_.statEveryDay(airportCode);
        break;

    default:
        break;
    }

    if (queryModel_.contains(reqType) == false)
    {
        queryModel_.insert(reqType, new QSqlQueryModel());
    }
    queryModel_.value(reqType)->setQuery(request, *dataBase_);
    auto error = queryModel_.value(reqType)->lastError().text();

    emit sig_SendStatusRequest(error);
}

QSqlQueryModel *DataBase::getQueryModel(const RequestType& reqType)
{
    return queryModel_.value(reqType, nullptr);
}

QSqlError DataBase::getLastError()
{
    return dataBase_->lastError( );
}







