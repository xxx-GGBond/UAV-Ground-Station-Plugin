#ifndef ITELEMETRYPROVIDER_H
#define ITELEMETRYPROVIDER_H

#include <QObject>
#include <QString>
#include <QJsonObject>
#include <QVector3D>

/**
 * @brief 无人机遥测数据结构体
 *
 * 封装一架无人机当前时刻的完整遥测数据快照。
 * 数据来源于 MAVLink 心跳包、GPS_RAW_INT、ATTITUDE 等消息的解析结果。
 */
struct TelemetryData {
    // ── 系统标识 ──
    uint8_t  systemId    = 0;    ///< MAVLink 系统 ID
    uint8_t  componentId = 0;    ///< MAVLink 组件 ID

    // ── 位置（GPS 经纬度 + 海拔） ──
    double   latitude    = 0.0;  ///< 纬度（度）
    double   longitude   = 0.0;  ///< 经度（度）
    double   altitude    = 0.0;  ///< 海拔高度（米）

    // ── 姿态（欧拉角，单位：度） ──
    float    roll        = 0.0f; ///< 横滚角
    float    pitch       = 0.0f; ///< 俯仰角
    float    yaw         = 0.0f; ///< 偏航角

    // ── 速度 ──
    float    groundSpeed = 0.0f; ///< 地速（m/s）
    float    airSpeed    = 0.0f; ///< 空速（m/s）

    // ── 电池 ──
    float    batteryVoltage  = 0.0f; ///< 电池电压（V）
    int32_t  batteryPercent  = -1;   ///< 电量百分比（0-100），-1 表示无效

    // ── 状态 ──
    bool     armed       = false;///< 是否解锁
    QString  flightMode;         ///< 飞行模式（如 "GUIDED", "AUTO", "LOITER"）

    // ── 时间戳 ──
    uint64_t timestamp   = 0;    ///< 数据采集时刻（毫秒级 Unix 时间戳）
};

/**
 * @brief 遥测数据提供者接口
 *
 * 定位：
 *   对上层（UI、日志、任务管理等）屏蔽 MAVLink 协议解析细节，
 *   只暴露标准化的 TelemetryData 结构体。
 *
 * 为什么这样设计？
 *   - UI 层不关心 MAVLink 二进制协议格式，只关心"经纬度多少"
 *   - 未来如果切换到其他协议（如 DJI SDK），只需替换 Provider 实现
 *   - 多个消费者（地图、仪表盘、日志）可以同时监听同一个信号
 */
class ITelemetryProvider : public QObject
{
    Q_OBJECT

public:
    explicit ITelemetryProvider(QObject* parent = nullptr) : QObject(parent) {}
    virtual ~ITelemetryProvider() = default;

    /**
     * @brief 获取指定飞机的最近一次遥测数据快照
     * @param systemId MAVLink 系统 ID
     * @return 遥测数据，若飞机不存在则返回默认构造的空结构体
     */
    virtual TelemetryData getLatestTelemetry(uint8_t systemId) const = 0;

    /**
     * @brief 获取当前在线的所有飞机系统 ID 列表
     */
    virtual QVector<uint8_t> getOnlineSystems() const = 0;

signals:
    /**
     * @brief 遥测数据更新信号 —— 当收到新遥测数据时发出
     * @param data 最新的遥测数据快照
     */
    void telemetryUpdated(const TelemetryData& data);

    /**
     * @brief 飞机上线信号
     * @param systemId 新上线的飞机系统 ID
     */
    void systemOnline(uint8_t systemId);

    /**
     * @brief 飞机离线信号（心跳超时）
     * @param systemId 离线的飞机系统 ID
     */
    void systemOffline(uint8_t systemId);
};

#endif // ITELEMETRYPROVIDER_H
