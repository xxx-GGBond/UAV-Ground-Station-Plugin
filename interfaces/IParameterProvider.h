#ifndef IPARAMETERPROVIDER_H
#define IPARAMETERPROVIDER_H

#include <QObject>
#include <QString>
#include <QVariant>
#include <QVector>

/**
 * @brief 单个参数的数据结构
 *
 * 对应 MAVLink PARAM_VALUE 消息的字段。
 * 飞控上的每个参数都有：名称（最长16字符）、值、类型、索引。
 */
struct ParamInfo {
    QString   id;       ///< 参数名称（飞控内部标识，如 "RC1_MIN", "BATT_MONITOR"）
    QVariant  value;    ///< 参数当前值
    int       type;     ///< MAVLink 参数类型（MAV_PARAM_TYPE_UINT8=1, INT32=4, REAL32=9 等）
    int       index;    ///< 参数在飞控参数列表中的索引（从 0 开始）
    QString   description;  ///< 参数描述（可选，部分飞控支持）
};

/**
 * @brief 参数读写接口 —— 与飞控进行参数交互的核心抽象
 *
 * 定位：
 *   所有对飞控参数的"读"和"写"操作都通过此接口进行。
 *   UI 层只关心 ParamInfo 结构体，完全不需要知道底层的
 *   MAVLink PARAM_REQUEST_LIST / PARAM_VALUE / PARAM_SET 等协议细节。
 *
 * 典型使用流程：
 *   1. 调用 requestList() 请求飞控发送全部参数
 *   2. 飞控逐个回复 PARAM_VALUE，插件内部解析后
 *      发出 paramReceived() 信号
 *   3. 发送端完成后发出 paramListComplete() 信号
 *   4. UI 调用 setParam() 写入新值，
 *      飞控确认后发出 paramSetAck() 信号
 */
class IParameterProvider : public QObject
{
    Q_OBJECT

public:
    explicit IParameterProvider(QObject* parent = nullptr) : QObject(parent) {}
    virtual ~IParameterProvider() = default;

    // ── 读操作 ──────────────────────────────────────────

    /**
     * @brief 请求飞控发送全部参数列表（异步）
     *
     * 底层发送 MAVLink PARAM_REQUEST_LIST 消息。
     * 结果通过 paramReceived() 逐个参数返回，
     * 全部完成后发出 paramListComplete()。
     */
    virtual void requestList() = 0;

    /**
     * @brief 请求飞控发送单个指定参数（异步）
     * @param paramId 参数名称（如 "RC1_MIN"）
     *
     * 底层发送 MAVLink PARAM_REQUEST_READ 消息。
     * 结果通过 paramReceived() 返回。
     */
    virtual void requestParam(const QString& paramId) = 0;

    // ── 写操作 ──────────────────────────────────────────

    /**
     * @brief 设置飞控上的某个参数值（异步）
     * @param paramId 参数名称
     * @param value   新值（类型需与参数类型匹配）
     *
     * 底层发送 MAVLink PARAM_SET 消息。
     * 飞控确认后发出 paramSetAck() 信号。
     */
    virtual void setParam(const QString& paramId, const QVariant& value) = 0;

    // ── 缓存查询（不从飞控读取，直接返回已缓存的参数） ──

    /// 获取已缓存的全部参数列表
    virtual QVector<ParamInfo> cachedParams() const = 0;

    /// 获取已缓存的单个参数值，不存在返回空 QVariant
    virtual QVariant cachedValue(const QString& paramId) const = 0;

signals:
    /**
     * @brief 收到单个参数（由 requestList 或 requestParam 触发）
     * @param param 参数信息
     *
     * 注意：此信号可能在非 UI 线程中发出。
     */
    void paramReceived(const ParamInfo& param);

    /**
     * @brief 全部参数接收完毕
     *
     * 收到此信号后，UI 可以调用 cachedParams() 获取完整列表。
     */
    void paramListComplete();

    /**
     * @brief 参数写入应答
     * @param paramId 参数名称
     * @param success true 表示写入成功
     */
    void paramSetAck(const QString& paramId, bool success);
};

#endif // IPARAMETERPROVIDER_H
