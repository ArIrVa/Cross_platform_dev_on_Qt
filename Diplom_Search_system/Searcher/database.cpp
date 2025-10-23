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

void DataBase::connectToDataBase_status(const QVector<QString>& data)
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

QSqlError DataBase::getLastError()
{
    return dataBase_->lastError( );
}







