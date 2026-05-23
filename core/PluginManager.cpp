#include "PluginManager.h"
#include "CommunicationHub.h"
#include "interfaces/IPlugin.h"

#include <QPluginLoader>
#include <QDebug>
#include <QFileInfo>

// ──────────────────────────────────────────────────────────
// PluginManager 实现
// ──────────────────────────────────────────────────────────

PluginManager::PluginManager(CommunicationHub* hub,
                             const QString& pluginDir,
                             QObject* parent)
    : QObject(parent)
    , m_hub(hub)
    , m_pluginDir(pluginDir)
{
    qDebug() << "[PluginManager] 插件管理器已创建，搜索目录:" << m_pluginDir.absolutePath();
}

PluginManager::~PluginManager()
{
    qDebug() << "[PluginManager] 插件管理器销毁中...";
    unloadAllPlugins();
}

// ── 批量加载 ─────────────────────────────────────────────

int PluginManager::loadAllPlugins()
{
    // 确保插件目录存在
    if (!m_pluginDir.exists()) {
        qWarning() << "[PluginManager] 插件目录不存在:" << m_pluginDir.absolutePath();
        return 0;
    }

    // 获取所有动态库文件（Windows: .dll, Linux: .so）
    QStringList nameFilters;
#ifdef Q_OS_WIN
    nameFilters << "*.dll";
#else
    nameFilters << "*.so";
#endif

    const QFileInfoList entries = m_pluginDir.entryInfoList(nameFilters, QDir::Files);
    int loadedCount = 0;

    for (const QFileInfo& info : entries) {
        IPlugin* plugin = loadPlugin(info.absoluteFilePath());
        if (plugin) {
            ++loadedCount;
        }
    }

    qDebug() << "[PluginManager] 扫描完成，成功加载" << loadedCount
             << "/" << entries.size() << "个插件";
    return loadedCount;
}

// ── 单插件加载 ───────────────────────────────────────────

IPlugin* PluginManager::loadPlugin(const QString& filePath)
{
    // 创建 QPluginLoader（底层调用 LoadLibrary/dlopen）
    auto loader = std::make_unique<QPluginLoader>(filePath);

    // 检查插件元数据中的 IID 是否匹配
    const QString iid = loader->metaData().value("IID").toString();
    if (iid != IPLUGIN_IID) {
        qWarning() << "[PluginManager] 插件 IID 不匹配，跳过:" << filePath;
        qWarning() << "  期望:" << IPLUGIN_IID << "实际:" << iid;
        return nullptr;
    }

    // 获取插件根 QObject 实例
    QObject* instance = loader->instance();
    if (!instance) {
        qWarning() << "[PluginManager] 无法加载插件实例:" << filePath
                     << "错误:" << loader->errorString();
        return nullptr;
    }

    // 安全映射到 IPlugin 接口（利用 qobject_cast 的 RTTI 检查）
    IPlugin* plugin = qobject_cast<IPlugin*>(instance);
    if (!plugin) {
        qWarning() << "[PluginManager] 插件未实现 IPlugin 接口:" << filePath;
        loader->unload();  // QPluginLoader 负责释放 instance
        return nullptr;
    }

    // 调用插件初始化
    if (!plugin->initialize()) {
        qWarning() << "[PluginManager] 插件初始化失败:" << plugin->pluginId();
        loader->unload();
        return nullptr;
    }

    // 注册到通信中枢
    m_hub->registerPlugin(plugin);

    // 记录到内部映射表
    const QString id = plugin->pluginId();
    LoadedPlugin entry;
    entry.loader = std::move(loader);   // 转移所有权
    entry.ptr    = plugin;
    m_plugins.insert(id, entry);

    qDebug() << "[PluginManager] 插件加载成功:" << id
             << "版本:" << plugin->pluginVersion();

    emit pluginLoaded(id);
    return plugin;
}

// ── 卸载 ─────────────────────────────────────────────────

bool PluginManager::unloadPlugin(const QString& pluginId)
{
    if (!m_plugins.contains(pluginId)) {
        qWarning() << "[PluginManager] 尝试卸载不存在的插件:" << pluginId;
        return false;
    }

    emit pluginAboutToUnload(pluginId);

    LoadedPlugin& entry = m_plugins[pluginId];

    // 1. 从通信中枢注销
    m_hub->unregisterPlugin(pluginId);

    // 2. 调用插件自身的 shutdown
    if (entry.ptr) {
        entry.ptr->shutdown();
    }

    // 3. 卸载动态库（QPluginLoader::unload 内部调用 FreeLibrary/dlclose）
    //    注意：entry.ptr 指向的内存由 QPluginLoader 管理，unload 后自动释放
    if (entry.loader) {
        if (!entry.loader->unload()) {
            qWarning() << "[PluginManager] 插件卸载警告:" << entry.loader->errorString();
        }
    }

    m_plugins.remove(pluginId);

    emit pluginUnloaded(pluginId);
    qDebug() << "[PluginManager] 插件已卸载:" << pluginId;
    return true;
}

void PluginManager::unloadAllPlugins()
{
    const QStringList ids = m_plugins.keys();
    for (const QString& id : ids) {
        unloadPlugin(id);
    }
    qDebug() << "[PluginManager] 所有插件已卸载，共" << ids.size() << "个";
}

// ── 查询 ─────────────────────────────────────────────────

QStringList PluginManager::loadedPluginIds() const
{
    return m_plugins.keys();
}
