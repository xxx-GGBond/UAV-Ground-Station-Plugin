#ifndef ILINK_H
#define ILINK_H

#include <QObject>
#include <QByteArray>
#include <QJsonObject>

/**
 * @brief 通信链路抽象接口
 *
 * 屏蔽串口/UDP/TCP 等底层差异，上层只通过此接口收发数据。
 */
class ILink : public QObject
{
    Q_OBJECT

public:
    explicit ILink(QObject* parent = nullptr) : QObject(parent) {}
    virtual ~ILink() = default;

    /// 打开链路（config 包含类型相关参数，如 port/baudrate 或 host/port）
    virtual bool open(const QJsonObject& config) = 0;

    /// 关闭链路
    virtual void close() = 0;

    /// 链路是否已打开
    virtual bool isOpen() const = 0;

    /// 发送原始数据，返回实际发送字节数
    virtual qint64 send(const QByteArray& data) = 0;

signals:
    /// 收到数据
    void dataReceived(const QByteArray& data);

    /// 连接状态变化
    void stateChanged(bool connected);
};

#endif // ILINK_H
