#include "LinkManager.h"
#include "SerialLink.h"
#include "UdpLink.h"

#include <QDebug>

LinkManager::LinkManager(QObject* parent)
    : ILinkManager(parent)
{
}

LinkManager::~LinkManager()
{
    // 清理所有链路
    for (auto& entry : m_links) {
        if (entry.link) {
            entry.link->close();
            delete entry.link;
        }
    }
    m_links.clear();
}

int LinkManager::createLink(const QString& type, const QJsonObject& config)
{
    ILink* link = nullptr;

    if (type == "serial") {
        link = new SerialLink(this);
    } else if (type == "udp") {
        link = new UdpLink(this);
    } else {
        qWarning() << "[LinkManager] 未知链路类型:" << type;
        return -1;
    }

    // 转发链路信号
    connect(link, &ILink::dataReceived, this, [this, link](const QByteArray& data) {
        for (auto& e : m_links) {
            if (e.link == link) {
                emit dataReceived(e.id, data);
                break;
            }
        }
    });

    connect(link, &ILink::stateChanged, this, [this, link](bool connected) {
        for (auto& e : m_links) {
            if (e.link == link) {
                emit linkStateChanged(e.id, connected);
                break;
            }
        }
    });

    Entry entry;
    entry.id   = m_nextId++;
    entry.type = type;
    entry.link = link;
    m_links.append(entry);

    qDebug() << "[LinkManager] 链路已创建:" << entry.id << type;
    return entry.id;
}

bool LinkManager::destroyLink(int linkId)
{
    Entry* e = find(linkId);
    if (!e) return false;

    if (e->link) {
        e->link->close();
        delete e->link;
    }
    m_links.removeOne(*e);
    qDebug() << "[LinkManager] 链路已销毁:" << linkId;
    return true;
}

bool LinkManager::openLink(int linkId)
{
    // 此处暂不调用 link->open()，因为 config 在 createLink 时已提供
    // 实际项目中可在 createLink 时同时 open，或另外设计
    Q_UNUSED(linkId);
    return false;
}

void LinkManager::closeLink(int linkId)
{
    Entry* e = find(linkId);
    if (e && e->link) {
        e->link->close();
    }
}

qint64 LinkManager::sendData(int linkId, const QByteArray& data)
{
    Entry* e = find(linkId);
    if (e && e->link && e->link->isOpen()) {
        return e->link->send(data);
    }
    return -1;
}

bool LinkManager::isLinkOpen(int linkId) const
{
    for (auto& e : m_links) {
        if (e.id == linkId && e.link) {
            return e.link->isOpen();
        }
    }
    return false;
}

LinkManager::Entry* LinkManager::find(int linkId)
{
    for (auto& e : m_links) {
        if (e.id == linkId) return &e;
    }
    return nullptr;
}
