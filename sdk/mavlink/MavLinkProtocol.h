#ifndef MAVLINKPROTOCOL_H
#define MAVLINKPROTOCOL_H

#include <QObject>
#include <QByteArray>
#include <functional>

/**
 * @file MavLinkProtocol.h
 * @brief MAVLink 协议处理骨架 —— SDK 层的 MAVLink 协议封装
 *
 * 定位：
 *   这是整个 SDK 中处理 MAVLink 协议的入口类。它不直接依赖 mavlink 库，
 *   而是通过模板/函数对象的方式解耦，避免 SDK 层与第三方库强绑定。
 *
 * 为什么放在 SDK 而非直接放在 core？
 *   - SDK 层是"可复用公共库"，协议处理是通用能力
 *   - 其他项目可以独立引用 SDK 而不引入 core 的依赖
 *   - 便于单元测试：可以模拟 MAVLink 字节流做协议解析测试
 *
 * MAVLink 协议简介（供初学者理解）：
 *   MAVLink 是无人机通信领域最主流的轻量级协议。
 *   每条消息的二进制结构：
 *   [STX(1)] [LEN(1)] [SEQ(1)] [SYS(1)] [COMP(1)] [MSGID(1)] [PAYLOAD(0~255)] [CKA(1) CKB(1)]
 *   其中 STX = 0xFE 表示 v1.0 协议，0xFD 表示 v2.0 协议。
 */

// ── MAVLink 消息 ID 枚举（常用子集） ──────────────────────
// 完整列表见 mavlink 库的 common/mavlink_msg_*.h

enum class MavLinkMessageId : uint32_t {
    HEARTBEAT           = 0,    ///< 心跳包（系统状态、模式）
    SYS_STATUS          = 1,    ///< 系统状态（电池、通信质量）
    GPS_RAW_INT         = 24,   ///< GPS 原始数据（经纬度、高度、速度）
    ATTITUDE            = 30,   ///< 姿态数据（roll/pitch/yaw）
    GLOBAL_POSITION_INT = 33,   ///< 全局位置（经纬度、相对/绝对高度）
    COMMAND_LONG        = 76,   ///< 长指令（用于发送任务指令）
    MISSION_ITEM        = 39,   ///< 航点数据（上传/下载）
    MISSION_COUNT       = 44,   ///< 航点总数声明
    MISSION_ACK         = 47,   ///< 航点操作应答
    PARAM_VALUE         = 22,   ///< 参数值
};

/**
 * @brief MAVLink 消息解析结果
 *
 * 将二进制原始字节解析为结构化数据，供上层（遥测、任务等）使用。
 * 这是一个"中立"结构体——不依赖于 mavlink 库的具体类型。
 */
struct MavLinkMessage {
    uint8_t     magic;          ///< 协议魔术字（0xFE=v1, 0xFD=v2）
    uint8_t     len;            ///< 负载长度
    uint8_t     seq;            ///< 包序号（用于检测丢包）
    uint8_t     sysid;          ///< 系统 ID（发送方）
    uint8_t     compid;         ///< 组件 ID（发送方）
    uint32_t    msgid;          ///< 消息 ID（对应 MavLinkMessageId 枚举值）
    QByteArray  payload;        ///< 负载数据（原始字节）
    uint16_t    checksum;       ///< CRC 校验和
};

/**
 * @brief MAVLink 协议处理器骨架
 *
 * 设计思路：
 *   实际项目中会引入 mavlink 官方 C 库（third_party/mavlink/），
 *   本类提供协议处理的"框架"，实际解析函数由 mavlink 库的代码生成器生成。
 *
 * 回调函数说明：
 *   使用 std::function 作为消息回调类型，而非 Qt 信号。原因是：
 *   1. SDK 层可能被非 Qt 项目复用
 *   2. 回调可以灵活绑定到 lambda、成员函数、自由函数
 *   3. 性能敏感场景下避免了信号槽的元对象开销
 */
class MavLinkProtocol
{
public:
    /// 消息处理回调类型：参数为解析后的结构化消息
    using MessageCallback = std::function<void(const MavLinkMessage& msg)>;

    MavLinkProtocol() = default;
    ~MavLinkProtocol() = default;

    /**
     * @brief 注册某类消息的回调处理函数
     * @param msgId 感兴趣的消息 ID
     * @param callback 收到该类消息时的回调
     *
     * 使用示例：
     *   protocol.onMessage(MavLinkMessageId::GPS_RAW_INT,
     *       [](const MavLinkMessage& msg) {
     *           // 解析 GPS 数据并更新 UI
     *       });
     */
    void onMessage(MavLinkMessageId msgId, MessageCallback callback);

    /**
     * @brief 将原始字节流送入解析器（喂数据）
     * @param rawData 从链路收到的原始字节
     *
     * 解析器内部维护状态机，处理粘包/拆包问题。
     * 每当完整解析出一条消息，就调用对应的回调。
     */
    void feedData(const QByteArray& rawData);

    /**
     * @brief 将结构化消息打包为原始字节（用于发送）
     * @param msg 待发送的结构化消息
     * @return 打包后的二进制数据
     */
    QByteArray packMessage(const MavLinkMessage& msg) const;

private:
    // 内部解析状态机缓冲区（处理粘包）
    QByteArray m_buffer;

    // 消息 ID -> 回调函数 映射表
    QMap<uint32_t, MessageCallback> m_callbacks;
};

#endif // MAVLINKPROTOCOL_H
