#include "gpiotopower.h"

#include <QDebug>
#include <fcntl.h>
#include <unistd.h>
#include "inputeventhandler.h"

GpioToPower::GpioToPower(int &argc, char *argv[])
    : QCoreApplication(argc, argv)
    , m_returnCode(0)
    , m_count(0)
    , m_inputs(0)
    , m_event(0)
    , m_waitingForEvent(false)
{
    m_fd = open("/dev/input/event0", O_RDONLY);
    if (m_fd < 0)
    {
        qDebug() << "Unable to open event stream.";
        return;
    }

    m_ioc = new IOCtrlCommController("/dev/ttymxc3");
    m_ioc->sendTestModeCMD(1);  // on

    m_inputs = new InputEventHandler(m_fd, 116, this);
    connect(this, SIGNAL(go()),
            this, SLOT(goImpl()),
            Qt::QueuedConnection);
    emit go();
}

GpioToPower::~GpioToPower()
{
    m_ioc->sendTestModeCMD(2);  // off
    if (m_fd >= 0)
    {
        close(m_fd);
        delete m_inputs;
    }
}

void GpioToPower::setReturnCode(int returnCode)
{
    m_returnCode = returnCode;
}

void GpioToPower::quitApplication()
{
    exit(m_returnCode);
}

// Main logic

void GpioToPower::goImpl()
{
    // external connections
    connect(m_inputs, SIGNAL(gotEvent(int)),
            this, SLOT(onEventSignalOk(int)));

    // internal connections
    connect(this, SIGNAL(success()),
            this, SLOT(successImpl()));

    QTimer::singleShot(1000, this, SLOT(timeout()));

    m_ioc->sendTestSetIOCMD(3, 0x00); // on
}

void GpioToPower::successImpl()
{
    qDebug() << "SUCCESS, got power event.";
    m_waitingForEvent = false;
    setReturnCode(0);
    QTimer::singleShot(100, this, SLOT(quitApplication()));
}

void GpioToPower::timeout()
{
    qDebug() << "FAILED, reponse timed out.";
    m_waitingForEvent = false;
    m_ioc->sendTestSetIOCMD(3, 0x01); // off
    setReturnCode(1);
    QTimer::singleShot(100, this, SLOT(quitApplication()));
}

void GpioToPower::onEventSignalOk(int value)
{
    if (value == 1)
    {
        m_waitingForEvent = true;
        m_ioc->sendTestSetIOCMD(3, 0x01); // off
        emit success();
    }
}
