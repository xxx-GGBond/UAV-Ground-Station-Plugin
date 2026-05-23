#ifndef IMISSIONMANAGER_H
#define IMISSIONMANAGER_H

#include <QObject>
#include <QString>
#include <QJsonArray>
#include <QVector>

/**
 * @brief 任务项（单个航点或动作）
 *
 * 设计意图：
 *   不直接使用 MAVLink 的 MissionItem 结构体，而是用中立的数据结构，
 *   这样未来切换到其他无人机协议时不需要修改接口。
 */
struct Waypoint {
    int     sequence    = 0;      ///< 航点序号（从 0 开始）
    double  latitude    = 0.0;    ///< 纬度（度）
    double  longitude   = 0.0;    ///< 经度（度）
    double  altitude    = 0.0;    ///< 目标高度（米）
    float   speed       = 0.0f;   ///< 目标速度（m/s），0 表示使用默认值
    uint16_t command    = 16;     ///< MAVLink 指令（默认 MAV_CMD_NAV_WAYPOINT）
};

/**
 * @brief 任务状态枚举
 */
enum class MissionState {
    Idle,           ///< 无任务执行中
    Uploading,      ///< 正在上传任务到飞控
    Executing,      ///< 任务执行中
    Paused,         ///< 任务暂停
    Completed,      ///< 任务完成
    Error           ///< 任务异常
};

/**
 * @brief 任务管理器接口 —— 管理无人机任务的创建、上传、监控
 *
 * 定位：
 *   这是一架飞机的"任务管家"。它不关心物理链路怎么发送数据，
 *   只负责把 Waypoint 列表转换成 MAVLink MissionItem 并调用发送。
 *
 * 为什么独立成接口？
 *   - 任务管理是核心业务，逻辑复杂（上传/下载/校验/状态机）
 *   - 与链路管理、遥测提供者解耦，通过信号与槽通信
 */
class IMissionManager : public QObject
{
    Q_OBJECT

public:
    explicit IMissionManager(QObject* parent = nullptr) : QObject(parent) {}
    virtual ~IMissionManager() = default;

    /**
     * @brief 加载航点列表到内存（不发送到飞控）
     * @param waypoints 航点列表
     */
    virtual void loadWaypoints(const QVector<Waypoint>& waypoints) = 0;

    /**
     * @brief 获取当前内存中的航点列表
     */
    virtual QVector<Waypoint> getWaypoints() const = 0;

    /**
     * @brief 上传任务到飞控
     * @param systemId 目标飞机系统 ID
     * @return true 表示上传请求已发送（结果通过信号通知）
     */
    virtual bool uploadMission(uint8_t systemId) = 0;

    /**
     * @brief 从飞控下载当前任务
     * @param systemId 目标飞机系统 ID
     * @return true 表示下载请求已发送
     */
    virtual bool downloadMission(uint8_t systemId) = 0;

    /**
     * @brief 指令飞控开始执行已上传的任务
     * @param systemId 目标飞机系统 ID
     */
    virtual bool startMission(uint8_t systemId) = 0;

    /**
     * @brief 暂停当前任务
     * @param systemId 目标飞机系统 ID
     */
    virtual bool pauseMission(uint8_t systemId) = 0;

    /**
     * @brief 获取当前任务状态
     */
    virtual MissionState getMissionState(uint8_t systemId) const = 0;

signals:
    /// 任务状态变化信号
    void missionStateChanged(uint8_t systemId, MissionState state);

    /// 当前航点序号变化信号（0-based）
    void currentWaypointChanged(uint8_t systemId, int sequence);

    /// 任务上传进度信号
    void missionUploadProgress(uint8_t systemId, int current, int total);

    /// 任务下载完成信号
    void missionDownloaded(uint8_t systemId, const QVector<Waypoint>& waypoints);
};

#endif // IMISSIONMANAGER_H
