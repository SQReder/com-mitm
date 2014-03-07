#include <QCoreApplication>
#include <QDebug>
#include <windows.h>
#include "cardreader.h"

QT_USE_NAMESPACE


int main(int argc, char *argv[])
{
    QTextStream cout(stdout);

    CardReader reader;
    reader.PrintAvaliablePorts();
    reader.SelectComByNumber(4);
    reader.Open();

    auto serialInfo = reader.GetComInfoByNumber(5);
    QSerialPort out;
    out.setPort(serialInfo);
    if (out.open(QIODevice::WriteOnly)) {
        out.setBaudRate(QSerialPort::Baud9600);
        out.setParity(QSerialPort::NoParity);
        out.setFlowControl(QSerialPort::NoFlowControl);
        QByteArray data("Hello world!");
        data.append(10).append(13);
//        auto written = out.write(data.data(), data.length());
    } else {
        qCritical() << "Can't open outer port";
        exit(1);
    }

    forever {
        auto code = reader.readCode();
        cout << "Got code    : " << code << endl;

        auto ok = false;
        int number = code.simplified().toInt(&ok);
        qDebug() << " " << number;
        qDebug() << ok;
        int low = number & 0x0000ffff;
        int high = (number & 0x00ff0000) >> 16;
        QByteArray result;
        result.append(QString::number(high))
              .append(",")
              .append(QString::number(low));
        cout << "Converted to: " << result << endl;
        result.prepend("Em-Marine[1901] ");
        result.append(10).append(13);
        qDebug() << result;
        auto written = out.write(result.data(), result.length());
        out.waitForBytesWritten(-1);
        qDebug() << "Written " << written;
    }

    reader.Close();
    out.close();
}
