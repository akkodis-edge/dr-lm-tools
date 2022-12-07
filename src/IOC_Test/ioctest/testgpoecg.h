#ifndef TESTGPOECG_H
#define TESTGPOECG_H

#include <QObject>
#include <QSemaphore>
#include "itestcase.h"
#include "ioctrlcommController.h"

class TestGPOECG
    : public QObject,
      public ITestCase
{
    Q_OBJECT
public:
    explicit TestGPOECG(IOCtrlCommController * const controller,
                     QObject *parent = 0
        );

    virtual QString getName() const;
    virtual void setReporter(ITestReporter *reporter);
    virtual void runTest();

private slots:
    void receivedAdcMessage(quint16 channel, quint16 value);

private:
    bool closeEnough(quint16 input, quint16 target);

    IOCtrlCommController *m_controller;
    ITestReporter *m_reporter;

    quint16 m_expectedLow;
    quint16 m_expectedHigh;

    QSemaphore m_semaphore;
    quint16 m_receivedValue;
};

#endif /* TESTGPOECG_H */
