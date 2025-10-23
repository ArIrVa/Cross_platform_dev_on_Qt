#include "indexer.h"

Indexer::Indexer(QObject *parent)
    : QObject{parent} {}

Indexer::~Indexer() {}

int Indexer::run() {
    // Открываем файл конфигурации
    QString filePath = "../../../config.ini";
    if (!QFile::exists(filePath)) {
        qWarning() << "Файл конфигурации не найден!";
    }

    // Чтение конфига из INI-файла
    QSettings settings(filePath, QSettings::IniFormat);
    QString indexPath = settings.value("indexer_settings/indexer_path").toString();
    QStringList extList = settings.value("indexer_settings/file_extensions").toString().split(",", Qt::SkipEmptyParts);

    QString dbHost = settings.value("database/host").toString();
    int dbPort = settings.value("database/port").toInt();
    QString username = settings.value("database/username").toString();
    QString password = settings.value("database/password").toString();
    QString dbname = settings.value("database/dbname").toString();

    // Установка соединения с базой данных PostgreSQL
    QSqlDatabase db = QSqlDatabase::addDatabase("QPSQL");
    db.setHostName(dbHost);
    db.setPort(dbPort);
    db.setUserName(username);
    db.setPassword(password);
    db.setDatabaseName(dbname);

    if (!db.open()) {
        qWarning() << "Ошибка подключения к базе данных:" << db.lastError().text();
        return EXIT_FAILURE;
    }

    // Создание нужных таблиц
    initializeDB(db);
    processDirectory(indexPath, extList, db);

    db.close();
    return EXIT_SUCCESS;
}

void Indexer::processDirectory(const QString &dirPath, const QStringList &extensions, const QSqlDatabase &db)
{
    QDir dir(dirPath);
    // Получаем список элементов текущей директории
    QStringList entries = dir.entryList(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot);
    foreach(const QString &entry, entries) {
        QString fullPath = dir.absoluteFilePath(entry);
        QFileInfo info(fullPath);
        if(info.isDir())
        {
            processDirectory(info.filePath(), extensions, db);
        }

        else if(info.isFile() && extensions.contains(info.suffix()))
        {
            processFile(fullPath, db);
        }
    }
}

QVector<QString> Indexer::cleanOfSymbols(const QString &text)
{
    QVector<QString> cleanWords;
    QString lowerText = text.toLower();
    lowerText.remove(QRegularExpression(R"([^\p{L}\d\s])"));
    QStringList parts = lowerText.split(QRegularExpression(R"(\s+)"), Qt::SkipEmptyParts);

    for (const QString &part : parts) {
        if (part.length() >= 3 && part.length() <= 32) {
            cleanWords.append(part);
        }
    }

    return cleanWords;
}

void Indexer::processFile(const QString &absolutePath,const QSqlDatabase &db)
{
    QFile file(absolutePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Невозможно открыть файл:" << absolutePath;
        return;
    }

    QByteArray data = file.readAll();
    QString content(data);
    QVector<QString> words = cleanOfSymbols(content);
    QFileInfo infoFile(absolutePath);
    QString nameFile;
    nameFile = infoFile.fileName();

    // Сохраняем файл в таблице документов
    bool isFindDoc = false;
    int idDoc = 0;
    isFindDoc = IsExistsDoc(db, absolutePath);
    if(!isFindDoc){
        idDoc = saveDocument(db, absolutePath, nameFile, content);
    }

    QMap<QString, int> wordFrequency;
    for (const auto& word : words)
    {
        wordFrequency[word]++;
    }

    // Сохраняем слова в таблицу слов
    bool isFindWord = false;
    int idWord = 0;
    int frequency = 0;
    for (const auto& word : wordFrequency.keys())
    {
        isFindWord = IsExistsWord(db, word);
        if(!isFindWord){
            idWord = saveWord(db, word);

        } else{
            idWord = findWordId(db, word);
        }
        // сохраняем слово файла
        frequency = wordFrequency[word];
        saveDocWord(db, idDoc, idWord, frequency);
    }

}

void Indexer::initializeDB(const QSqlDatabase &db) {
    QSqlQuery query(db);

    // удаляем существующие таблицы
    query.exec("DROP TABLE IF EXISTS doc_word_freq, documents_table, words_table;");
    // создаем таблицы
    query.exec("CREATE TABLE IF NOT EXISTS documents_table(\n"
               "id SERIAL PRIMARY KEY,\n"
               "file_name TEXT NOT NULL,\n"
               "path TEXT NOT NULL UNIQUE,\n"
               "content TEXT NOT NULL\n"
               ");\n"
               "\n"
               "CREATE TABLE IF NOT EXISTS words_table(\n"
               "id SERIAL PRIMARY KEY,\n"
               "word TEXT NOT NULL UNIQUE\n"
               ");\n"
               "\n"
               "CREATE TABLE IF NOT EXISTS doc_word_freq(\n"
               "doc_id INT REFERENCES documents_table(id),\n"
               "word_id INT REFERENCES words_table(id),\n"
               "frequency INT DEFAULT 1,\n"
               "PRIMARY KEY(doc_id, word_id));");

}

int Indexer::saveDocument(const QSqlDatabase &db, const QString &path, const QString &nameFile, const QString &content) {
    QSqlQuery query(db);
    bool isDoc = false;
    isDoc = IsExistsDoc(db, path);

    if (!isDoc){
        QString queryText = "INSERT INTO documents_table(path, file_name, content) VALUES(:path, :name, :content) RETURNING id";
        query.prepare(queryText);
        query.bindValue(":path", "'" + path + "'");
        query.bindValue(":name", "'" + nameFile + "'");
        query.bindValue(":content", "'" + content + "'");
    }
    if (!query.exec()) {
        qWarning() << "Ошибка сохранения документа:" << query.lastError().text();
        return -1;
    }
    else
    {
        if (query.next()) {
        return query.value(0).toInt();
        }
        else {
            return 0;
        }
    }
}

void Indexer::saveDocWord(const QSqlDatabase &db, const int &docId, const int &wordId, const int &frequency) {
    QSqlQuery query(db);
    query.prepare("INSERT INTO doc_word_freq(doc_id, word_id, frequency) VALUES(:docId, :wordId, :frequency)");
    query.bindValue(":docId", docId);
    query.bindValue(":wordId", wordId);
    query.bindValue(":frequency", frequency);

    if (!query.exec()) {
        qWarning() << "Ошибка сохранения слова файла:" << query.lastError().text();
    }
}

int Indexer::findDocId(const QSqlDatabase &db, const QString &path) {
    QSqlQuery query(db);
    query.prepare("SELECT id FROM documents_table WHERE path = :path");
    query.bindValue(":path", "'" + path + "'");

    if (!query.exec())
    {
        return 0;
    }
    else return query.value(0).toInt();
}

bool Indexer::IsExistsDoc(const QSqlDatabase &db, const QString &path) {
    QSqlQuery query(db);
    query.prepare("SELECT EXISTS(SELECT id FROM documents_table WHERE path=:path)");
    query.bindValue(":path", "'" + path + "'");

    if (query.next()) {
        return query.value(0).toBool();
    }
    else {
        return false;
    }
}

bool Indexer::IsExistsWord(const QSqlDatabase &db, const QString &word) {
    QSqlQuery query(db);
    QString query_text = "SELECT EXISTS(SELECT id FROM words_table WHERE word = '" + word + "')";
    query.prepare(query_text);

    if (!query.exec()) {
        qWarning() << "Ошибка проверки слова:" << query.lastError().text();
        return -1;
    }
    else
    {
        if (query.next()) {
            return query.value(0).toBool();
        }
        else {
            return 0;
        }
    }
}

int Indexer::findWordId(const QSqlDatabase &db, const QString &word) {
    QSqlQuery query(db);
    bool isFind = false;
    query.prepare("SELECT id FROM words_table WHERE word=:word");
    query.bindValue(":word", word);

    if (!query.exec()) {
        return 0;
    }
    else
    {
        if (query.next()) {
            return query.value(0).toInt();
        }
        else {
            return 0;
        }
    }
}

int Indexer::saveWord(const QSqlDatabase &db, const QString &word){
    QSqlQuery query(db);
    query.prepare("INSERT INTO words_table(word) VALUES(:word) RETURNING id");
    query.bindValue(":word", word);

    if (!query.exec()) {
        qWarning() << "Ошибка сохранения слова:" << query.lastError().text();
        return -1;
    }
    else
    {
        if (query.next()) {
            return query.value(0).toInt();
        }
        else {
            return 0;
        }
    }
}



