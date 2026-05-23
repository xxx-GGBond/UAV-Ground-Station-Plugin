#ifndef IPLUGIN_H
#define IPLUGIN_H

#include <QObject>
#include <QString>

/**
 * @brief 插件基础接口 —— 所有地面站插件必须实现的抽象基类
 *
 * 宿主只通过此接口与插件交互。插件不包含主程序任何头文件，
 * 主程序不依赖具体插件实现，实现完全解耦。
 */
class IPlugin
{
public:
    virtual ~IPlugin() = default;

    /// 插件初始化（加载后调用）
    virtual bool initialize() = 0;

    /// 插件反初始化（卸载前调用）
    virtual void shutdown() = 0;

    /// 插件唯一标识（如 "com.uav.parameter"）
    virtual QString pluginId() const = 0;

    /// 插件名称（如 "参数读写插件"）
    virtual QString pluginName() const = 0;
};

#define IPLUGIN_IID "com.uav.groundstation.IPlugin/1.0"
Q_DECLARE_INTERFACE(IPlugin, IPLUGIN_IID)

#endif // IPLUGIN_H
