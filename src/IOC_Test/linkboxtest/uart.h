#ifndef UART_H
#define UART_H

#include <qextserialport.h>

enum UartMessages
{
	CMD_SYN					= 0x01,
	CMD_ACK					= 0x02,
	CMD_SYNACK				= 0x03,
	CMD_FINISH				= 0x04,

	CMD_FIRST_BYTE			= 0xAA,
	CMD_LAST_BYTE			= 0xDD,

	// Audio Quality Test
	CMD_AUDIO_START			= 0x11,
	CMD_AUDIO_START_ACK		= 0x12,
	CMD_AUDIO_STOP			= 0x13,
	CMD_AUDIO_STOP_ACK		= 0x14,

	// Pulse Detect Test
	CMD_PULSE_DETECT		= 0x20,

	// Pulse Drivers Test
	CMD_PULSE_DRIVE_10		= 0x31,
	CMD_PULSE_DRIVE_10_ACK	= 0x32,
	CMD_PULSE_DRIVE_50		= 0x33,
	CMD_PULSE_DRIVE_50_ACK	= 0x34,
	CMD_PULSE_DRIVE_OFF		= 0x35,
	CMD_PULSE_DRIVE_OFF_ACK	= 0x36,

	// Analog Input Test
	CMD_ANALOG_INPUT		= 0x40,
	CMD_ANALOG_INPUT1		= 0x41,
	CMD_ANALOG_INPUT2		= 0x42,
	CMD_ANALOG_INPUT3		= 0x43,
	CMD_ANALOG_INPUT4		= 0x44,
	CMD_ANALOG_INPUT5		= 0x45,
	CMD_ANALOG_INPUT6		= 0x46,

	// ECG / GPO Test
	CMD_ECG_GPO_ON			= 0x51,
	CMD_ECG_GPO_ON_ACK		= 0x52,
	CMD_ECG_GPO_OFF			= 0x53,
	CMD_ECG_GPO_OFF_ACK		= 0x54,
};



class Uart
{
private:

	bool _isConnected;

public:

	QextSerialPort *port;

	Uart();
	int connect();
	int disconnect(bool);
	bool isConnected(void);
	int send(int cmd);
	int receive();
	int write(const char *str);
	int write(int num);
};


#endif // UART_H
