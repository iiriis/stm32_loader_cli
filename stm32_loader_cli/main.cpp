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
comm_status_t write_Serial_Port(uint8_t* pData, uint8_t size);
comm_status_t read_Serial_Port(uint8_t* pData, uint8_t size);


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

    uint8_t buff[10];

    buff[0] = 0x7f;

    for(int i=1;i<10;i++)
        buff[i]=0;

    write_Serial_Port(buff, 1);
    QThread::msleep(100);
        read_Serial_Port(buff, 10);


    for(int i:buff)
    {
       QString hexadecimal;
        hexadecimal.setNum(i,16);

    qDebug() << hexadecimal;
    }



    return a.exec();
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
comm_status_t write_Serial_Port(uint8_t* pData, uint8_t size)
{
    /* write to the serial port and check the number of bytes written equals size */
    if (serialPort.write((const char*)pData, size) != size)
         return COMM_WRITE_ERROR;

    /* block the flow until the data bytes are written or timeout happened */
    if(! serialPort.waitForBytesWritten(SERIAL_PORT_WRITE_TIMEOUT))
         return COMM_WRITE_ERROR;

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
comm_status_t read_Serial_Port(uint8_t* pData, uint8_t size)
{

    /* block the flow until the data arrived or timeout happened */
    if (! serialPort.waitForReadyRead(SERIAL_PORT_WRITE_TIMEOUT))
        return COMM_READ_ERROR;

    /* read from the serial port and check the number of bytes read equals size */
    if ( serialPort.read((char *)pData, size) != size)
        return COMM_READ_ERROR;

    return COMM_SUCCESS;
}
