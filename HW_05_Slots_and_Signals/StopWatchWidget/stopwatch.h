#ifndef STOPWATCH_H
#define STOPWATCH_H

#include <QObject>
#include <QTimer>

class Stopwatch : public QObject
{
    Q_OBJECT
public:
    explicit Stopwatch(QObject *parent = nullptr);
    ~Stopwatch();

    void start();
    void stop();
    void reset();
    bool isRunning() const;
    QString currentTime();

signals:
    void updated(QString);

private slots:
    void updateTime();    

private:
    bool isRunning_ = false;
    int elapsedMilliSec_ {0};
    QTimer timer_;
};

#endif // STOPWATCH_H
