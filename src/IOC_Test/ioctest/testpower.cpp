#include "testpower.h"

#include <fcntl.h>
#include <linux/input.h>
#include <poll.h>
#include <unistd.h>
#include <QTime>
#include <QThread>

#include "testmodewrapper.h"
#include "inputeventhandler.h"

TestPower::TestPower(IOCtrlCommController * controller,
                     quint16 timeout,
                     QObject *parent)
    : QObject(parent)
    , m_controller(controller)
    , m_reporter(0)
    , m_channel(3)              // GPIO2
    , m_timeout(timeout)
    , m_IOCcomm(0)
    , m_powerEvent(0) 
    , m_fd(0)
    , m_count(0)
{
}

QString TestPower::getName() const
{
    return QString("GPIO2, PowerButton");
}

void TestPower::setReporter(ITestReporter *reporter)
{
    m_reporter = reporter;
}

void TestPower::receivedMessage(quint16 channel, quint16 value)
{
    if (channel == m_channel)
    {
        m_receivedValue = value;
        m_IOCcomm.release();
    }
}

#define GET_VALUE \
    m_semaphore.release(m_semaphore.available());               \
    m_controller->sendTestGetIOCMD(m_channel);                  \
    if (not m_semaphore.tryAcquire(1, 1000))                    \
    {                                                           \
        m_reporter->testHasFailed("Receive value timed out.");  \
        return;                                                 \
    }

void TestPower::signalSemaphore(int value)
{
    if (value == 1)
    {
        qDebug() << "<<<ding>>>";
        m_powerEvent.release();
    }
}

void TestPower::readGPIO(int value)
{
    qDebug() << "readGPIO: enter";
    if (value == 1)
    {
        qDebug() << "readGPIO: _1";
        m_IOCcomm.release(m_IOCcomm.available());
        m_controller->sendTestGetIOCMD(m_channel);
        if (m_IOCcomm.tryAcquire(1, 200))
        {
            qDebug() << "readGPIO: _2";
            if (m_receivedValue == 1)
            {
                qDebug() << "readGPIO: _3";
                m_powerEvent.release();
                qDebug() << "readGPIO: _4";
            }
            qDebug() << "readGPIO: _5";
        }
        qDebug() << "readGPIO: _6";
    }
    qDebug() << "readGPIO: exit";
}

void TestPower::runTest()
{
    m_fd = open("/dev/input/event0", O_RDONLY);
    if (m_fd < 0)
    {
        m_reporter->testHasFailed("Unable to open /dev/input/event0.");
        return;
    }

    runTest0();

    runTest1();
    runTest2();
}

void TestPower::runTest0()
{
    TestModeWrapper test(m_controller);

    m_controller->sendTestSetIOCMD(m_channel, 0x00); // event value = 1
    QThread::yieldCurrentThread();
    // sleep(1);                   // TODO: This is necessary, but why?
    m_controller->sendTestSetIOCMD(m_channel, 0x01); // event value = 0
}

void TestPower::runTest1()
{
    TestModeWrapper test(m_controller);

    InputEventHandler evhandler(m_fd, 116);

    // pollfd ps[1];
    // ps[0].fd = m_fd;
    // ps[0].events = POLLIN;
    // bool gotit = false;
    // int ready = poll(ps, 1, 5000);
    // if (ready)
    // {
    //     input_event ev;
    //     int bytes = read(ps[0].fd, &ev, sizeof(input_event));
    //     if (bytes == sizeof(input_event))
    //     {
    //         qDebug() << "Once!"
    //                  << "code:" << ev.code
    //                  << "value:" << ev.value;
    //         gotit = ev.value && ev.code == KEY_POWER;
    //     }
    // }

    // 1
    m_powerEvent.release(m_powerEvent.available());

    if (not connect(
            &evhandler, SIGNAL(gotEvent(int)),
                    this, SLOT(signalSemaphore(int)))
        )
    {
        m_reporter->testHasFailed("Failed to connect.");
        m_reporter->logResult("Error,");
        return;
    }


    m_controller->sendTestSetIOCMD(m_channel, 0x00); // event value = 1
    sleep(1);
    m_controller->sendTestSetIOCMD(m_channel, 0x01); // event value = 0
    sleep(1);

    bool gotEvent = m_powerEvent.tryAcquire(1, 100);
    if (not gotEvent)
    {
        m_reporter->testHasFailed("Set GPIO2, read event timed out!");
    }
}

void TestPower::runTest2()
{
    TestModeWrapper test(m_controller);

    InputEventHandler evhandler(m_fd, 116);
    // 2
    m_controller->sendTestSetIOCMD(m_channel, 0x01); // event value = 0
    sleep(1);
    qDebug() << "\nPress the power-button (5sec timeout)";
    evhandler.disconnect(SIGNAL(gotEvent(int)));
    qDebug() << "~~ trace1";
    m_powerEvent.release(m_powerEvent.available());
    qDebug() << "~~ trace2";
    connect(&evhandler, SIGNAL(gotEvent(int)),
            this, SLOT(readGPIO(int)));
    qDebug() << "~~ trace3";
    if (not m_powerEvent.tryAcquire(1, m_timeout * 1000))
    {
        m_reporter->testHasFailed("GPIO2 not set when pressing the power button.");
    }
    qDebug() << "~~ end";
}
