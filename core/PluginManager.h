#ifndef PLUGINMANAGER_H
#define PLUGINMANAGER_H

#include <QObject>
#include <QDir>
#include <QStringList>
#include <QMap>
#include <QPluginLoader>

class IPlugin;

/**
 * @brief 插件管理器 —— 扫描 plugins/ 目录并加载 Qt 插件
 *
 * 使用 QPluginLoader 运行时加载 .dll，通过 qobject_cast<IPlugin*> 映射接口。
 */
class PluginManager : public QObject
{
    Q_OBJECT

public:
    explicit PluginManager(const QString& pluginDir, QObject* parent = nullptr);
    ~PluginManager() override;

    /// 扫描目录并加载所有有效插件，返回成功数量
    int loadAll();

    /// 卸载指定插件
    bool unload(const QString& pluginId);

    /// 卸载全部插件
    void unloadAll();

    /// 已加载的插件 ID 列表
    QStringList loadedIds() const;

    /// 根据 ID 获取插件接口指针
    IPlugin* plugin(const QString& pluginId) const;

signals:
    void pluginLoaded(const QString& pluginId);
    void pluginUnloaded(const QString& pluginId);

private:
    IPlugin* loadOne(const QString& filePath);

    QDir m_pluginDir;

    struct Entry {
        QPluginLoader* loader = nullptr;
        IPlugin*       ptr    = nullptr;
    };
    QMap<QString, Entry> m_plugins;
};

#endif // PLUGINMANAGER_H
