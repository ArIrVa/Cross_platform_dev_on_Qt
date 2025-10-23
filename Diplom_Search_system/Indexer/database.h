#ifndef DATABASE_H
#define DATABASE_H

// #include <QSqlQueryModel>
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

// enum RequestType{
//     reqNull = 0,
//     reqListAirports,
//     reqArrivalAirplans,
//     reqDepartureAirplans,
//     reqStatEveryMonth,
//     reqStatEveryDay
// };

class DataBase : public QObject
{
    Q_OBJECT

public:
    explicit DataBase(QObject *parent = nullptr);
    ~DataBase();

public slots:
    bool connectToDB(const QString& driver, const QVector<QString>& data);
    void disconnectFromDataBase();

    // QSqlError getLastError();

private slots:
    void createTables(const QSqlDatabase &db);

private:
    QSqlDatabase *db_;

};

#endif // DATABASE_H
