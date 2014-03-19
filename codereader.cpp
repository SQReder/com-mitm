#include "codereader.h"
#include "cardreader.h"
#include <QException>
#include <QTextStream>
#include <QDebug>

CodeReader::CodeReader(int portNumber, QSharedPointer<QList<QByteArray>> buffer)
{
    this->buffer = buffer;
    informer = new CardReader();
    auto portInfo = informer->GetComInfoByNumber(portNumber);
    serial.setPort(portInfo);
    if (!Open())
        throw;
}

void CodeReader::Close()
{
    if (serial.isOpen())
        serial.close();
}

CodeReader::~CodeReader()
{
    Close();
}

QByteArray CodeReader::getCode()
{
    if (!buffer->isEmpty())
        return buffer->takeFirst();
    return QByteArray();
}


void CodeReader::doWork()
{
    forever {
        auto code = readCode();
        if (!code.isEmpty())
            buffer->append(code);
    }
}


QByteArray CodeReader::readCode()
{
    static QByteArray buff;
//    qDebug() << buff;

    auto to = buff.indexOf(0x3);
    if (to != -1) {
        auto from = buff.indexOf(0x2);
        auto code = buff.mid(from+1, to-3);
//        qDebug() << code;
        buff = buff.mid(to+1);
        return code;

    }

    bool toAdd = false;
    forever {
        if(serial.waitForReadyRead(1000)) {
            buff += serial.readAll();
//            if (byte.toHex() == "02") {
//                toAdd = true;
//                continue;
//            }
//            if (byte.toHex() == "03")
//                break;
//            if (toAdd)
//                buff.append(byte);
            break;
        } else {
            QTextStream  cout(stdout);
            cout << ".";
            cout.flush();
        }
    }
//    qDebug() << buff;
    return QByteArray();
//    return buff;
}


/**
 * @brief CardReader::Open
 * @return true if port has benn opened successfully else false
 */
bool CodeReader::Open()
{
    if (!serial.open(QIODevice::ReadWrite)) {
        qDebug() << "Can't open port " << serial.portName();
        qDebug() << "Error is " << serial.errorString();
        return false;
    }
    serial.setBaudRate(QSerialPort::Baud9600, QSerialPort::AllDirections);
    serial.setFlowControl(QSerialPort::NoFlowControl);

    return true;
}
