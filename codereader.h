#ifndef CODEREADER_H
#define CODEREADER_H

#include <QObject>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QSharedPointer>

class CardReader;

class CodeReader : public QObject
{
    Q_OBJECT

    CardReader *informer;
    QSerialPort serial;
    QSharedPointer<QList<QByteArray>> buffer;
    QByteArray readCode();

    bool Open();
    void Close();
public:
    explicit CodeReader(int portNumber, QSharedPointer<QList<QByteArray>> buffer);
    ~CodeReader();

    QByteArray getCode();
signals:
    void workFinished();
public slots:
    void doWork();
};

#endif // CODEREADER_H
