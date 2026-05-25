#ifndef IPARAMETERPROVIDER_H
#define IPARAMETERPROVIDER_H

#include <QObject>
#include <QString>
#include <QVariant>

/**
 * @brief 参数提供者接口 —— 插件通过此接口向外提供参数读写能力
 *
 * 典型调用链：
 *   UI → IParameterProvider::setParam()
 *       → (插件内部调用 IMavlinkService 发送 MAVLink 消息)
 *       → 飞控应答 → paramSetAck() 信号
 */
class IParameterProvider
{
public:
    virtual ~IParameterProvider() = default;

    /// 请求飞控发送全部参数
    virtual void requestList() = 0;

    /// 设置飞控参数
    virtual void setParam(const QString& id, const QVariant& value) = 0;

    /// 获取缓存的参数值
    virtual QVariant paramValue(const QString& id) const = 0;
};

#define IPARAMETERPROVIDER_IID "com.uav.groundstation.IParameterProvider"

Q_DECLARE_INTERFACE(IParameterProvider, IPARAMETERPROVIDER_IID)

#endif // IPARAMETERPROVIDER_H
