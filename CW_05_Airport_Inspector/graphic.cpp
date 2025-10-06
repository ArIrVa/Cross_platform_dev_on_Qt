#include "graphic.h"
#include "ui_graphic.h"

Graphic::Graphic(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Graphic),
    axisLineX_(nullptr), axisLineY_(nullptr),
    axisBarX_(nullptr), axisBarY_(nullptr),
    lineSer_(nullptr), barSer_(nullptr)
{
    ui->setupUi(this);

    setWindowModality(Qt::ApplicationModal);
    ui->tabWidget->setCurrentIndex(TabYear);

    chartBar_ = new QChart();
    chartLine_ = new QChart();
    chartBar_->legend()->hide();
    chartLine_->legend()->hide();

    viewBar_ = new QChartView(chartBar_);
    viewLine_ = new QChartView(chartLine_);

    ui->layoutBar->addWidget(viewBar_);
    ui->layoutLine->addWidget(viewLine_);

    connect(ui->tabWidget, &QTabWidget::currentChanged, this, &Graphic::choiceTab);
}

Graphic::~Graphic()
{
    delete chartBar_;
    delete chartLine_;
    delete viewBar_;
    delete viewLine_;
    delete ui;
}

void Graphic::addData(Tab idx, QMap<QDate, int> &statistic, QString &airportName)
{
    ui->lb_airportName->setText("Аэропорт \"" + airportName + "\"");

    switch (idx) {
    case TabYear:
    {
        if (barSer_ == nullptr){
            barSer_ = new QBarSeries(this);
            chartBar_->addSeries(barSer_);
            chartBar_->setAnimationOptions(QChart::SeriesAnimations);
        }

        int max(0);
        QBarSet *barSet = new QBarSet("Bar", this);
        for (auto it = statistic.begin(); it != statistic.end(); ++it){
            auto date = it.key();
            QString date_str = MONTH.at(date.month() - 1) + " " + QString::number(date.year());

            categories_.append(date_str);
            barSet->append(it.value());

            if (max < it.value()){
                max = it.value();
            }
        }
        barSer_->append(barSet);

        if (axisBarX_ == nullptr){
            axisBarX_ = new QBarCategoryAxis(this);
            chartBar_->addAxis(axisBarX_, Qt::AlignBottom);
            axisBarX_->setLabelsAngle(270);
        }
        axisBarX_->append(categories_);
        barSer_->attachAxis(axisBarX_);

        if (axisBarY_ == nullptr){
            axisBarY_ = new QValueAxis(this);
            chartBar_->addAxis(axisBarY_, Qt::AlignLeft);
            axisBarY_->setTitleText("Кол-во прилетов/вылетов");
            axisBarY_->setLabelFormat("%u");
        }
        axisBarY_->setRange(0, max);
        barSer_->attachAxis(axisBarY_);
        break;
    }

    case TabMonth:
    {
        ui->cb_month->clear();
        const int DEFAULT_IDX(4);

        statYearOfDay_ = qMove(statistic);

        auto it = statYearOfDay_.constKeyValueBegin();
        auto end_it = statYearOfDay_.constKeyValueEnd();

        int oldMonth(0);
        while(it != end_it)
        {
            int month = it.base().key().month();
            if (oldMonth != month){
                oldMonth = month;
                QString date_str = MONTH.at(month - 1) + " " + QString::number(it.base().key().year());
                ui->cb_month->addItem(date_str);
                listDataIt_.append(it.base());
            }
            ++it;
        }
        listDataIt_.append(end_it.base());

        ui->cb_month->setCurrentIndex(DEFAULT_IDX);
        choiceMonth(DEFAULT_IDX);
        break;
    }

    default:
        break;
    }
}

void Graphic::closeEvent(QCloseEvent *event)
{
    event->accept();

    categories_.clear();
    listDataIt_.clear();

    if (barSer_->count()){
        barSer_->detachAxis(axisBarX_);
        barSer_->detachAxis(axisBarY_);
        barSer_->clear();
    }
    if (lineSer_ && lineSer_->count()){
        lineSer_->detachAxis(axisLineX_);
        lineSer_->detachAxis(axisLineY_);
        lineSer_->clear();
    }
}

void Graphic::choiceMonth(int dateIdx)
{
    if (lineSer_ == nullptr){
        lineSer_ = new QLineSeries(this);
        chartLine_->addSeries(lineSer_);
        chartLine_->setAnimationOptions(QChart::SeriesAnimations);
    }
    if (lineSer_->count()){
        lineSer_->detachAxis(axisLineX_);
        lineSer_->detachAxis(axisLineY_);
        lineSer_->clear();
    }

    auto currentMonth_it = listDataIt_.at(dateIdx);
    auto nextMonth_it = listDataIt_.at(dateIdx + 1);
    QDate date;
    int max(0), min((std::numeric_limits<int>::max()));
    while(currentMonth_it != nextMonth_it)
    {
        date = currentMonth_it.key();
        int value = currentMonth_it.value();
        lineSer_->append(date.day(), value);
        ++currentMonth_it;
        if (max < value){
            max = value;
        }else if (min > value){
            min = value;
        }
    }

    if (axisLineX_ == nullptr){
        axisLineX_ = new QValueAxis(this);
        chartLine_->addAxis(axisLineX_, Qt::AlignBottom);
        axisLineX_->setTitleText("День");
        axisLineX_->setLabelFormat("%u");
        axisLineX_->setTickCount(1);
    }
    axisLineX_->setRange(1, date.daysInMonth());
    lineSer_->attachAxis(axisLineX_);

    if (axisLineY_ == nullptr){
        axisLineY_ = new QValueAxis(this);
        chartLine_->addAxis(axisLineY_, Qt::AlignLeft);
        axisLineY_->setTitleText("Кол-во прилетов/вылетов");
        axisLineY_->setLabelFormat("%u");
    }
    axisLineY_->setRange(--min, ++max);
    axisLineY_->setTickCount(max - min + 1);
    lineSer_->attachAxis(axisLineY_);
}

void Graphic::choiceTab()
{
    int tabIdx = ui->tabWidget->currentIndex();

    if ((tabIdx == TabYear && categories_.empty()) ||
            (tabIdx == TabMonth && listDataIt_.empty()))
    {
        emit sig_requestData(tabIdx);
    }    
}

void Graphic::on_pB_close_clicked()
{
    close();
}

void Graphic::on_cb_month_activated(int index)
{
    choiceMonth(index);
}

