#ifndef SEARCHER_H
#define SEARCHER_H

#include <QMainWindow>
#include <QMessageBox>
#include <QtConcurrent>
#include <QSettings>
#include <QFileDialog>
#include <QHeaderView>
#include <QStandardItemModel>
#include <QStringList>

#include "database.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class Searcher;
}
QT_END_NAMESPACE

class Searcher : public QMainWindow
{
    Q_OBJECT

public:
    explicit Searcher(QWidget *parent = nullptr);
    ~Searcher();

public slots:
    QStringList cleanOfSymbols(const QString &text);
    void receiveStatusConnectToDB(bool status);

signals:
    void sig_SendStatusConnection(bool);

private slots:
    const QVector<QString> readConfigFile();
    void connectToDB();
    void on_pb_searchButton_clicked();
    void on_pb_close_clicked();
    void on_tabWidget_currentChanged(int index);

private:
    Ui::Searcher *ui;
    QVector<QString> dataForConnect_;
    DataBase* db_;
    QMessageBox* msg_;    
};
#endif // SEARCHER_H
