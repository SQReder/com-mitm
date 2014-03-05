#pragma once

#include <QObject>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QMap>
//#include <QString>

class CardReader : public QObject
{
    Q_OBJECT

    QSerialPort serial;
    QMap<QString, QSerialPortInfo> avaliablePortsInfo;

    void PrintPortInfo(const QSerialPortInfo &info);
    void UpdateAvaliablePorts();
public:
    explicit CardReader(QObject *parent = 0);

    void PrintAvaliablePorts();
    void SelectComByNumber(int number);

    bool Open();
    void Close();

    QByteArray readCode();
signals:

public slots:

};
