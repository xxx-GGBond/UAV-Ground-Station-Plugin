#ifndef MAVLINKSERVICE_H
#define MAVLINKSERVICE_H

#include "interfaces/IMavlinkService.h"

/**
 * @brief MAVLink 服务 —— MAVLink 协议处理
 *
 * 当前为占位实现（TODO），不依赖真实 MAVLink 头文件。
 * 后续集成 mavlink 库后替换 feedData/sendMessage 内部逻辑即可。
 */
class MavlinkService : public IMavlinkService
{
    Q_OBJECT

public:
    explicit MavlinkService(QObject* parent = nullptr);
    ~MavlinkService() override;

    void setLink(ILink* link) override;
    void feedData(const QByteArray& data) override;
    bool sendMessage(const QByteArray& payload) override;

private:
    ILink* m_link = nullptr;
};

#endif // MAVLINKSERVICE_H
