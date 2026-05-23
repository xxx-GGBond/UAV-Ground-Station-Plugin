#include "MavLinkProtocol.h"

#include <QDebug>

// ──────────────────────────────────────────────────────────
// MavLinkProtocol 实现（骨架）
//
// 说明：
//   当前为骨架实现，展示协议处理器的架构和调用流程。
//   完整实现需要集成 third_party/mavlink/ 库的代码生成器
//   生成的 mavlink_helper.h 和 mavlink_msg_*.h 文件。
//   届时替换 feedData/packMessage 中的解析/打包逻辑即可，
//   接口（onMessage/feedData/packMessage）不需要修改。
// ──────────────────────────────────────────────────────────

void MavLinkProtocol::onMessage(MavLinkMessageId msgId, MessageCallback callback)
{
    const uint32_t id = static_cast<uint32_t>(msgId);
    m_callbacks.insert(id, std::move(callback));
    qDebug() << "[MavLinkProtocol] 注册消息回调，消息ID:" << id;
}

void MavLinkProtocol::feedData(const QByteArray& rawData)
{
    // 追加到内部缓冲区（处理粘包：一次可能收到多个包，或半个包）
    m_buffer.append(rawData);

    // ── 简化的状态机解析（骨架实现） ─────────────────
    // 完整实现详见 mavlink 官方库的 mavlink_parse_char() 函数

    while (m_buffer.size() >= 2) {
        // 查找包头魔术字 STX
        const uint8_t magic = static_cast<uint8_t>(m_buffer.at(0));
        if (magic != 0xFE && magic != 0xFD) {
            // 不是合法的 MAVLink 包头，丢弃一个字节继续找
            m_buffer.remove(0, 1);
            continue;
        }

        // 简化的最小包长判断（实际应根据协议版本计算）
        // v1: 8 字节头 + CRC，最小 8 字节；v2: 12 字节头 + CRC
        const int minLen = (magic == 0xFE) ? 8 : 12;
        if (m_buffer.size() < minLen) {
            return;  // 数据不够，等下一次 feedData
        }

        // 解析消息
        MavLinkMessage msg;
        msg.magic = magic;
        msg.len   = static_cast<uint8_t>(m_buffer.at(1));
        // ... 完整解析此处省略（骨架实现）

        // 查找已注册的回调
        auto it = m_callbacks.find(msg.msgid);
        if (it != m_callbacks.end() && it.value()) {
            it.value()(msg);  // 调用回调
        }

        // 从缓冲区移除已解析的字节
        const int totalLen = msg.len + minLen;  // 头 + 负载 + CRC
        m_buffer.remove(0, totalLen);
    }
}

QByteArray MavLinkProtocol::packMessage(const MavLinkMessage& msg) const
{
    // ── 简化的打包逻辑（骨架实现） ─────────────────
    // 完整实现使用 mavlink_msg_xxx_pack() 系列函数

    QByteArray packed;
    packed.append(static_cast<char>(msg.magic));
    packed.append(static_cast<char>(msg.len));
    packed.append(static_cast<char>(msg.seq));
    packed.append(static_cast<char>(msg.sysid));
    packed.append(static_cast<char>(msg.compid));
    packed.append(static_cast<char>(msg.msgid & 0xFF));
    packed.append(msg.payload);
    // CRC 计算此处省略（骨架实现）

    return packed;
}
