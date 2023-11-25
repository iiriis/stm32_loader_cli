#include <QCoreApplication>
#include <QtSerialPort/QSerialPort>
#include <QHash>
#include <QThread>
#include <QString>

#include <loader_core.h>

#define SERIAL_PORT_WRITE_TIMEOUT 2000
#define SERIAL_PORT_READ_TIMEOUT 2000

/* The Serial Port instance*/
QSerialPort serialPort;

comm_status_t init_Serial_Port(QString portName);
comm_status_t write_Serial_Port(uint8_t* pData, uint64_t size);
comm_status_t read_Serial_Port(uint8_t* pData, uint64_t size);

    uint8_t buff[1024];

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    /* Create a QHash to map the flags and it's arguments */
    QHash<QString, QString> cli_flags;

    /* Iterate through the cli arguments and store the flag and values */
    for(int i=0; i<(argc-1); i++)
    {
        QString argv_temp = argv[i];

        /* checks for all the possible flags */
        if(argv_temp == "-p")
            cli_flags.insert("-p", QString(argv[i+1]));
    }

    /* enable serial port communication */
    init_Serial_Port(cli_flags.value("-p"));



    buff[0] = 0x7f;

    write_Serial_Port(buff, 1);
    QThread::msleep(10);
    read_Serial_Port(buff, 1);

    QString hexadecimal;
    hexadecimal.setNum(buff[0],16);
    qDebug() << "7f " + hexadecimal;

    buff[0] = 0x11;
    buff[1] = 0xee;

    write_Serial_Port(buff, 2);
    QThread::msleep(10);
    read_Serial_Port(buff, 1);

    hexadecimal.setNum(buff[0],16);
    qDebug() << "11 ee "+ hexadecimal;

    buff[0] = 0x08;
    buff[1] = 0x00;
    buff[2] = 0x00;
    buff[3] = 0x00;
    buff[4] = 0x08;

    write_Serial_Port(buff, 5);
    QThread::msleep(10);
    read_Serial_Port(buff, 1);

    hexadecimal.setNum(buff[0],16);
    qDebug() << "address "+ hexadecimal;

    buff[0] = 0xff;
    buff[1] = 0x00;

    write_Serial_Port(buff, 2);
    QThread::msleep(50);
    read_Serial_Port(buff, 256);

    for(int i=0;i<256;i++)
    {
        hexadecimal.setNum(buff[i],16);
        if(hexadecimal.length() > 1)
        qDebug() << hexadecimal;
        else
        qDebug() << "0"+hexadecimal;

    }


    serialPort.close();

    return 0;
}

/* Setup and Initialize the Serial Communication
 * Parameters
 * portName: the name of the Serial Port in the device
 *
 * Returns
 * COMM_SUCCESS: Upon establishing proper Serial communication
 * COMM_ERROR: Upon unsuccessful opening of Serial Port
 *
 */
comm_status_t init_Serial_Port(QString portName)
{
    /* setup and initialise the Serial port */
    serialPort.setPortName(portName);
    serialPort.setBaudRate(QSerialPort::Baud115200);
    serialPort.setDataBits(QSerialPort::Data8);
    serialPort.setParity(QSerialPort::NoParity);
    serialPort.setStopBits(QSerialPort::OneStop);
    serialPort.setFlowControl(QSerialPort::NoFlowControl);

    /* open the serial port in read and write mode */
    if (serialPort.open(QIODevice::ReadWrite)) {
        qDebug() << "Serial port opened successfully";
        return COMM_SUCCESS;
    }

    qDebug() << "Failed to open serial port";
    return COMM_INIT_ERROR;
}

/* Blocking function that writes a buffer to the Serial Port
 *
 * Parameters
 * pData: the buffer
 * size: the size of buffer
 *
 * Returns
 * COMM_SUCCESS: Upon successful write
 * COMM_WRITE_ERROR: Upon error during write/timeout
 *
 */
comm_status_t write_Serial_Port(uint8_t* pData, uint64_t size)
{
    for(uint64_t idx = 0; idx<size; idx++){

        /* write to the serial port and check the number of bytes written equals size */
        if (serialPort.write((const char*)&pData[idx], 1) != 1)
            return COMM_WRITE_ERROR;

        /* block the flow until the data byte is written or timeout happened */
        if(! serialPort.waitForBytesWritten(SERIAL_PORT_WRITE_TIMEOUT))
            return COMM_WRITE_ERROR;
    }

    return COMM_SUCCESS;
}

/* Blocking function that reads data to a buffer from the Serial Port
 *
 * Parameters
 * pData: the buffer to store
 * size: number of bytes to read from serialport
 *
 * Returns
 * COMM_SUCCESS: Upon successful read
 * COMM_READ_ERROR: Upon  error during read/timeout
 *
 */
comm_status_t read_Serial_Port(uint8_t* pData, uint64_t size)
{

    /* block the flow until the data arrived or timeout happened */
    if (! serialPort.waitForReadyRead(SERIAL_PORT_WRITE_TIMEOUT))
        return COMM_READ_ERROR;

    /* read from the serial port and check the number of bytes read equals size */
    if ( (uint64_t)serialPort.read((char *)pData, size) != size)
        return COMM_READ_ERROR;

    return COMM_SUCCESS;
}
