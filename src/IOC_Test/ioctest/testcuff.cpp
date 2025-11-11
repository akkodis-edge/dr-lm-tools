#include "testcuff.h"
#include <QThread>
// #include "testmodewrapper.h"

TestCuff::TestCuff(IOCtrlCommController * controller,
                   quint32 threshold,
                   QObject *parent)
    : QObject(parent)
    , m_controller(controller)
    , m_reporter(0)
    , m_channel(0)
    , m_maxReceived(0)
    , m_threshold(threshold)
    , m_semaphore(0)
{ }

QString TestCuff::getName() const
{
    return QString("Cuff");
}

void TestCuff::setReporter(ITestReporter *reporter)
{
    m_reporter = reporter;
}

void TestCuff::receivedMessage(quint32 value)
{
    qDebug() << "TestCuff::receivedMessage() value:" << value;
    if (value >= m_threshold)
    {
        m_maxReceived = std::max<quint32>(value, m_maxReceived);
        m_semaphore.release();
    }
}

void TestCuff::runTest()
{
    // Parameters for retry
    const int maxRetries = 3;     // Maximum number of retry attempts
    const int retryDelay= 3;       // Retry delay of 3 seconds

    // Try connecting to IOC and running the test
    for (int attempt = 0; attempt < maxRetries; ++attempt)
    {
        qDebug() << "Attempting to connect to IOC, attempt" << (attempt + 1);

        // Connect the cuff signal to the receivedMessage() slot
        connect(m_controller, SIGNAL(cuffMessage(quint32)),
                this, SLOT(receivedMessage(quint32)),
                Qt::AutoConnection);

        // Wait for message with a timeout
        if (m_semaphore.tryAcquire(1, 5000))  // Wait for 5 seconds
        {
            // Successfully received a message, no need to retry
            return;
        }
        else
        {
            // No response or max value below threshold, retry
            qDebug() << "No response from IOC or max value (" << m_maxReceived << ") below threshold (" << m_threshold << ")";
            if (attempt < maxRetries - 1) // Only wait if we are going to retry again
            {
		qDebug() << "Make sure to set the cuff meter to a value over 40mmHg with the pump inflator";
                QThread::sleep(1);
		qDebug() << "Retrying connection to IOC in" << retryDelay << "second(s)...";
                QThread::sleep(retryDelay);  // Delay before retrying
            }
        }
    }

    // If all retries fail, report failure
    m_reporter->testHasFailed(
        QString("Failed to connect to IOC after %1 attempts. Max value (%2) below threshold (%3)")
        .arg(maxRetries).arg(m_maxReceived).arg(m_threshold));
}
