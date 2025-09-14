#ifndef DATABASE_H
#define DATABASE_H

#include <QSqlTableModel>
#include <QSqlQueryModel>
#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>


#define POSTGRE_DRIVER "QPSQL"
#define DB_NAME "MyDB"

//Количество полей данных необходимых для подключения к БД
#define NUM_DATA_FOR_CONNECT_TO_DB 5

//Перечисление полей данных
enum fieldsForConnect{
    hostName = 0,
    dbName = 1,
    login = 2,
    pass = 3,
    port = 4
};

//Типы запросов
enum requestType{

    requestAllFilms = 1,
    requestComedy   = 2,
    requestHorrors  = 3

};



class DataBase : public QObject
{
    Q_OBJECT

public:
    explicit DataBase(QObject *parent = nullptr);
    ~DataBase();

    void AddDataBase(QString driver, QString nameDB = "");
    void ConnectToDataBase(QVector<QString> dataForConnect);
    void DisconnectFromDataBase(QString nameDb = "");
    void RequestToDB_allMovies();
    void RequestToDB(QString request);
    void ReadAnswerFromDB( int answerType );
    QSqlError GetLastError(void);

signals:
    void sig_SendDataFromDB(QAbstractItemModel *tView, int typeR);
    void sig_SendStatusConnection(bool);
    void sig_SendStatusRequest(const QString err);


private:

    QSqlDatabase* dataBase;
    QSqlQuery* simpleQuery;
    QSqlQueryModel *queryModel;
    QSqlTableModel  *tableModel;
    // QTableView *tableView;


};

#endif // DATABASE_H
