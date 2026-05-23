#ifndef PLUGINMANAGER_H
#define PLUGINMANAGER_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QDir>
#include <memory>

class IPlugin;
class CommunicationHub;

/**
 * @brief 插件管理器 —— 负责插件的发现、加载、卸载与生命周期管理
 *
 * 定位：
 *   这是地面站插件的"管家"。它使用 Qt 的 QPluginLoader 在运行时
 *   动态加载和卸载 .dll/.so 插件文件。
 *
 * 工作原理（底层原理）：
 *   QPluginLoader 底层调用平台原生动态库加载 API：
 *   - Windows: LoadLibrary / GetProcAddress
 *   - Linux:   dlopen / dlsym
 *   加载后通过 QPluginLoader::instance() 获取 QObject 根实例，
 *   再通过 qobject_cast<IPlugin*> 映射到我们的抽象接口。
 *
 * 线程安全：
 *   所有对 m_plugins 的写操作都在主线程（GUI 线程）中执行，
 *   Qt 的 QPluginLoader 本身不是线程安全的，必须主线程调用。
 */
class PluginManager : public QObject
{
    Q_OBJECT

public:
    /**
     * @param hub 通信中枢指针（不拥有所有权，生命周期由宿主 manage）
     * @param pluginDir 插件搜索目录（默认 "./plugins"）
     * @param parent QObject 父对象
     */
    explicit PluginManager(CommunicationHub* hub,
                           const QString& pluginDir = "./plugins",
                           QObject* parent = nullptr);
    ~PluginManager() override;

    /**
     * @brief 扫描插件目录，发现并加载所有有效插件
     *
     * 扫描逻辑：
     *   1. 遍历 pluginDir 下的所有 .dll / .so 文件
     *   2. 使用 QPluginLoader 尝试加载
     *   3. 检查是否实现了 IPlugin 接口
     *   4. 调用 plugin->initialize() 完成初始化
     *   5. 注册到 CommunicationHub
     *
     * @return 成功加载的插件数量
     */
    int loadAllPlugins();

    /**
     * @brief 加载单个插件文件
     * @param filePath 插件文件绝对路径
     * @return 成功返回插件接口指针（所有权归 PluginManager），失败返回 nullptr
     */
    IPlugin* loadPlugin(const QString& filePath);

    /**
     * @brief 卸载指定插件
     * @param pluginId 插件唯一标识
     * @return true 表示卸载成功
     */
    bool unloadPlugin(const QString& pluginId);

    /**
     * @brief 卸载所有已加载的插件（程序退出时调用）
     */
    void unloadAllPlugins();

    /**
     * @brief 获取已加载的插件 ID 列表
     */
    QStringList loadedPluginIds() const;

signals:
    /// 插件成功加载信号
    void pluginLoaded(const QString& pluginId);

    /// 插件即将卸载信号
    void pluginAboutToUnload(const QString& pluginId);

    /// 插件卸载完成信号
    void pluginUnloaded(const QString& pluginId);

private:
    CommunicationHub* m_hub;    ///< 通信中枢（不拥有所有权）

    QDir m_pluginDir;           ///< 插件搜索目录

    /// 内部记录：插件加载器 + 接口指针（一对一的拥有关系）
    struct LoadedPlugin {
        std::unique_ptr<QPluginLoader> loader;  ///< QPluginLoader 所有权
        IPlugin*                       ptr;     ///< 裸指针（loader 管理的 QObject 的接口视图）
    };
    QMap<QString, LoadedPlugin> m_plugins;      ///< key = pluginId
};

#endif // PLUGINMANAGER_H
