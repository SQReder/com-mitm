#include "cardreader.h"
#include <QDebug>
#include <windows.h>

QT_USE_NAMESPACE

void CardReader::PrintPortInfo(const QSerialPortInfo &info)
{
    QTextStream cout(stdout);
    cout << "Name        : " << info.portName() << endl
         << "Description : " << info.description() << endl
         << "Manufacturer: " << info.manufacturer() << endl
         << "Is busy     : " << info.isBusy() << endl
         << endl;
}

CardReader::CardReader(QObject *parent) :
    QObject(parent)
{
    UpdateAvaliablePorts();
}

void CardReader::PrintAvaliablePorts()
{
    foreach (auto info, avaliablePortsInfo) {
        PrintPortInfo(info);
    }
}

void CardReader::UpdateAvaliablePorts()
{
    avaliablePortsInfo.clear();
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {
        avaliablePortsInfo.insert(info.portName(), info);
    }
}

void CardReader::SelectComByNumber(int number)
{
    QTextStream cout(stdout);
    QString name("COM");
    name = name + QString::number(number).simplified();
    if (avaliablePortsInfo.contains(name)) {
        serial.setPort(avaliablePortsInfo[name]);
        cout << "Use port " << name << endl;
    } else
        qCritical() << "There is no port COM" << number  << "!";
}

const QSerialPortInfo &CardReader::GetComInfoByNumber(int number)
{
    QString name("COM");
    name = name + QString::number(number).simplified();
    if (avaliablePortsInfo.contains(name)) {
        return avaliablePortsInfo[name];
    }
    else
        throw;
}

/**
 * @brief CardReader::Open
 * @return true if port has benn opened successfully else false
 */
bool CardReader::Open()
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

void CardReader::Close()
{
    if (serial.isOpen())
        serial.close();
}

QByteArray CardReader::readCode()
{
    QByteArray buff;
    bool toAdd = false;
    forever {
        if(serial.waitForReadyRead(1000)) {
            auto byte = serial.read(1);
            if (byte.toHex() == "02") {
                toAdd = true;
                continue;
            }
            if (byte.toHex() == "03")
                break;
            if (toAdd)
                buff.append(byte);
        } else {
            QTextStream  cout(stdout);
            cout << ".";
            cout.flush();
        }
    }
    qDebug() << buff;
    return buff;
}
