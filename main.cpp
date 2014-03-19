#include <QCoreApplication>
#include <QDebug>
#include <QThread>
#include <windows.h>
#include "cardreader.h"
#include "codereader.h"

QT_USE_NAMESPACE

QSerialPort out;
CardReader reader;
CodeReader *bufferedReader;

BOOL CtrlHandler( DWORD fdwCtrlType )
{
  switch( fdwCtrlType )
  {
    // Handle the CTRL-C signal.
    case CTRL_C_EVENT:
    // CTRL-CLOSE: confirm that the user wants to exit.
    case CTRL_CLOSE_EVENT:
    // Pass other signals to the next handler.
    case CTRL_BREAK_EVENT:
    case CTRL_LOGOFF_EVENT:
    case CTRL_SHUTDOWN_EVENT:
        out.close();
        reader.Close();
        printf("\n\nOpened ports closed\nNow exit\n");
        QThread::sleep(1);
      return false;
    default:
      return FALSE;
  }
}


QByteArray ProcessCode(QByteArray code) {
    QTextStream cout(stdout);
    auto ok = false;
    int number = code.simplified().toInt(&ok);

//    qDebug() << " " << number;
//    qDebug() << ok;

    int low = number & 0x0000ffff;
    int high = (number & 0x00ff0000) >> 16;

    QByteArray result;
    result.append(QString::number(high))
          .append(",")
          .append(QString::number(low));
    cout << "Converted to:  " << result << endl;
    result.prepend("Em-Marine[1901] ");
    result.append(10).append(13);

    return result;
}


int main(int argc, char *argv[])
{
    if( SetConsoleCtrlHandler( (PHANDLER_ROUTINE) CtrlHandler, TRUE ) )
      {
        printf( "The Control Handler is installed.\n" );
      }
      else
      {
        printf( "\nERROR: Could not set control handler");
        return 1;
      }

    QTextStream cout(stdout);
    QTextStream cin(stdin);

    reader.PrintAvaliablePorts();

    int inputPort = 4;
    int outputPort = 5;

    cout << "Select port, if you agreed with default value just press Enter" << endl
         << "Select input port (4): ";
    cout.flush();
    auto str = cin.read(1);
    bool ok;
    int in = str.toInt(&ok);
    if (ok)
        inputPort = in;

    cout << "Select output port (5): ";
    cout.flush();
    str = cin.read(1);
    in = str.toInt(&ok);
    if (ok)
        outputPort = in;

//    reader.SelectComByNumber(inputPort);
//    reader.Open();

    auto buffer = QSharedPointer<QList<QByteArray>>(new QList<QByteArray>());

    bufferedReader = new CodeReader(inputPort, buffer);
    QThread *thread = new QThread();
    bufferedReader->moveToThread(thread);
    QObject::connect( thread, SIGNAL(started()), bufferedReader, SLOT(doWork()) );
    QObject::connect( bufferedReader, SIGNAL(workFinished()), thread, SLOT(quit()) );
    //automatically delete thread and task object when work is done:
    QObject::connect( thread, SIGNAL(finished()), bufferedReader, SLOT(deleteLater()) );
    QObject::connect( thread, SIGNAL(finished()), thread, SLOT(deleteLater()) );
    thread->start();


    try {
        auto serialInfo = reader.GetComInfoByNumber(outputPort);
        out.setPort(serialInfo);
        qDebug() << "Opening output port (" << outputPort << ")";

        if (out.open(QIODevice::WriteOnly)) {
            qDebug() << "Success open outer port";
            out.setBaudRate(QSerialPort::Baud9600);
            out.setParity(QSerialPort::NoParity);
            out.setFlowControl(QSerialPort::SoftwareControl);
        } else {
            qCritical() << "Can't open outer port";
            exit(1);
        }
    } catch (...) {
        qDebug() << "FATAL ERROR WHILE OPEN OUT PORT";
        out.close();
        reader.Close();
        exit(1);
    }

    try {
        qDebug() << "Start reading code" << bufferedReader;
        forever {
            if (buffer->isEmpty()) {
                QThread::msleep(1);
                continue;
            }

            auto code = buffer->takeFirst();
            qDebug();
            qDebug() << "Got code    : " << code;

            auto result = ProcessCode(code);
//            qDebug() << result;
            auto written = out.write(result.data(), result.length());
            out.waitForBytesWritten(-1);
            qDebug() << "Written " << written;
            qDebug();
        }
    } catch (std::exception &e) {
        qDebug() << "Cough exception: " << e.what();
    } catch (...) {
        qDebug() << "\n\nUNKNOWN FATAL ERROR\n\n";
    }

    reader.Close();
    out.close();
}
