#include "searcher.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Searcher w;
    w.show();
    return a.exec();
}
