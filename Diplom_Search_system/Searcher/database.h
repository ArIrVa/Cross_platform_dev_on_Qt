#ifndef DATABASE_H
#define DATABASE_H

#include <QSqlTableModel>
#include <QSqlQueryModel>
#include <QSqlQuery>
#include <QObject>
#include <QSqlDatabase>
#include <QSqlError>


#define POSTGRE_DRIVER "QPSQL"
#define NUM_DATA_FOR_CONNECT_TO_DB 5

enum FieldsForConnect{
    hostName = 0,
    dbName = 1,
    login = 2,
    pass = 3,
    port = 4
};

class DataBase : public QObject
{
    Q_OBJECT

public:
    explicit DataBase(QObject *parent = nullptr);
    ~DataBase();

public slots:
    void addDataBase(const QString& driver);
    void connectToDataBase_status(const QVector<QString>& data);
    void disconnectFromDataBase();    
    QSqlError getLastError();

signals:
    void sig_SendStatusConnection(bool);
    void sig_SendStatusRequest(const QString& err);


private:
    QSqlDatabase* dataBase_;

};

#endif // DATABASE_H
