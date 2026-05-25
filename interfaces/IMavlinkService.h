#ifndef IMAVLINKSERVICE_H
#define IMAVLINKSERVICE_H

#include <QObject>
#include <QByteArray>
#include <QString>
#include <QVector>

class ILink;

/**
 * @brief 参数信息结构体
 */
struct ParamInfo {
    QString   id;          ///< 参数名称
    float     value = 0;   ///< 参数值
    int       type  = 0;   ///< MAVLink 参数类型
    int       index = 0;   ///< 参数索引
};

/**
 * @brief MAVLink 服务接口 —— 处理 MAVLink 协议解析与组包
 *
 * 从链路接收原始数据、解析 MAVLink 消息，并提供参数读写等高层功能。
 */
class IMavlinkService : public QObject
{
    Q_OBJECT

public:
    explicit IMavlinkService(QObject* parent = nullptr) : QObject(parent) {}
    virtual ~IMavlinkService() = default;

    /// 绑定数据链路（接收和发送都通过此链路）
    virtual void setLink(ILink* link) = 0;

    /// 将原始数据送入 MAVLink 解析器
    virtual void feedData(const QByteArray& data) = 0;

    /// 发送 MAVLink 消息（底层组包后通过链路发出）
    virtual bool sendMessage(const QByteArray& payload) = 0;

signals:
    /// 收到参数数据
    void paramReceived(const ParamInfo& param);

    /// 参数列表接收完毕
    void paramListComplete();
};

#endif // IMAVLINKSERVICE_H
