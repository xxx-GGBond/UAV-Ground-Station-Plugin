#include "UdpLink.h"
#include <QDebug>
#include <QNetworkDatagram>

UdpLink::UdpLink(QObject* parent)
    : ILink(parent)
    , m_socket(new QUdpSocket(this))
{
    connect(m_socket, &QUdpSocket::readyRead, this, &UdpLink::onReadyRead);
}

UdpLink::~UdpLink()
{
    close();
}

bool UdpLink::open(const QJsonObject& config)
{
    m_host = config.value("host").toString("127.0.0.1");
    m_port = static_cast<quint16>(config.value("port").toInt(14550));

    // 绑定本地端口（用于接收）
    if (!m_socket->bind(QHostAddress::AnyIPv4, m_port)) {
        qWarning() << "[UdpLink] 绑定失败:" << m_socket->errorString();
        return false;
    }

    qDebug() << "[UdpLink] 已绑定:" << m_host << m_port;
    emit stateChanged(true);
    return true;
}

void UdpLink::close()
{
    m_socket->close();
    emit stateChanged(false);
    qDebug() << "[UdpLink] 已关闭";
}

bool UdpLink::isOpen() const
{
    return m_socket->state() == QAbstractSocket::BoundState;
}

qint64 UdpLink::send(const QByteArray& data)
{
    if (!isOpen()) return -1;
    return m_socket->writeDatagram(data, QHostAddress(m_host), m_port);
}

void UdpLink::onReadyRead()
{
    while (m_socket->hasPendingDatagrams()) {
        QNetworkDatagram datagram = m_socket->receiveDatagram();
        emit dataReceived(datagram.data());
    }
}
