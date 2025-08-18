#include "stopwatch.h"

Stopwatch::Stopwatch(QObject *parent)
    : QObject{parent}, timer_(this)
{
    connect(&timer_, &QTimer::timeout, this, &Stopwatch::updateTime);
}

Stopwatch::~Stopwatch() {}

void Stopwatch::start()
{
    if(!isRunning_)
    {
        isRunning_ = true;
        timer_.start(1);
    }
}

void Stopwatch::stop()
{
    if(isRunning_)
    {
        isRunning_ = false;
        timer_.stop();
    }
}

void Stopwatch::reset()
{
    stop();
    elapsedMilliSec_ = 0;
    emit updated("0.0");
}

QString Stopwatch::currentTime()
{
    return QString("%1").arg(elapsedMilliSec_ / 1000.0f, 0, 'f', 1);
}

bool Stopwatch::isRunning() const
{
     return isRunning_;
}

void Stopwatch::updateTime()
{
    ++elapsedMilliSec_;
    emit updated(currentTime());
}


