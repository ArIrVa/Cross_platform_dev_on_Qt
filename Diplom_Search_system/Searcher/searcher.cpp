#include "searcher.h"
#include "./ui_searcher.h"

#include <QDebug>

Searcher::Searcher(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Searcher)
{
    ui->setupUi(this);

    db_ = new DataBase(this);
    msg_ = new QMessageBox(this);

    dataForConnect_.resize(NUM_DATA_FOR_CONNECT_TO_DB);
    db_->addDataBase(POSTGRE_DRIVER);

    connect(db_, &DataBase::sig_SendStatusConnection, this, &Searcher::receiveStatusConnectToDB);    

    ui->tabWidget->setTabText(0, "Поиск");
    ui->tabWidget->setTabText(1, "Все слова");
    ui->tabWidget->setCurrentIndex(0);

    ui->lb_result->setText("");

    this->resize(600, 400);
    this->setWindowTitle("Поисковик");

    ui->lb_statusConnect->setText("Отключено");
    ui->lb_statusConnect->setStyleSheet("color:red");

    ui->pb_searchButton->setEnabled(false);

    ui->chb_fullName->setText("Показать полный путь к файлу");


    ui->tblView_resultsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tblView_resultsTable->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tblView_resultsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tblView_resultsTable->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

    ui->tblView_allWordsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tblView_allWordsTable->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tblView_allWordsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tblView_allWordsTable->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

    dataForConnect_ = readConfigFile();
    connectToDB();
}

Searcher::~Searcher()
{
    db_->disconnectFromDataBase();
    delete msg_;
    delete ui;
}

const QVector<QString> Searcher::readConfigFile()
{
    QVector<QString>data;
    data.resize(NUM_DATA_FOR_CONNECT_TO_DB);
    QString filePath = "../../../config.ini";
    if (!QFile::exists(filePath)) {
        qWarning() << "Файл конфигурации не найден!";
    }

    QSettings settings(filePath, QSettings::IniFormat);

    data[hostName] = settings.value("database/host").toString();
    data[port] = settings.value("database/port").toString();
    data[login] = settings.value("database/username").toString();
    data[pass] = settings.value("database/password").toString();
    data[dbName] = settings.value("database/dbname").toString();

    return data;
}

void Searcher::connectToDB()
{
    auto connectToDb = [this]{ db_->connectToDataBase_status(dataForConnect_); };
    QFuture<void> futConn = QtConcurrent::run(connectToDb);
}

QStringList Searcher::cleanOfSymbols(const QString &text)
{
    QStringList cleanWords;
    QMap<QString, int>uniqWords;
    QString lowerText = text.trimmed().toLower();
    lowerText.remove(QRegularExpression(R"([^\p{L}\d\s])"));
    QStringList words = lowerText.split(QRegularExpression(R"(\s+)"), Qt::SkipEmptyParts);

    for (const QString &word : words)
    {
        if (word.length() >= 3 && word.length() <= 32) {
            uniqWords[word]++;
        }
    }

    for (const auto& word : uniqWords.keys()) {
        cleanWords.append(word);
    }

    return cleanWords;
}

void Searcher::receiveStatusConnectToDB(bool status)
{
    if(status){
        ui->lb_statusConnect->setText("Подключено");
        ui->lb_statusConnect->setStyleSheet("color:green");
        ui->pb_searchButton->setEnabled(true);
    }    
    else{
        db_->disconnectFromDataBase();
        msg_->setIcon(QMessageBox::Critical);
        msg_->setText(db_->getLastError().text());
        ui->lb_statusConnect->setText("Отключено");
        ui->lb_statusConnect->setStyleSheet("color:red");
        msg_->exec();

        QTimer::singleShot(5000, this, SLOT(connectToDB()));

    }
}

void Searcher::on_pb_searchButton_clicked()
{
    ui->tblView_resultsTable->setModel(nullptr);
    QStringList wordList = cleanOfSymbols(ui->le_searchInput->text());
    if (wordList.isEmpty()) {
        QMessageBox::warning(this, "Предупреждение", "Вы ничего не ввели или введенное слово короче 3 символов!");
        return;
    }

    QStringList quoteList;
    for (const QString &temp : wordList)
    {
        quoteList << QString("'%1'").arg(temp);
    }

    if (wordList.size() > 4) { // Ограничиваем число слов до четырех
        QMessageBox::warning(this, "Предупреждение", "Максимальное количество слов — 4!");
        return;
    }

    QString optionFileName;
    if(ui->chb_fullName->isChecked())
    {
        optionFileName = "d.path";
    }
    else
    {
        optionFileName = "d.file_name";
    }

    QString sqlQuery = QString("SELECT %1, SUM(dwf.frequency)\n"
                           "FROM doc_word_freq dwf\n"
                           "INNER JOIN documents_table d ON d.id = dwf.doc_id\n"
                           "INNER JOIN words_table w ON w.id = dwf.word_id\n"
                           "WHERE w.word IN (%2)\n"
                           "GROUP BY %1\n"
                           "HAVING COUNT(DISTINCT dwf.word_id) = %3\n"
                           "ORDER BY SUM(dwf.frequency) DESC\n"
                           "LIMIT 10;").arg(optionFileName).arg(quoteList.join(",")).arg(QString::number(quoteList.size()));


    QSqlQueryModel* model = new QSqlQueryModel(db_);
    model->setQuery(sqlQuery);

    if (model->lastError().isValid()) {
        QMessageBox::critical(this, "Ошибка", "Ошибка выполнения запроса: " + model->lastError().text());
        return;
    }

    int rowCount = model->rowCount();
    if (rowCount == 0) {
        ui->lb_result->setText("Нет результатов поиска.");
    } else {
        QStringList headers{"Файл", "Суммарная частота"};
        model->setHeaderData(0, Qt::Horizontal, headers.at(0));
        model->setHeaderData(1, Qt::Horizontal, headers.at(1));
        ui->tblView_resultsTable->setModel(model);
        ui->lb_result->clear();
    }
}

void Searcher::on_pb_close_clicked()
{
    close();
}

void Searcher::on_tabWidget_currentChanged(int index)
{
    int tabIdx = ui->tabWidget->currentIndex();
    if (tabIdx == 1)
    {        
        QString sqlQuery = QString("SELECT w.word, SUM(dwf.frequency)\n"
                                   "FROM doc_word_freq dwf\n"
                                   "INNER JOIN words_table w ON w.id = dwf.word_id\n"
                                   "GROUP BY w.word\n"
                                   "ORDER BY SUM(dwf.frequency) DESC;");
        QSqlQueryModel* model = new QSqlQueryModel(db_);
        model->setQuery(sqlQuery);

        if (model->lastError().isValid()) {
            QMessageBox::critical(this, "Ошибка", "Ошибка выполнения запроса: " + model->lastError().text());
            return;
        }

        QStringList columns {"Слово", "Частота появления"};
        model->setHeaderData(0, Qt::Horizontal, columns.at(0));
        model->setHeaderData(1, Qt::Horizontal, columns.at(1));
        ui->tblView_allWordsTable->setModel(model);
    }
}

