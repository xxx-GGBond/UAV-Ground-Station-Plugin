#ifndef UDPLINK_H
#define UDPLINK_H

#include "interfaces/ILink.h"
#include <QUdpSocket>

/**
 * @brief UDP 链路 —— 通过 QUdpSocket 实现 UDP 通信
 */
class UdpLink : public ILink
{
    Q_OBJECT

public:
    explicit UdpLink(QObject* parent = nullptr);
    ~UdpLink() override;

    bool   open(const QJsonObject& config) override;
    void   close() override;
    bool   isOpen() const override;
    qint64 send(const QByteArray& data) override;

private slots:
    void onReadyRead();

private:
    QUdpSocket* m_socket  = nullptr;
    QString     m_host;
    quint16     m_port    = 0;
};

#endif // UDPLINK_H
