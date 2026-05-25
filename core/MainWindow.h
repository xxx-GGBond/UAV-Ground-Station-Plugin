#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class PluginManager;
class LinkManager;
class MavlinkService;

/**
 * @brief 地面站主窗口 —— 负责初始化核心服务与插件系统
 *
 * 按顺序初始化：
 *   1. PluginManager  —— 扫描并加载 plugins/ 目录下的 Qt 插件
 *   2. LinkManager     —— 管理串口/UDP 等通信链路
 *   3. MavlinkService  —— MAVLink 协议处理（当前为占位实现）
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override;

    /// 初始化所有核心组件，返回 true 表示成功
    bool initialize();

protected:
    void closeEvent(QCloseEvent* event) override;

private:
    PluginManager*  m_pluginMgr  = nullptr;
    LinkManager*    m_linkMgr    = nullptr;
    MavlinkService* m_mavlinkSvc = nullptr;
};

#endif // MAINWINDOW_H
