#ifndef HW_04_MAINWINDOW_H
#define HW_04_MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class HW_04_MainWindow;
}
QT_END_NAMESPACE

class HW_04_MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    HW_04_MainWindow(QWidget *parent = nullptr);
    ~HW_04_MainWindow();

private slots:
    void on_pb_send_clicked();

private:
    Ui::HW_04_MainWindow *ui;
    int currentValue_;
    const int step_{10};
    int count_ {0};
};
#endif // HW_04_MAINWINDOW_H
