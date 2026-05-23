#ifndef COMMUNICATIONHUB_H
#define COMMUNICATIONHUB_H

#include <QObject>
#include <QMap>
#include <memory>

// 前置声明（不包含具体实现头文件，保持编译期解耦）
class IPlugin;
class ILinkManager;
class ITelemetryProvider;
class IMissionManager;

/**
 * @brief 通信中枢 —— 地面站所有模块间信号与槽路由的"唯一中介"
 *
 * 定位（核心设计决策）：
 *   这是整个地面站架构的"中央神经系统"。所有插件之间的通信
 *   不直接进行——插件 A 不持有插件 B 的指针，也不调用插件 B 的方法。
 *   而是通过 CommunicationHub 转发信号和槽：
 *
 *   PluginA --signal--> CommunicationHub --slot call--> PluginB
 *
 * 为什么这样设计？
 *   1. 插件间完全解耦：插件 A 不知道自己编译时不存在的东西
 *   2. 统一管理：所有跨模块通信可以在此记录日志、做权限校验
 *   3. 线程安全：可以在此统一处理跨线程的信号投递（Qt::QueuedConnection）
 *   4. 可扩展：新增插件时不需要修改已有插件代码，只需在此注册新的路由规则
 *
 * QObject 对象树的注意事项：
 *   CommunicationHub 作为 QObject 子类，传入 parent 后由 Qt 管理生命周期。
 *   但它持有的接口指针（plugin/manager provider）的生命周期不由它管理——
 *   那些对象由 PluginManager 或宿主 main() 统一创建和销毁。
 */
class CommunicationHub : public QObject
{
    Q_OBJECT

public:
    explicit CommunicationHub(QObject* parent = nullptr);
    ~CommunicationHub() override;

    // ── 模块注册（插件加载时由 PluginManager 调用） ──────────

    /**
     * @brief 注册一个插件实例到中枢
     * @param plugin 插件接口指针（所有权归 PluginManager，中枢不负责释放）
     *
     * 注册后，中枢会连接该插件的信号到对应的槽函数，
     * 实现插件间通信的路由。
     */
    void registerPlugin(IPlugin* plugin);

    /**
     * @brief 注销插件（插件卸载时调用）
     * @param pluginId 插件唯一标识
     */
    void unregisterPlugin(const QString& pluginId);

    /**
     * @brief 注册链路管理器（由宿主在启动时调用，全局唯一）
     */
    void setLinkManager(ILinkManager* manager);

    /**
     * @brief 注册遥测提供者（由宿主在启动时调用，全局唯一）
     */
    void setTelemetryProvider(ITelemetryProvider* provider);

    /**
     * @brief 注册任务管理器（由宿主在启动时调用，全局唯一）
     */
    void setMissionManager(IMissionManager* manager);

    // ── 查询 ────────────────────────────────────────────────

    /**
     * @brief 获取已注册的链路管理器（可能为 nullptr）
     */
    ILinkManager* linkManager() const;

    /**
     * @brief 获取已注册的遥测提供者（可能为 nullptr）
     */
    ITelemetryProvider* telemetryProvider() const;

    /**
     * @brief 获取已注册的任务管理器（可能为 nullptr）
     */
    IMissionManager* missionManager() const;

private:
    // 存储已注册插件的元信息（不拥有所有权）
    struct PluginEntry {
        IPlugin*    ptr     = nullptr;
        QString     pluginId;
    };
    QMap<QString, PluginEntry> m_plugins;           ///< 已注册插件映射表

    // 核心服务接口指针（不拥有所有权）
    ILinkManager*       m_linkManager       = nullptr;
    ITelemetryProvider* m_telemetryProvider = nullptr;
    IMissionManager*    m_missionManager    = nullptr;
};

#endif // COMMUNICATIONHUB_H
