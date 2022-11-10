#include "iocontrollerupdatethread.h"

//#include "deviceconfiguration.h"

#include <QJsonValue>
#include <iostream>
#include <QFile>
#include <QDebug>
#include <QTimer>

#include <QLoggingCategory>
Q_LOGGING_CATEGORY(DBG_IOCFLASH_UPDATE_THREAD,"IOCFlash.UpdateThread", QtInfoMsg)

IoControllerUpdateThread::IoControllerUpdateThread()
    : m_RetryCounter(0)
    , m_iocGPIOmcuBoot0{"mcu_boot0", false}
    , m_iocGPIOmcuBoot1{"mcu_boot1", false}
    , m_iocGPIOmcuReset{"mcu_reset", true}
{
    m_FlashData_idx = 0;

    m_ReceiveStatus = eMessageSyncronizing;
    m_BootCMDpending = eNoCMD;

    //Create a timer for controlling the update state machine
    m_IOcontrUpdateTimer = new QTimer(this);
    connect(m_IOcontrUpdateTimer, &QTimer::timeout, this, &IoControllerUpdateThread::IOcontrUpdateProc);
}

IoControllerUpdateThread::~IoControllerUpdateThread()
{
    //Cleanup
    delete(m_IOcontrUpdateTimer);
    while(!m_FlashData.isEmpty())
    {
        FlashData_t *flashData = m_FlashData.at(0);
        delete flashData; //Cleanup
        m_FlashData.removeFirst();
    }
}

void IoControllerUpdateThread::run()
{

}


void IoControllerUpdateThread::IOcontrUpdateProc(void)
{
    //Timer event is used to control the update progress. When a responce from the bootloader is recieved, the
    //timer is set to fire immedeate. If no responce, the timer is preset to fire
    //in a sufficient amount of time, thus working as a timeout.

    m_IOcontrUpdateTimer->stop();

    if(m_ReceiveStatus != eMessageReceived &&
       m_IOcontrUpdateStatus != eBootEnter)
    {
        qCInfo(DBG_IOCFLASH_UPDATE_THREAD) << "Error! IOController did not respond";
        if (4 > m_RetryCounter++)
        {
            enterBoot();
            m_IOcontrUpdateTimer->start(2000);
            qCInfo(DBG_IOCFLASH_UPDATE_THREAD) << "Retrying...";
            return;
        }
        m_RetryCounter=0;
        m_error = eErrorFlashFailed;
        terminateBoot();
    }
    else
    {
        qCDebug(DBG_IOCFLASH_UPDATE_THREAD) << "IOcontrUpdateStatus:" << m_IOcontrUpdateStatus;
        switch(m_IOcontrUpdateStatus)
        {
            case eBootEnter:
                m_IOcontrUpdateStatus = eBootGetCommands;
                m_error = eNoError;
                enterBoot();
                m_IOcontrUpdateTimer->start(1000);
                break;
            case eBootGetCommands:
                m_IOcontrUpdateStatus = eBootEraseCMD;
                sendCMD(eGet);
                m_IOcontrUpdateTimer->start(1000);
                qCInfo(DBG_IOCFLASH_UPDATE_THREAD) << "Get commands";
                break;
            case eBootEraseCMD:
                m_IOcontrUpdateStatus = eBootEraseData;
                sendCMD(eErase);
                m_IOcontrUpdateTimer->start(1000);
                break;
            case eBootEraseData:
                {
                    QByteArray data;
                    m_IOcontrUpdateStatus = eBootFlashCMD;
                    m_FlashData_idx = 0;
                    qCInfo(DBG_IOCFLASH_UPDATE_THREAD) << "Erasing flash";
                    data.append(static_cast<char>(0xff));   //Erase all
                    sendData(data);
                    m_IOcontrUpdateTimer->start(10000);   //Erasing takes some time
                    qCInfo(DBG_IOCFLASH_UPDATE_THREAD) << "Programming flash...";
                }
                break;
            case eBootFlashCMD:
                if(m_FlashData_idx < m_FlashData.count())
                {
                    m_IOcontrUpdateStatus = eBootFlashAddr;
                    sendCMD(eWriteMem);
                    m_IOcontrUpdateTimer->start(5000);
                }
                else
                {
                    m_IOcontrUpdateStatus = eBootExit;
                    m_IOcontrUpdateTimer->start(1000);
                }
                break;
            case eBootFlashAddr:
                if(m_FlashData_idx < m_FlashData.count())
                {
                    m_IOcontrUpdateStatus = eBootFlashData;
                    sendData(m_FlashData.at(m_FlashData_idx)->m_startAddress);
                    m_IOcontrUpdateTimer->start(5000);
#if 0
                    quint32 addr;
                    addr = ((m_FlashData.at(m_FlashData_idx)->m_startAddress.at(0)) << 24) & 0xff000000u;
                    addr += ((m_FlashData.at(m_FlashData_idx)->m_startAddress.at(1)) << 16) & 0x00ff0000u;
                    addr += ((m_FlashData.at(m_FlashData_idx)->m_startAddress.at(2)) << 8) & 0x0000ff00u;
                    addr += (m_FlashData.at(m_FlashData_idx)->m_startAddress.at(1)) & 0x000000ffu;
                    qDebug("IoControllerUpdateThread::IOcontrUpdateProc - Writing address %x", addr);
#endif
                }
                else
                {
                    m_IOcontrUpdateStatus = eBootExit;
                    m_IOcontrUpdateTimer->start(1000);
                }

                break;
            case eBootFlashData:
                if(m_FlashData_idx < m_FlashData.count())
                {
                    m_IOcontrUpdateStatus = eBootFlashCMD;
                    FlashData_t *flashData = m_FlashData.at(m_FlashData_idx);
                    flashData->m_data.insert(0, FLASH_MEM_WR_BLOCK_SIZE - 1); //No of databytes to send
                    sendData(flashData->m_data);
                    m_FlashData_idx++;
                    m_IOcontrUpdateTimer->start(5000);
                }
                else
                {
                    m_IOcontrUpdateStatus = eBootExit;
                    m_IOcontrUpdateTimer->start(5000);
                    //End prog
                }

                break;
            case eBootExit:
                exitBoot();

                break;
            default:
                terminateBoot();
                break;
        }
    }
}



void IoControllerUpdateThread::receivedData(void)
{
    QByteArray data = m_SerialPort->readAll();
    qint32 dataIdx = 0;
    static qint32 noBytesLeftOfResponce = 0xffff;
    qint32 bytesRemaining = data.size();

    if(m_ReceiveStatus == eMessageSyncronizing)
    {
        //Sync
        do
        {
            if(data.at(dataIdx) == BOOT_NACK)
            {
                qCDebug(DBG_IOCFLASH_UPDATE_THREAD) <<  "Received NACK on CMD:" << m_BootCMDpending;

                m_ReceiveStatus = eMessageError;
                return;
            }
            if(data.at(dataIdx++) == BOOT_ACK)
            {
                m_ReceiveStatus = eMessageReceiving;
                noBytesLeftOfResponce = 0xffff; //Indicate that next byte to read is responce length
            }
            bytesRemaining--;
        } while(bytesRemaining && m_ReceiveStatus == eMessageSyncronizing);
    }

    if(m_ReceiveStatus == eMessageReceiving)
    {
        switch(m_BootCMDpending)
        {
            case eGet:
            {
                if(bytesRemaining)
                {
                    if(noBytesLeftOfResponce == 0xffff)
                    {
                        noBytesLeftOfResponce = data.at(dataIdx++) + 1;

                        bytesRemaining--;
                        m_IOcontrBootloaderCommandSet.clear();

                    }
                    while(bytesRemaining && noBytesLeftOfResponce)
                    {
                        m_IOcontrBootloaderCommandSet.append(static_cast<quint8>(data.at(dataIdx++)));
                        bytesRemaining--;
                        noBytesLeftOfResponce--;
                    }
                    if(noBytesLeftOfResponce == 0 && bytesRemaining) //Look for ACK
                    {
                        if(data.at(dataIdx) == BOOT_ACK)
                        {
                            m_ReceiveStatus = eMessageReceived;
                        }
                        else
                        {
                            m_ReceiveStatus = eMessageError;
                        }
                    }
                }

                break;
            }
            case eGetVerAndReadProtStat:
                break;
            case eGetID:
                break;
            case eReadMem:
                break;
            case eGo:
                break;
            case eWriteMem:
                m_ReceiveStatus = eMessageReceived;
                break;
            case eErase:
                m_ReceiveStatus = eMessageReceived;
                break;
            case eExtErase:
                break;
            case eWriteProt:
                break;
            case eWriteUnprot:
                break;
            case eReadoutProt:
                break;
            case eReadoutUnprot:
                break;
            case eAutoBaudSig:
                m_ReceiveStatus = eMessageReceived;
                break;
            case eNoCMD:
                break;
            default:
                return;
        }
    }
    if(m_ReceiveStatus == eMessageReceived)
    {
        m_IOcontrUpdateTimer->start(1);   //Fire update timer immedate if recieved message
    }
}

void IoControllerUpdateThread::sendCMD(BootCMD_t a_CMD)
{
    QByteArray byteArray;

    //Check if command is supported. Note: "Get" command is always supported.
    if(a_CMD != eGet && a_CMD != eAutoBaudSig)
    {
        bool commandAllowed = false;
        for(quint8 cmd : m_IOcontrBootloaderCommandSet)
        {
            if(static_cast<BootCMD_t>(cmd) == a_CMD)
            {
                commandAllowed = true;
            }
        }
        if(!commandAllowed)
        {
            qCInfo(DBG_IOCFLASH_UPDATE_THREAD) << "Command not supported by IO controller:" <<a_CMD;
        }
    }

    byteArray.append(a_CMD);

    if(a_CMD != eAutoBaudSig)
    {
        byteArray.append(~a_CMD);
    }

    m_ReceiveStatus = eMessageSyncronizing;
    m_BootCMDpending = a_CMD;

    int bytesSent = m_SerialPort->write(byteArray);
    if (bytesSent != byteArray.size())
    {
        qCWarning(DBG_IOCFLASH_UPDATE_THREAD) << "Failed sending command" << a_CMD;
    }
}

void IoControllerUpdateThread::sendData(QByteArray a_bytesToSend)
{
    char chSum = 0;
    QByteArray byteArray;

    for(quint8 byte : a_bytesToSend)
    {
        byteArray.append(byte);
        chSum ^= byte;
    }

    if(m_BootCMDpending == eErase && ((unsigned char) a_bytesToSend.at(0)) == 0xff)
    {
        chSum = 0;
    }

    byteArray.append(chSum);

    m_ReceiveStatus = eMessageSyncronizing;

    int bytesSent = m_SerialPort->write(byteArray);
    if (bytesSent != byteArray.size())
    {
        //emit error
    }
}

void IoControllerUpdateThread::setBootMode(IOCtrlBootMode_t a_BootMode)
{
    qCDebug(DBG_IOCFLASH_UPDATE_THREAD) << "BootMode:" << a_BootMode;

    switch( a_BootMode )
    {
    case IOCTRLBOOT_NORMAL:
        m_iocGPIOmcuBoot0.set(false);
        m_iocGPIOmcuBoot1.set(false);
        break;
    case IOCTRLBOOT_REPROGRAM:
        m_iocGPIOmcuBoot0.set(true);
        m_iocGPIOmcuBoot1.set(false);
        break;
    }
}

void IoControllerUpdateThread::ioControllerReset(bool a_Reset)
{
    m_iocGPIOmcuReset.set(a_Reset);
}

void IoControllerUpdateThread::enterBoot(void)
{
    qCInfo(DBG_IOCFLASH_UPDATE_THREAD) << "Restart IOProc in boot mode";

    //Restart IOProc in boot mode
    setBootMode(IOCTRLBOOT_REPROGRAM);
    msleep(5);
    ioControllerReset(true);
    msleep(5);
    ioControllerReset(false);
    msleep(500);

    qCInfo(DBG_IOCFLASH_UPDATE_THREAD, "Enter boot");

    sendCMD(eAutoBaudSig);
}

void IoControllerUpdateThread::exitBoot(void)
{
    qCInfo(DBG_IOCFLASH_UPDATE_THREAD) << "Exit boot";

    //Restart IOProc in user prog mode
    setBootMode(IOCTRLBOOT_NORMAL);
    msleep(5);
    ioControllerReset(true);
    msleep(5);
    ioControllerReset(false);
    msleep(500);

    m_IOcontrUpdateTimer->stop();
    m_SerialPort->flush();
    m_SerialPort->close();

    disconnect(m_SerialPort, &QIODevice::readyRead, this, &IoControllerUpdateThread::receivedData);
    disconnect(m_IOcontrUpdateTimer, &QTimer::timeout, this, &IoControllerUpdateThread::IOcontrUpdateProc);

    if(m_error)
    {
        emit updateFinished(false);
    }
    else
    {
        emit updateFinished(true);
    }
}


void IoControllerUpdateThread::terminateBoot(void)
{
    qCInfo(DBG_IOCFLASH_UPDATE_THREAD);
    exitBoot();

}

bool IoControllerUpdateThread::configureSerial(QString a_SerialPort, BaudRateType a_BaudRate)
{
    if (0 < a_SerialPort.length())
    {
        m_SerialPort = new QextSerialPort(a_SerialPort, QextSerialPort::EventDriven );
        m_SerialPort->setBaudRate(a_BaudRate);
        m_SerialPort->setFlowControl(FLOW_OFF);
        m_SerialPort->setParity(PAR_EVEN);
        m_SerialPort->setDataBits(DATA_8);
        m_SerialPort->setStopBits(STOP_1);
        if(!m_SerialPort->open(QIODevice::ReadWrite))
        {
            qCWarning(DBG_IOCFLASH_UPDATE_THREAD) << "Unable to open serial port:" << qPrintable(m_SerialPort->errorString());
            return false;
        }
        return true;
    }

    qCWarning(DBG_IOCFLASH_UPDATE_THREAD) <<  "No COM port defined";
    return false;
}

void IoControllerUpdateThread::updateIOcontroller(QString a_SerialPort, QByteArray a_SimFileData)
{
    if(configureSerial(a_SerialPort, BAUD115200))
    {
        m_SerialPort->flush();

        connect(m_SerialPort, &QIODevice::readyRead, this, &IoControllerUpdateThread::receivedData);

        m_IOcontrUpdateStatus = eBootEnter;

        if(!SimpleCodeProcessFile(a_SimFileData))
        {
            emit updateFinished(false);
        }
        else
        {
            m_IOcontrUpdateTimer->start(1000);
        }
    }
    else
    {
        emit updateFinished(false);
    }
}

bool IoControllerUpdateThread::SimpleCodeProcessFile(QByteArray a_FileData)
{
    quint32 drec_start;
    quint32 drec_bytes;
    qint16 record_tag;
    qint16 record_number = 0;
    quint32 curr_addr;
    quint32 i; // Also used for checksum calculation.
    quint8 byte;
    quint8 proceed;
    qint8 record_error = 0;
    QByteArray simFileData = a_FileData;
    qint32 simFileDataIdx = 0;
    FlashData_t *flashData;
    qint32 flashDataIdx = 0;

    m_checksum=0;
    m_error = eNoError;

    // Read file header.
    if (SimFileReadUint(simFileData, &simFileDataIdx, 4) != 0x7f494152) // magic
    {
        qCInfo(DBG_IOCFLASH_UPDATE_THREAD) << "ERR_SIM_BAD_FORMAT";
        m_error = eErrorSimFile;
        return false;
    }

    SimFileReadUint(simFileData, &simFileDataIdx, 4); // flags (not used)
    SimFileReadUint(simFileData, &simFileDataIdx, 4); // hdr_bytes (not used in tiny mode)
    SimFileReadUint(simFileData, &simFileDataIdx, 2); // version (not used)

    // Loop over all records.
    for (proceed = 1; proceed;)
    {

        // Read record tag.
        record_tag = SimFileReadUint(simFileData, &simFileDataIdx, 1);
        switch (record_tag)
        {

            case 1: // Data record.

                SimFileReadUint(simFileData, &simFileDataIdx, 1); // segtype (not used)
                SimFileReadUint(simFileData, &simFileDataIdx, 2); // flags (not used)
                drec_start = SimFileReadUint(simFileData, &simFileDataIdx, 4);
                drec_bytes = SimFileReadUint(simFileData, &simFileDataIdx, 4);

                curr_addr = drec_start;



                // Loop over all data bytes in the record.
                for (i = 0; i < drec_bytes; i++)
                {
                    if(flashDataIdx % FLASH_MEM_WR_BLOCK_SIZE == 0)
                    {
                        if(flashDataIdx > 0)
                        {
                            m_FlashData.append(flashData);
                        }

                        flashData = new FlashData_t;
                        flashData->m_startAddress.append((curr_addr & 0xff000000u) >> 24);
                        flashData->m_startAddress.append((curr_addr & 0xff0000u) >> 16);
                        flashData->m_startAddress.append((curr_addr & 0xff00u) >> 8);
                        flashData->m_startAddress.append(curr_addr & 0xff);

                    }

                    byte = SimFileReadUint(simFileData, &simFileDataIdx, 1);
                    flashData->m_data.append(byte);
                    flashDataIdx++;
                    curr_addr++;
                    //bytes_written++;
                }
                record_number++;
                break;
            case 2: // Entry record (not used in flash loader).
                SimFileReadUint(simFileData, &simFileDataIdx, 4);
                SimFileReadUint(simFileData, &simFileDataIdx, 1);
                break;
            case 3: // End record.
                // The checksum itself is not part of the calculated checsum.
                i = m_checksum; // Save calculated checksum before reading checksum from file.
                m_checksum = SimFileReadUint(simFileData, &simFileDataIdx, 4);;
                m_checksum += i; // Should be zero, verified below.
                proceed = 0;
                break;
            default:
                record_error = 1;
                break;
        }
    }

    quint16 currentBytes = flashDataIdx % FLASH_MEM_WR_BLOCK_SIZE;
    if(currentBytes)
    {
        for(; currentBytes < FLASH_MEM_WR_BLOCK_SIZE; currentBytes++)
        {
            flashData->m_data.append(0xff); //Fill the flash data buffer block
        }

        m_FlashData.append(flashData);
    }


    if (record_error)
    {
        m_error = eErrorSimFileBadRecord;
    }

    if (m_checksum)
    {
        qCWarning(DBG_IOCFLASH_UPDATE_THREAD) << "ERR_SIM_CHECKSUM";
        m_error = eErrorSimFileChSum;
    }

    if(m_error)
    {
        qCWarning(DBG_IOCFLASH_UPDATE_THREAD) << m_error;
        return false;
    }

    return true;
}

quint8 IoControllerUpdateThread::SimFileReadByte(QByteArray a_SimFile, qint32 *a_pCurrIdx)
{
    quint8 c;

    if(a_SimFile.isEmpty() || *a_pCurrIdx >= a_SimFile.length())
    {
        m_error = eErrorSimFile;
        return 0;
    }

    c = a_SimFile.at(*a_pCurrIdx);
    (*a_pCurrIdx)++;

    return c;
}

quint32 IoControllerUpdateThread::SimFileReadUint(QByteArray a_SimFile, qint32 *a_pCurrIdx, qint16 a_Size)
{
    qint16 i;
    quint8 c;
    quint32 value = 0;

    // Loop over all bytes to be read and shift them in.
    for (i = 0; i < a_Size; i++)
    {
        if(*a_pCurrIdx >= a_SimFile.length())
        {
            m_error = eErrorSimFile;
            return 0;
        }

        c = SimFileReadByte(a_SimFile, a_pCurrIdx);
        value <<= 8;
        value |= c;
        m_checksum += c; // Calculate checksum.
    }
    return value;
}

