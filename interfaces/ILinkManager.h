#ifndef ILINKMANAGER_H
#define ILINKMANAGER_H

#include <QObject>
#include <QString>
#include <QByteArray>
#include <QJsonObject>

/**
 * @brief 链路管理器接口 —— 统一管理多条通信链路
 *
 * 负责创建/销毁/连接/断开链路，并将链路数据统一转发。
 */
class ILinkManager : public QObject
{
    Q_OBJECT

public:
    explicit ILinkManager(QObject* parent = nullptr) : QObject(parent) {}
    virtual ~ILinkManager() = default;

    /// 创建一条链路（type: "serial" / "udp"），返回链路 ID（-1 表示失败）
    virtual int createLink(const QString& type, const QJsonObject& config) = 0;

    /// 销毁指定链路
    virtual bool destroyLink(int linkId) = 0;

    /// 打开链路连接
    virtual bool openLink(int linkId) = 0;

    /// 关闭链路连接
    virtual void closeLink(int linkId) = 0;

    /// 通过指定链路发送数据
    virtual qint64 sendData(int linkId, const QByteArray& data) = 0;

    /// 获取链路是否开启
    virtual bool isLinkOpen(int linkId) const = 0;

signals:
    /// 链路状态变化
    void linkStateChanged(int linkId, bool connected);

    /// 收到某条链路的数据
    void dataReceived(int linkId, const QByteArray& data);
};

#endif // ILINKMANAGER_H
