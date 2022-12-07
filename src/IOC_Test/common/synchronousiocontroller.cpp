#include "synchronousiocontroller.h"
#include "ioctrlcommController.h"

#include <QTest>
#include <QSignalSpy>

SynchronousIoController::SynchronousIoController(QString port, QObject *parent) :
    QThread(parent)
{
    ioCommCtrl = new IOCtrlCommController(port, parent);
}

void SynchronousIoController::run()
{
    exec();
}

void SynchronousIoController::enterTestMode()
{
    ioCommCtrl->sendTestModeCMD(0x01);
}

void SynchronousIoController::exitTestMode()
{
    ioCommCtrl->sendTestModeCMD(0x02);
}

void SynchronousIoController::setPwmValue(quint16 channel, quint16 pwmValue)
{
    ioCommCtrl->sendTestSetPwmCMD(channel, pwmValue);
}

bool waitForSignal(const QSignalSpy *spy)
{
    const int maxAttempts = 10;
    int attempts = 0;

    while (spy->count() == 0 && attempts != maxAttempts) {
        QTest::qWait(50);
        attempts++;
    }

    return (attempts < maxAttempts) ? true : false;
}

quint16 SynchronousIoController::analogIn(quint16 channel)
{
    QSignalSpy spy(ioCommCtrl, SIGNAL(adcMessage(quint16, quint16)));

    ioCommCtrl->sendTestGetAdcCMD(channel);

    bool didReceiveSignal = waitForSignal(&spy);

    if (didReceiveSignal) {
        QList<QVariant> arguments = spy.takeFirst();
        return arguments.at(1).toUInt();
    } else {
        qDebug("Timed out while waiting for ADC signal");
        return 0;
    }
}

quint32 SynchronousIoController::pulseDetectIn(quint16 channel)
{
    QSignalSpy spy(ioCommCtrl, SIGNAL(pulseMessage(quint16, quint32)));

    ioCommCtrl->sendTestGetPulsePalpFreqCMD(channel);

    bool didReceiveSignal = waitForSignal(&spy);

    if (didReceiveSignal) {
        QList<QVariant> arguments = spy.takeFirst();
        return arguments.at(1).toUInt();
    } else {
        qDebug("Timed out while waiting for pulse signal");
        return 0;
    }
}

quint16 SynchronousIoController::gpio(quint16 pin)
{
    QSignalSpy spy(ioCommCtrl, SIGNAL(gpioMessage(quint16, quint16)));

    ioCommCtrl->sendTestGetIOCMD(pin);

    bool didReceiveSignal = waitForSignal(&spy);

    if (didReceiveSignal) {
        QList<QVariant> arguments = spy.takeFirst();
        return arguments.at(1).toUInt();
    } else {
        qDebug("Timed out while waiting for GPIO signal");
        return 2;
    }
}

void SynchronousIoController::setGpio(quint16 pin, quint16 value)
{
    ioCommCtrl->sendTestSetIOCMD(pin, value);
}
