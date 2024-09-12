#ifndef TESTPULSEDRIVER_H
#define TESTPULSEDRIVER_H
 
#include <QObject>
#include <QSemaphore>
 
#include "itestcase.h"
#include "ioctrlcommController.h"
 
class TestPulseDriver
    : public QObject,
      public ITestCase
{
    Q_OBJECT
public:
    explicit TestPulseDriver(IOCtrlCommController * const controller,
                             quint16 channel,
                             quint16 expectedLow,
                             quint16 expectedHigh,
                             QObject *parent = 0);
 
    virtual QString getName() const;
    virtual void setReporter(ITestReporter *reporter);
    virtual void runTest();
 
private slots:
    quint16 actualChannel(quint16 channel);
    void setPwmValue(quint16 channel, quint16 value);
    void receivedAdcMessage(quint16, quint16 value);
 
private:
    IOCtrlCommController *m_controller;
    ITestReporter *m_reporter;
    quint16 m_pdChannel;
    quint16 m_adcChannel;
    quint16 m_expectedLow;
    quint16 m_expectedHigh;
 
    QSemaphore m_semaphore;
    quint16 m_receivedValue;
};
 
#endif // TESTPULSEDRIVER_H
