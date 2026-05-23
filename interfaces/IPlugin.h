#ifndef IPLUGIN_H
#define IPLUGIN_H

#include <QObject>
#include <QString>
#include <QJsonObject>

/**
 * @brief 插件基础接口 —— 所有地面站插件必须实现的抽象基类
 *
 * 设计意图：
 *   宿主程序（core/）只通过此接口与插件交互，不依赖任何具体插件实现。
 *   这保证了主程序与插件之间的"完全解耦"——主程序不知道插件的存在，
 *   插件不包含主程序的任何头文件，双方仅通过此纯虚接口通信。
 *
 * 底层原理：
 *   Qt 的插件系统基于 QPluginLoader，它利用 C++ 的 RTTI 和虚函数表
 *   实现运行时多态。只要插件实现了 IPlugin 接口，宿主即可通过
 *   qobject_cast<IPlugin*> 安全地将其映射到接口指针进行调用。
 */
class IPlugin
{
public:
    virtual ~IPlugin() = default;

    // ── 生命周期管理 ────────────────────────────────────────

    /**
     * @brief 插件初始化 —— 在插件被加载后调用，用于注册到通信中枢等
     * @return true 表示初始化成功，false 表示失败（宿主应卸载该插件）
     */
    virtual bool initialize() = 0;

    /**
     * @brief 插件反初始化 —— 在插件被卸载前调用，用于清理资源
     */
    virtual void shutdown() = 0;

    // ── 元信息 ──────────────────────────────────────────────

    /**
     * @brief 返回插件唯一标识符（如 "com.uav.telemetry"）
     */
    virtual QString pluginId() const = 0;

    /**
     * @brief 返回插件人类可读名称（如 "遥测数据显示"）
     */
    virtual QString pluginName() const = 0;

    /**
     * @brief 返回插件版本号（如 "1.0.0"）
     */
    virtual QString pluginVersion() const = 0;

    // ── 配置 ────────────────────────────────────────────────

    /**
     * @brief 获取插件当前配置（JSON 格式，用于持久化保存）
     */
    virtual QJsonObject getConfig() const = 0;

    /**
     * @brief 应用外部传入的配置（JSON 格式，用于恢复上次保存的状态）
     * @param config 配置数据
     * @return true 表示配置应用成功
     */
    virtual bool setConfig(const QJsonObject& config) = 0;
};

// ── Qt 插件元信息宏所需的接口标识符 ─────────────────
// 每个具体插件需要在 .cpp 中通过 Q_PLUGIN_METADATA 声明
// 并在类的顶部使用 Q_INTERFACES(IPlugin) 告知 Qt 元对象系统

#define IPLUGIN_IID "com.uav.groundstation.IPlugin/1.0"
Q_DECLARE_INTERFACE(IPlugin, IPLUGIN_IID)

#endif // IPLUGIN_H
