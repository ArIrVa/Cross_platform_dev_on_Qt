#ifndef INDEXER_H
#define INDEXER_H

#include <QObject>
#include <QString>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QDebug>
#include <QDirIterator>
#include <QRegularExpression>
#include <QMap>
#include <QSettings>


class Indexer : public QObject
{
    Q_OBJECT

public:
    explicit Indexer(QObject *parent = nullptr);
    ~Indexer();

public slots:
    int run();

private slots:
    // Метод чистки строки и разделение её на отдельные слова
    QVector<QString> cleanOfSymbols(const QString &text);

    // Обработка отдельного файла, чтение его содержания и индексация слова
    void processFile(const QString &absolutePath, const QSqlDatabase &db);

    // Рекурсивная обработка директории
    void processDirectory(const QString &dirPath, const QStringList &extensions, const QSqlDatabase &db);

    // Создание необходимых таблиц в базе данных
    void initializeDB(const QSqlDatabase &db);

    // Сохранение документа в таблицу документов   
    int saveDocument(const QSqlDatabase &db, const QString &path, const QString &nameFile, const QString &content);

    // Сохранение нового слова
    int saveWord(const QSqlDatabase &db, const QString &word);

    // Сохранение слова в таблицу слов файлов
    void saveDocWord(const QSqlDatabase &db, const int &docId, const int &wordId, const int &frequency);

    // Нахождение Id нового документа
    int findDocId(const QSqlDatabase &db, const QString &path);

    // Нахождение Id нового слова
    int findWordId(const QSqlDatabase &db, const QString &word);

    // Проверка наличия файла в базе
    bool IsExistsDoc(const QSqlDatabase &db, const QString &path);

    // Проверка наличия слова в базе
    bool IsExistsWord(const QSqlDatabase &db, const QString &word);

};



#endif // INDEXER_H
