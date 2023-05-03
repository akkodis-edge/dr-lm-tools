#ifndef TESTGPO_H
#define TESTGPO_H

#include <QObject>
#include <QSemaphore>
#include "itestcase.h"
#include "ioctrlcommController.h"

class TestGPO
    : public QObject,
      public ITestCase
{
    Q_OBJECT
public:
    explicit TestGPO(IOCtrlCommController * const controller,
                     quint16 channel,
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

    quint16 m_channel;
    quint16 m_expectedLow;
    quint16 m_expectedHigh;

    QSemaphore m_semaphore;
    quint16 m_receivedValue;
};

#endif /* TESTGPO_H */
