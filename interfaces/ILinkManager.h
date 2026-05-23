#ifndef ILINKMANAGER_H
#define ILINKMANAGER_H

#include <QObject>
#include <QString>
#include <QByteArray>
#include <QJsonObject>

/**
 * @brief 链路管理接口 —— 抽象所有物理通信链路
 *
 * 无论底层是串口、UDP 还是 TCP，上层只通过此接口收发数据。
 * 参数读写、心跳等所有 MAVLink 通信都经由链路收发原始字节。
 */
class ILinkManager : public QObject
{
    Q_OBJECT

public:
    explicit ILinkManager(QObject* parent = nullptr) : QObject(parent) {}
    virtual ~ILinkManager() = default;

    /// 打开链路（config 包含 port/baudrate 或 host/port 等参数）
    virtual bool open(const QJsonObject& config) = 0;

    /// 关闭链路
    virtual void close() = 0;

    /// 是否已连接
    virtual bool isOpen() const = 0;

    /// 发送原始字节
    virtual int64_t send(const QByteArray& data) = 0;

signals:
    /// 收到数据时发出（在非 UI 线程中发出，接收方需注意线程安全）
    void dataReceived(const QByteArray& data);

    /// 链路状态变化
    void stateChanged(bool connected);
};

#endif // ILINKMANAGER_H
