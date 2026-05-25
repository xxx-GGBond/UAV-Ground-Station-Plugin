#include "MavlinkService.h"
#include "interfaces/ILink.h"

#include <QDebug>

MavlinkService::MavlinkService(QObject* parent)
    : IMavlinkService(parent)
{
    qDebug() << "[MavlinkService] 已创建（占位实现）";
}

MavlinkService::~MavlinkService()
{
}

void MavlinkService::setLink(ILink* link)
{
    m_link = link;
    if (m_link) {
        // 连接链路数据信号，将原始数据送入 MAVLink 解析
        connect(m_link, &ILink::dataReceived, this, &MavlinkService::feedData,
                Qt::UniqueConnection);
    }
    qDebug() << "[MavlinkService] 链路已绑定";
}

void MavlinkService::feedData(const QByteArray& data)
{
    // TODO: 集成 mavlink 库后实现完整协议解析
    // mavlink_message_t msg;
    // mavlink_parse_char(MAVLINK_COMM_0, data[i], &msg, &status);

    qDebug() << "[MavlinkService] 收到数据:" << data.size() << "字节 (暂未解析)";
}

bool MavlinkService::sendMessage(const QByteArray& payload)
{
    if (!m_link || !m_link->isOpen()) {
        return false;
    }

    // TODO: 集成 mavlink 库后实现完整协议组包
    // mavlink_msg_xxx_pack(...);

    m_link->send(payload);
    return true;
}
