#include "udpworker.h"

UDPworker::UDPworker(QObject *parent) : QObject(parent)
{

}

/*!
 * @brief Метод инициализирует UDP сервер
 */
void UDPworker::InitSocket(const quint16& p)
{
    port = p;
    serviceUdpSocket = new QUdpSocket(this);
    /*
     * Соединяем присваиваем адрес и порт серверу и соединяем функцию
     * обраотчик принятых пакетов с сокетом
     */
    serviceUdpSocket->bind(QHostAddress::LocalHost, port);

    connect(serviceUdpSocket, &QUdpSocket::readyRead, this, &UDPworker::readPendingDatagrams);
}

/*!
 * @brief Метод осуществляет обработку принятой датаграммы
 */
void UDPworker::ReadDatagram(QNetworkDatagram datagram)
{
    QByteArray data;

    data.resize(static_cast<int>(serviceUdpSocket->pendingDatagramSize()));
    data = datagram.data();


    QDataStream inStr(&data, QIODevice::ReadOnly);
    QDateTime dateTime;
    inStr >> dateTime;
    emit sig_sendTimeToGUI(dateTime);

    QHostAddress senderAddr;
    quint16 senderPort{};
    int size = datagram.data().size();
    senderAddr = datagram.senderAddress();
    senderPort = datagram.senderPort();
    if(senderPort == 13000)
    {
        emit sig_sendUserText(((data.toStdString().data())), senderAddr, size);
    }

}
/*!
 * @brief Метод осуществляет опередачу датаграммы
 */
void UDPworker::SendDatagram(QByteArray data)
{
    /*
     *  Отправляем данные на localhost и задефайненный порт
     */
    serviceUdpSocket->writeDatagram(data, QHostAddress::LocalHost, port);
}

/*!
 * @brief Метод осуществляет чтение датаграм из сокета
 */
void UDPworker::readPendingDatagrams( void )
{
    /*
     *  Производим чтение принятых датаграмм
     */
    while(serviceUdpSocket->hasPendingDatagrams()){
            QNetworkDatagram datagram = serviceUdpSocket->receiveDatagram();
            ReadDatagram(datagram);
    }

}

