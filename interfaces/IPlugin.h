#ifndef IPLUGIN_H
#define IPLUGIN_H

#include <QString>

/**
 * @brief 插件基础接口 —— 所有地面站插件必须实现
 *
 * 宿主通过此接口管理插件的生命周期，不依赖具体实现。
 */
class IPlugin
{
public:
    virtual ~IPlugin() = default;

    /// 插件初始化（加载后调用，返回 true 表示成功）
    virtual bool initialize() = 0;

    /// 插件反初始化（卸载前调用）
    virtual void shutdown() = 0;

    /// 插件唯一标识，如 "com.uav.parameter"
    virtual QString pluginId() const = 0;

    /// 插件显示名称，如 "参数插件"
    virtual QString pluginName() const = 0;
};

#define IPLUGIN_IID "com.uav.groundstation.IPlugin"

Q_DECLARE_INTERFACE(IPlugin, IPLUGIN_IID)

#endif // IPLUGIN_H
