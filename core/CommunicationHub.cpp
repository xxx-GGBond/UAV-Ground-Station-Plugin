#include "CommunicationHub.h"
#include "interfaces/IPlugin.h"

#include <QDebug>

// ──────────────────────────────────────────────────────────
// CommunicationHub 实现
// ──────────────────────────────────────────────────────────

CommunicationHub::CommunicationHub(QObject* parent)
    : QObject(parent)
{
    qDebug() << "[CommunicationHub] 通信中枢已创建";
}

CommunicationHub::~CommunicationHub()
{
    qDebug() << "[CommunicationHub] 通信中枢已销毁";
    // 注意：此处不清除 m_plugins 中的指针，因为所有权归 PluginManager
    m_plugins.clear();
}

// ── 插件注册 / 注销 ─────────────────────────────────────

void CommunicationHub::registerPlugin(IPlugin* plugin)
{
    if (!plugin) {
        qWarning() << "[CommunicationHub] registerPlugin: 传入空指针，已忽略";
        return;
    }

    const QString id = plugin->pluginId();
    if (m_plugins.contains(id)) {
        qWarning() << "[CommunicationHub] 插件" << id << "已注册，跳过重复注册";
        return;
    }

    PluginEntry entry;
    entry.ptr      = plugin;
    entry.pluginId = id;
    m_plugins.insert(id, entry);

    qDebug() << "[CommunicationHub] 插件已注册:" << id
             << "名称:" << plugin->pluginName();
}

void CommunicationHub::unregisterPlugin(const QString& pluginId)
{
    if (m_plugins.remove(pluginId)) {
        qDebug() << "[CommunicationHub] 插件已注销:" << pluginId;
    } else {
        qWarning() << "[CommunicationHub] 尝试注销不存在的插件:" << pluginId;
    }
}

// ── 核心服务注册 ────────────────────────────────────────

void CommunicationHub::setLinkManager(ILinkManager* manager)
{
    m_linkManager = manager;
    qDebug() << "[CommunicationHub] 链路管理器已注册";
}

void CommunicationHub::setTelemetryProvider(ITelemetryProvider* provider)
{
    m_telemetryProvider = provider;
    qDebug() << "[CommunicationHub] 遥测提供者已注册";
}

void CommunicationHub::setMissionManager(IMissionManager* manager)
{
    m_missionManager = manager;
    qDebug() << "[CommunicationHub] 任务管理器已注册";
}

// ── 查询 ─────────────────────────────────────────────────

ILinkManager* CommunicationHub::linkManager() const
{
    return m_linkManager;
}

ITelemetryProvider* CommunicationHub::telemetryProvider() const
{
    return m_telemetryProvider;
}

IMissionManager* CommunicationHub::missionManager() const
{
    return m_missionManager;
}
