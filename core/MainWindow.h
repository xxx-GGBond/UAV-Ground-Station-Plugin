#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <memory>

// 前置声明（不包含具体头文件，减少编译依赖）
class CommunicationHub;
class PluginManager;

/**
 * @brief 主窗口 —— 地面站的顶层 UI 容器
 *
 * 定位：
 *   MainWindow 是地面站的主界面框架。它本身不包含任何业务逻辑——
 *   所有业务功能由插件提供，MainWindow 只是"画布"：
 *   1. 创建菜单栏、工具栏、状态栏
 *   2. 管理 QDockWidget 的布局位置（通过插件的 preferredDockArea）
 *   3. 委托 PluginManager 加载插件
 *
 * 为什么"瘦主窗口"？
 *   - 主窗口代码不依赖任何具体插件，新增功能不需要修改 MainWindow
 *   - 符合开闭原则（Open-Closed Principle）：对扩展开放，对修改关闭
 *   - Qt 的 QDockWidget 天然支持插件化 UI 布局
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override;

    /**
     * @brief 初始化主窗口 —— 创建通信中枢、加载所有插件
     * @return true 表示初始化成功
     *
     * 设计决策：将初始化逻辑从构造函数中分离出来，
     * 方便在 main() 中控制初始化顺序和错误处理。
     */
    bool initialize();

protected:
    /**
     * @brief 窗口关闭事件 —— 确保插件正确卸载
     */
    void closeEvent(QCloseEvent* event) override;

private:
    // 核心组件（栈上成员，生命周期由 MainWindow 管理）
    // std::unique_ptr 明确表达了所有权语义
    std::unique_ptr<CommunicationHub> m_hub;       ///< 通信中枢
    std::unique_ptr<PluginManager>    m_pluginMgr; ///< 插件管理器

    // ── UI 组件 ──
    // QDockWidget 的布局状态在 saveState/restoreState 中持久化
};

#endif // MAINWINDOW_H
