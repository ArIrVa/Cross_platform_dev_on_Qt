#include "database.h"

DataBase::DataBase(QObject *parent)
    : QObject{parent}
{
    db_ = new QSqlDatabase();
}

DataBase::~DataBase()
{
    delete db_;
}

// Подключение к базе данных
bool DataBase::connectToDB(const QString& driver, const QVector<QString>& data)
{
    *db_ = QSqlDatabase::addDatabase(driver);

    db_->setHostName(data[hostName]);
    db_->setDatabaseName(data[dbName]);
    db_->setUserName(data[login]);
    db_->setPassword(data[pass]);
    db_->setPort(data[port].toInt());

    if (!db_->open()){
        qCritical() << "Ошибка подключения к базе данных:" << db_->lastError().text();

        return false;
    }
    return true;
}

void DataBase::disconnectFromDataBase()
{    
    db_->database();
    db_->close();
}

// Создание структуры БД

void DataBase::createTables(const QSqlDatabase &db)
{
    QSqlQuery query(db);
    // Создание таблицы документов
    query.exec(
        "CREATE TABLE IF NOT EXISTS documents (\n"
        "id INTEGER PRIMARY KEY,\n"
        "path TEXT NOT NULL UNIQUE\n"
        ");"
        );
    // Создание таблицы слов
    query.exec(
        "CREATE TABLE IF NOT EXISTS words (\n"
        "id INTEGER PRIMARY KEY,\n"
        "word TEXT NOT NULL UNIQUE\n"
        ");"
        );
    // Создание таблицы связи 'many-to-many'
    query.exec(
        "CREATE TABLE IF NOT EXISTS word_frequency (\n"
        "document_id INTEGER REFERENCES documents(id),\n"
        "word_id INTEGER REFERENCES words(id),\n"
        "frequency INTEGER DEFAULT 0,\n"
        "PRIMARY KEY(document_id, word_id)\n"
        ");"
        );
}







// QSqlError DataBase::getLastError()
// {
//     return db_->lastError( );
// }







