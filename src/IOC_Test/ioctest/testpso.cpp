#include "testpso.h"

#include "testmodewrapper.h"

TestPSO::TestPSO(IOCtrlCommController *controller,
                 quint16 channel,
                 quint32 expectedFreq,
                 QObject *parent)
    : QObject(parent)
    , m_controller(controller)
    , m_reporter(0)
    , m_channel(channel)
    , m_semaphore(0)
    , m_expectedFreq(expectedFreq)
    , m_receivedFreq(0)
{ }

quint16 TestPSO::ioChannel(quint16 ch)
{
    if (ch >= 1 && ch <= 6)
        return ch - 1;
    else
        return 0;
}

QString TestPSO::getName() const
{
    return QString("Pulse_det_in%1").arg(m_channel);
}

void TestPSO::setReporter(ITestReporter *reporter)
{
    m_reporter = reporter;
}

void TestPSO::receivedPulseMessage(quint16 channel, quint32 frequency)
{
    if (channel == ioChannel(m_channel))
    {
        m_receivedFreq = frequency;
        m_semaphore.release();
    }
}

bool TestPSO::closeEnough(quint32 input, quint32 target)
{
    return (input > 0.9*target)
        && (input < 1.1*target);
}

void TestPSO::runTest()
{
    m_reporter->setLogTestHeader(getName() + QString(" (%1),").arg(m_expectedFreq));
    qDebug("%s, %u\n", getName().toStdString().c_str(), m_expectedFreq);

    bool connected = connect(
        m_controller, SIGNAL(pulseMessage(quint16, quint32)),
        this, SLOT(receivedPulseMessage(quint16, quint32)));
    
    if (not connected)
    {
        m_reporter->testHasFailed("Failed to connect callback!");
        m_reporter->logResult("Error,");
        return;
    }

    TestModeWrapper testMode(m_controller);   // raii

    m_semaphore.release(m_semaphore.available());
    m_controller->sendTestGetPulsePalpFreqCMD(ioChannel(m_channel));
    if (not m_semaphore.tryAcquire(1, 1000))
    {
        m_reporter->testHasFailed("Reply from IO Controller timed out.");
        m_reporter->logResult("Error,");
        return;
    }

    m_reporter->logResult(QString("%1,").arg(m_receivedFreq));
    if (not closeEnough(m_receivedFreq, m_expectedFreq))
    {
        m_reporter->testHasFailed(
            QString("Value outside expected range: Got %1, expected %2")
            .arg(m_receivedFreq).arg(m_expectedFreq));
        return;
    }
}
