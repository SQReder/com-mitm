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

    forever {
        auto code = reader.readCode();
        cout << "Got code    : " << code << endl;

        auto ok = false;
        int number = code.toInt(&ok);
        qDebug() << " " << number;
        qDebug() << ok;
        int low = number & 0x0000ffff;
        int high = (number & 0x00ff0000) >> 16;
        QString result;
        result.append(QString::number(high))
              .append(",")
              .append(QString::number(low));
        cout << "Converted to: " << result << endl;
    }

    reader.Close();
}
