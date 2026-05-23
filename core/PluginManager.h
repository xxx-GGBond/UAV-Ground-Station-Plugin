#ifndef PLUGINMANAGER_H
#define PLUGINMANAGER_H

#include <QObject>
#include <QDir>
#include <QStringList>
#include <QMap>
#include <QPluginLoader>
#include <QSharedPointer>

class IPlugin;

/**
 * @brief 插件管理器 —— 负责插件的发现、加载、卸载
 *
 * 使用 Qt 的 QPluginLoader 在运行时动态加载 .dll/.so 插件文件，
 * 通过 qobject_cast<IPlugin*> 映射到抽象接口。
 *
 * 线程安全：所有操作必须在主线程中执行。
 */
class PluginManager : public QObject
{
    Q_OBJECT

public:
    /**
     * @param pluginDir 插件搜索目录（默认 "./plugins"）
     */
    explicit PluginManager(const QString& pluginDir = "./plugins",
                           QObject* parent = nullptr);
    ~PluginManager() override;

    /// 扫描插件目录并加载所有有效插件，返回成功加载的数量
    int loadAll();

    /// 加载单个插件文件，成功返回接口指针（所有权归 PluginManager）
    IPlugin* loadOne(const QString& filePath);

    /// 卸载指定插件
    bool unload(const QString& pluginId);

    /// 卸载全部插件
    void unloadAll();

    /// 获取已加载的插件 ID 列表
    QStringList loadedIds() const;

    /// 通过插件 ID 获取接口指针（未加载返回 nullptr）
    IPlugin* plugin(const QString& pluginId) const;

signals:
    void pluginLoaded(const QString& pluginId);
    void pluginUnloaded(const QString& pluginId);

private:
    QDir m_pluginDir;

    struct Entry {
        QSharedPointer<QPluginLoader> loader;   ///< 共享指针 —— QMap 要求值类型可复制
        IPlugin* ptr = nullptr;
    };
    QMap<QString, Entry> m_plugins;
};

#endif // PLUGINMANAGER_H
