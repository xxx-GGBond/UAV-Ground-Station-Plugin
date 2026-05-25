#include "SerialLink.h"
#include <QDebug>

SerialLink::SerialLink(QObject* parent)
    : ILink(parent)
    , m_serial(new QSerialPort(this))
{
    connect(m_serial, &QSerialPort::readyRead, this, &SerialLink::onReadyRead);
    connect(m_serial, &QSerialPort::errorOccurred, this, &SerialLink::onError);
}

SerialLink::~SerialLink()
{
    close();
}

bool SerialLink::open(const QJsonObject& config)
{
    const QString port     = config.value("port").toString("COM1");
    const int     baudRate = config.value("baudRate").toInt(115200);

    m_serial->setPortName(port);
    m_serial->setBaudRate(baudRate);

    if (!m_serial->open(QIODevice::ReadWrite)) {
        qWarning() << "[SerialLink] 打开失败:" << port << m_serial->errorString();
        return false;
    }

    qDebug() << "[SerialLink] 已打开:" << port << baudRate;
    emit stateChanged(true);
    return true;
}

void SerialLink::close()
{
    if (m_serial && m_serial->isOpen()) {
        m_serial->close();
        emit stateChanged(false);
        qDebug() << "[SerialLink] 已关闭";
    }
}

bool SerialLink::isOpen() const
{
    return m_serial && m_serial->isOpen();
}

qint64 SerialLink::send(const QByteArray& data)
{
    if (!isOpen()) return -1;
    return m_serial->write(data);
}

void SerialLink::onReadyRead()
{
    const QByteArray data = m_serial->readAll();
    if (!data.isEmpty()) {
        emit dataReceived(data);
    }
}

void SerialLink::onError(QSerialPort::SerialPortError error)
{
    if (error != QSerialPort::NoError) {
        qWarning() << "[SerialLink] 错误:" << error << m_serial->errorString();
        emit stateChanged(false);
    }
}
