#ifndef GLOBALDEFINES_H
#define GLOBALDEFINES_H

#include <QtGlobal>

/**
 * @file GlobalDefines.h
 * @brief 项目全局宏定义、常量、枚举
 *
 * 设计意图：
 *   将所有"散落"在代码各处的字面量集中管理，避免魔法数字（Magic Number）。
 *   所有模块（core/sdk/plugins）均可安全包含此文件，不引入任何依赖。
 */

// ── 项目版本信息 ─────────────────────────────────────────
#define GCS_VERSION_MAJOR   1
#define GCS_VERSION_MINOR   0
#define GCS_VERSION_PATCH   0

// ── MAVLink 协议相关常量 ─────────────────────────────────
#define MAVLINK_SYSTEM_ID_DEFAULT        255     ///< 地面站默认系统 ID
#define MAVLINK_COMPONENT_ID_DEFAULT     1       ///< 地面站组件 ID
#define MAVLINK_HEARTBEAT_TIMEOUT_MS     5000    ///< 心跳超时（毫秒），超时判定飞机离线

// ── 链路相关常量 ─────────────────────────────────────────
#define LINK_RECONNECT_INTERVAL_MS       3000    ///< 链路断开后自动重连间隔（毫秒）
#define LINK_MAX_RETRY_COUNT             5       ///< 链路最大重试次数
#define LINK_READ_BUFFER_SIZE            4096    ///< 链路接收缓冲区大小（字节）

// ── 航点规划限制 ─────────────────────────────────────────
#define MISSION_MAX_WAYPOINTS            500     ///< 单条任务最大航点数

// ── 编译期平台判断 ───────────────────────────────────────
#if defined(Q_OS_WIN)
#  define PLUGIN_FILE_EXT  ".dll"
#elif defined(Q_OS_MACOS)
#  define PLUGIN_FILE_EXT  ".dylib"
#else
#  define PLUGIN_FILE_EXT  ".so"
#endif

// ── 安全的指针删除辅助宏 ─────────────────────────────────
/// 安全删除 QObject 子类（利用 deleteLater 避免悬空指针问题）
#define SAFE_DELETE_LATER(obj) \
    do { \
        if ((obj) != nullptr) { \
            (obj)->deleteLater(); \
            (obj) = nullptr; \
        } \
    } while (0)

#endif // GLOBALDEFINES_H
