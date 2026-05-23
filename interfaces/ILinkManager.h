#ifndef ILINKMANAGER_H
#define ILINKMANAGER_H

#include <QObject>
#include <QString>
#include <QByteArray>
#include <QVector>

/**
 * @brief 链路状态枚举
 *
 * 描述一条通信链路当前所处的状态。
 * 链路可能是串口、UDP、TCP 等物理通道的抽象。
 */
enum class LinkState {
    Disconnected,   ///< 已断开
    Connecting,     ///< 正在连接中
    Connected,      ///< 已连接且正常工作
    Error           ///< 异常状态
};

/**
 * @brief 链路信息结构体
 *
 * 封装单条链路的基本元数据，供 UI 或日志模块使用。
 */
struct LinkInfo {
    int         linkId    = -1;       ///< 链路唯一 ID（由链路管理器分配）
    QString     name;                 ///< 链路自定义名称（如 "数传电台 #1"）
    QString     type;                 ///< 链路类型（"serial" / "udp" / "tcp"）
    LinkState   state     = LinkState::Disconnected;
    QString     details;              ///< 连接参数描述（如 "COM3@115200"）
};

/**
 * @brief 链路管理接口 —— 抽象所有物理 / 网络通信链路的管理行为
 *
 * 定位：
 *   这是整个地面站通信层的入口接口。所有链路（串口、UDP、TCP 等）的
 *   创建、销毁、数据收发均通过此接口统一调度。
 *
 * 为什么使用接口？
 *   - 宿主只依赖 ILinkManager，不关心底层是串口还是网络
 *   - 可以轻松替换链路实现（例如从串口切换到 UDP）而不影响上层
 *   - 利于单元测试：可以注入一个 Mock 链路管理器
 */
class ILinkManager : public QObject
{
    Q_OBJECT

public:
    explicit ILinkManager(QObject* parent = nullptr) : QObject(parent) {}
    virtual ~ILinkManager() = default;

    // ── 链路生命周期 ────────────────────────────────────────

    /**
     * @brief 创建一条新链路
     * @param type 链路类型（"serial"/"udp"/"tcp"）
     * @param config 链路配置参数（JSON 格式）
     * @return 新链路的唯一 ID，失败返回 -1
     */
    virtual int createLink(const QString& type, const QJsonObject& config) = 0;

    /**
     * @brief 销毁指定链路
     * @param linkId 链路 ID
     * @return true 表示成功销毁
     */
    virtual bool destroyLink(int linkId) = 0;

    /**
     * @brief 连接指定链路
     * @param linkId 链路 ID
     * @return true 表示连接指令已发送（实际连接结果通过信号通知）
     */
    virtual bool connectLink(int linkId) = 0;

    /**
     * @brief 断开指定链路
     * @param linkId 链路 ID
     * @return true 表示断开成功
     */
    virtual bool disconnectLink(int linkId) = 0;

    // ── 数据收发（纯虚，由具体链路实现） ────────────────────

    /**
     * @brief 通过指定链路发送原始字节数据
     * @param linkId 目标链路 ID
     * @param data 待发送数据
     * @return 实际发送的字节数，失败返回 -1
     */
    virtual int64_t sendData(int linkId, const QByteArray& data) = 0;

    // ── 查询 ────────────────────────────────────────────────

    /**
     * @brief 获取所有链路的当前信息快照
     */
    virtual QVector<LinkInfo> getAllLinks() const = 0;

    /**
     * @brief 获取指定链路的当前状态
     */
    virtual LinkState getLinkState(int linkId) const = 0;

signals:
    /**
     * @brief 链路状态变化信号 —— 任何链路的状态改变都会发出此信号
     * @param linkId 变化的链路 ID
     * @param newState 新状态
     */
    void linkStateChanged(int linkId, LinkState newState);

    /**
     * @brief 数据接收信号 —— 从链路收到原始数据时发出
     * @param linkId 来源链路 ID
     * @param data 原始数据
     *
     * 注意：此信号在非 UI 线程中发出，接收者需要注意线程安全。
     */
    void dataReceived(int linkId, const QByteArray& data);
};

#endif // ILINKMANAGER_H
