#include "PluginManager.h"
#include "interfaces/IPlugin.h"

#include <QDebug>
#include <QFileInfo>

PluginManager::PluginManager(const QString& pluginDir, QObject* parent)
    : QObject(parent)
    , m_pluginDir(pluginDir)
{
    qDebug() << "[PluginManager] 初始化，绝对路径:" << m_pluginDir.absolutePath();
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
    qDebug() << "[PluginManager] 扫描到" << files.size() << "个候选文件";

    int count = 0;
    for (const QFileInfo& info : files) {
        qDebug() << "[PluginManager] 尝试加载:" << info.absoluteFilePath();
        if (loadOne(info.absoluteFilePath())) {
            ++count;
        }
    }
    qDebug() << "[PluginManager] 扫描完成，成功:" << count << "/" << files.size();
    return count;
}

IPlugin* PluginManager::loadOne(const QString& filePath)
{
    auto* loader = new QPluginLoader(filePath, this);

    // 先检查 loader 自身的元数据
    const QJsonObject meta = loader->metaData();
    const QString iid = meta.value("IID").toString();
    qDebug() << "[PluginManager]   插件 IID:" << iid << "  期望:" << IPLUGIN_IID;

    if (iid != IPLUGIN_IID) {
        qWarning() << "[PluginManager]   IID 不匹配，跳过";
        delete loader;
        return nullptr;
    }

    QObject* instance = loader->instance();
    if (!instance) {
        qWarning() << "[PluginManager]   加载失败:" << loader->errorString();
        delete loader;
        return nullptr;
    }

    IPlugin* p = qobject_cast<IPlugin*>(instance);
    if (!p) {
        qWarning() << "[PluginManager]   qobject_cast<IPlugin*> 失败";
        loader->unload();
        delete loader;
        return nullptr;
    }

    if (!p->initialize()) {
        qWarning() << "[PluginManager]   插件 initialize() 返回 false";
        loader->unload();
        delete loader;
        return nullptr;
    }

    Entry entry;
    entry.loader = loader;
    entry.ptr    = p;
    m_plugins.insert(p->pluginId(), entry);

    qDebug() << "[PluginManager]   ✅ 成功:" << p->pluginId() << p->pluginName();
    emit pluginLoaded(p->pluginId());
    return p;
}

bool PluginManager::unload(const QString& pluginId)
{
    if (!m_plugins.contains(pluginId)) return false;

    Entry& e = m_plugins[pluginId];

    emit pluginUnloaded(pluginId);

    if (e.ptr) e.ptr->shutdown();
    if (e.loader) {
        e.loader->unload();
        delete e.loader;
    }

    m_plugins.remove(pluginId);
    qDebug() << "[PluginManager] 已卸载:" << pluginId;
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
    return m_plugins.value(pluginId).ptr;
}
