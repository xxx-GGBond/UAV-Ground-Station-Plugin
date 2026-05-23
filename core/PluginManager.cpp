#include "PluginManager.h"
#include "interfaces/IPlugin.h"

#include <QPluginLoader>
#include <QDebug>
#include <QFileInfo>

PluginManager::PluginManager(const QString& pluginDir, QObject* parent)
    : QObject(parent)
    , m_pluginDir(pluginDir)
{
    qDebug() << "[PluginManager] 初始化，搜索目录:" << m_pluginDir.absolutePath();
}

PluginManager::~PluginManager()
{
    unloadAll();
}

int PluginManager::loadAll()
{
    if (!m_pluginDir.exists()) {
        qWarning() << "[PluginManager] 插件目录不存在:" << m_pluginDir.absolutePath();
        return 0;
    }

    QStringList filters;
#ifdef Q_OS_WIN
    filters << "*.dll";
#else
    filters << "*.so";
#endif

    const QFileInfoList files = m_pluginDir.entryInfoList(filters, QDir::Files);
    int count = 0;
    for (const QFileInfo& info : files) {
        if (loadOne(info.absoluteFilePath())) {
            ++count;
        }
    }

    qDebug() << "[PluginManager] 扫描完成:" << count << "/" << files.size() << "个插件加载成功";
    return count;
}

IPlugin* PluginManager::loadOne(const QString& filePath)
{
    auto loader = QSharedPointer<QPluginLoader>::create(filePath);

    // 检查 IID 是否匹配
    if (loader->metaData().value("IID").toString() != IPLUGIN_IID) {
        qWarning() << "[PluginManager] IID 不匹配，跳过:" << filePath;
        return nullptr;
    }

    QObject* instance = loader->instance();
    if (!instance) {
        qWarning() << "[PluginManager] 加载失败:" << filePath
                     << "错误:" << loader->errorString();
        return nullptr;
    }

    IPlugin* p = qobject_cast<IPlugin*>(instance);
    if (!p) {
        qWarning() << "[PluginManager] 未实现 IPlugin:" << filePath;
        loader->unload();
        return nullptr;
    }

    if (!p->initialize()) {
        qWarning() << "[PluginManager] 插件初始化失败:" << p->pluginId();
        loader->unload();
        return nullptr;
    }

    Entry entry;
    entry.loader = loader;    // QSharedPointer 可复制，QMap 不再报错
    entry.ptr    = p;
    m_plugins.insert(p->pluginId(), entry);

    qDebug() << "[PluginManager] 加载成功:" << p->pluginId() << p->pluginName();
    emit pluginLoaded(p->pluginId());
    return p;
}

bool PluginManager::unload(const QString& pluginId)
{
    if (!m_plugins.contains(pluginId)) {
        return false;
    }

    Entry& e = m_plugins[pluginId];
    if (e.ptr) e.ptr->shutdown();
    if (e.loader) e.loader->unload();
    m_plugins.remove(pluginId);

    qDebug() << "[PluginManager] 已卸载:" << pluginId;
    emit pluginUnloaded(pluginId);
    return true;
}

void PluginManager::unloadAll()
{
    const QStringList ids = m_plugins.keys();
    for (const QString& id : ids) {
        unload(id);
    }
}

QStringList PluginManager::loadedIds() const
{
    return m_plugins.keys();
}

IPlugin* PluginManager::plugin(const QString& pluginId) const
{
    auto it = m_plugins.find(pluginId);
    return (it != m_plugins.end()) ? it.value().ptr : nullptr;
}
