#ifndef LINKMANAGER_H
#define LINKMANAGER_H

#include "interfaces/ILinkManager.h"
#include <QList>

class ILink;

/**
 * @brief 链路管理器 —— 统一管理所有通信链路
 *
 * 实现 ILinkManager 接口，负责创建 SerialLink/UdpLink 实例并管理其生命周期。
 */
class LinkManager : public ILinkManager
{
    Q_OBJECT

public:
    explicit LinkManager(QObject* parent = nullptr);
    ~LinkManager() override;

    int  createLink(const QString& type, const QJsonObject& config) override;
    bool destroyLink(int linkId) override;
    bool openLink(int linkId) override;
    void closeLink(int linkId) override;
    qint64 sendData(int linkId, const QByteArray& data) override;
    bool isLinkOpen(int linkId) const override;

private:
    struct Entry {
        int     id   = -1;
        QString type;
        ILink*  link = nullptr;

        bool operator==(const Entry& other) const { return id == other.id; }
    };

    int    m_nextId  = 0;
    QList<Entry> m_links;

    /// 根据 ID 查找条目，未找到返回 nullptr
    Entry* find(int linkId);
};

#endif // LINKMANAGER_H
