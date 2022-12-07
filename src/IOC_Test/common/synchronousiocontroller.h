#ifndef SYNCHRONOUSIOCONTROLLER_H
#define SYNCHRONOUSIOCONTROLLER_H

#include <QObject>
#include <QThread>


/*
const uint8_t BP_SENSOR     = 14;
const uint8_t DEFIB_SENSOR  = 11;
const uint8_t MANIKIN_ID    = 12;
const uint8_t AD_CH_1       = 11;
const uint8_t AD_CH_2       = 12;
const uint8_t AD_CH_3       = 4;
const uint8_t AD_CH_4       = 15;
const uint8_t AD_CH_5       = 8;
const uint8_t AD_CH_6       = 9;
*/

/** ADC channel numbers as defined in the IO Controller */
enum AnalogChannels {
    AnalogIn1 = 11,
    AnalogIn2 = 12,
    AnalogIn3 = 4,
    AnalogIn4 = 15,
    AnalogIn5 = 8,
    AnalogIn6 = 9
};

enum IoLines {
    McuGpio0 = 1,
    McuGpio1 = 2,
    McuGpio2 = 3,
    ManikinGpo1 = 4,
    ManikinGpo2 = 5
};

class IOCtrlCommController;

class SynchronousIoController : public QThread
{
    Q_OBJECT
public:
    explicit SynchronousIoController(QString port, QObject *parent = 0);

    void run();

    void enterTestMode();
    void exitTestMode();
    void setPwmValue(quint16 channel, quint16 pwmValue);
    quint16 analogIn(quint16 channel);
    quint32 pulseDetectIn(quint16 channel);
    quint16 gpio(quint16 pin);
    bool testGpioToPower();
    void setGpio(quint16 pin, quint16 value);

private:
    IOCtrlCommController *ioCommCtrl;

signals:

public slots:

};

#endif // SYNCHRONOUSIOCONTROLLER_H
