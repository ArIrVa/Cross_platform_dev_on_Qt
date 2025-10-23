#include "indexer.h"

int main(int argc, char *argv[])
{
    Indexer indexer;

    indexer.run();
    qDebug() << "Работа Индексатора завершена";

    return EXIT_SUCCESS;
}
