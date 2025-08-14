#include "hw_04_mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    HW_04_MainWindow w;
    w.show();
    return a.exec();
}
