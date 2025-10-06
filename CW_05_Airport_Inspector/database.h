#ifndef DATABASE_H
#define DATABASE_H

#include <QSqlTableModel>
#include <QSqlQueryModel>
#include <QObject>
#include <QSqlDatabase>
#include <QDate>
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

enum RequestType{
    reqNull = 0,
    reqListAirports,
    reqArrivalAirplans,
    reqDepartureAirplans,
    reqStatEveryMonth,
    reqStatEveryDay
};

class DataBase : public QObject
{
    Q_OBJECT

public:
    explicit DataBase(QObject *parent = nullptr);
    ~DataBase();

public slots:
    void addDataBase(const QString& driver);
    void connectToDataBase(const QVector<QString>& data);
    void disconnectFromDataBase();
    void requestToDB(const RequestType& reqType, const QString &airportCode = "",
                     const QDate &data = QDate(0, 0, 0));
    QSqlQueryModel* getQueryModel(const RequestType& reqType);
    QSqlError getLastError();

signals:
    void sig_SendStatusConnection(bool);
    void sig_SendStatusRequest(const QString& err);


private:
    QSqlDatabase* dataBase_;
    QHash<RequestType, QSqlQueryModel*> queryModel_;

    struct RequestString{
        const QString listAirports(){
            return  "SELECT airport_name->>'ru' as \"airportName\", airport_code FROM\
                    bookings.airports_data\
                    order by \"airportName\"";
        }
        const QString inAirplans(const QString &airportCode, const QDate date){
            return  "SELECT flight_no, scheduled_arrival, ad.airport_name->>'ru' as \"Name\" from bookings.flights f\
                    JOIN bookings.airports_data ad on ad.airport_code = f.departure_airport\
                    where f.arrival_airport = '" + airportCode + "'\
                    and f.scheduled_arrival::date = date('"
                    + QString::number(date.year()) + "-"
                    + QString::number(date.month()) + "-"
                    + QString::number(date.day()) + "')\
                    order by \"Name\"";
        }
        const QString outAirplans(const QString &airportCode, const QDate date){
            return  "SELECT flight_no, scheduled_departure, ad.airport_name->>'ru' as \"Name\" from bookings.flights f\
                    JOIN bookings.airports_data ad on ad.airport_code = f.arrival_airport\
                    WHERE f.departure_airport = '" + airportCode + "'\
                    and f.scheduled_departure::date = date('"
                    + QString::number(date.year()) + "-"
                    + QString::number(date.month()) + "-"
                    + QString::number(date.day()) + "')\
                    order by \"Name\"";
        }
        const QString statEveryMonth(const QString &airportCode){
            return   "SELECT count(flight_no), date_trunc('month', scheduled_departure) as \"Month\" from bookings.flights f\
                    WHERE (scheduled_departure::date > date('2016-08-31') and\
                    scheduled_departure::date <= date('2017-08-31')) and\
                    ( departure_airport = '" + airportCode + "' or arrival_airport = '" + airportCode + "' )\
                    group by \"Month\"";
        }
        const QString statEveryDay(const QString &airportCode){
            return  "SELECT count(flight_no), date_trunc('day', scheduled_departure) as \"Day\" from bookings.flights f\
                    WHERE(scheduled_departure::date > date('2016-08-31') and\
                    scheduled_departure::date <= date('2017-08-31')) and\
                    ( departure_airport = '" + airportCode + "' or arrival_airport = '" + airportCode + "')\
                    GROUP BY \"Day\"";
        }
    }request_;

};

#endif // DATABASE_H
