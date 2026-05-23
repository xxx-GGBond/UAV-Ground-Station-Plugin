#include "MainWindow.h"
#include "CommunicationHub.h"
#include "PluginManager.h"

#include <QCloseEvent>
#include <QMenuBar>
#include <QStatusBar>
#include <QDockWidget>
#include <QMessageBox>
#include <QDebug>
#include <QApplication>

// ──────────────────────────────────────────────────────────
// MainWindow 实现
// ──────────────────────────────────────────────────────────

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    // 设置窗口基本属性
    setWindowTitle(QStringLiteral("UAV Ground Station"));
    resize(1280, 720);

    // 创建状态栏
    statusBar()->showMessage(QStringLiteral("地面站就绪"));

    qDebug() << "[MainWindow] 主窗口已创建";
}

MainWindow::~MainWindow()
{
    qDebug() << "[MainWindow] 主窗口销毁中...";
    // m_hub 和 m_pluginMgr 由 unique_ptr 自动析构
    // 析构顺序：先 m_pluginMgr（卸载所有插件），后 m_hub
}

bool MainWindow::initialize()
{
    // 第一步：创建通信中枢（所有模块的"中央交换机"）
    m_hub = std::make_unique<CommunicationHub>(this);

    // 第二步：创建插件管理器（将中枢注入，插件加载后自动注册到中枢）
    m_pluginMgr = std::make_unique<PluginManager>(m_hub.get(), "./plugins", this);

    // 第三步：扫描并加载所有插件
    const int count = m_pluginMgr->loadAllPlugins();
    statusBar()->showMessage(
        QStringLiteral("已加载 %1 个插件").arg(count));

    qDebug() << "[MainWindow] 初始化完成，已加载" << count << "个插件";

    // 第四步：设置 UI 布局
    // 实际的 QDockWidget 创建和布局由各插件通过 IUiComponent 接口自行完成
    // MainWindow 只负责创建菜单等框架 UI
    QMenu* fileMenu = menuBar()->addMenu(QStringLiteral("文件(&F)"));
    Q_UNUSED(fileMenu);  // 菜单项由插件动态添加（后续扩展）

    QMenu* viewMenu = menuBar()->addMenu(QStringLiteral("视图(&V)"));
    Q_UNUSED(viewMenu);

    QMenu* helpMenu = menuBar()->addMenu(QStringLiteral("帮助(&H)"));
    Q_UNUSED(helpMenu);

    return true;
}

void MainWindow::closeEvent(QCloseEvent* event)
{
    qDebug() << "[MainWindow] 收到关闭事件，准备退出...";

    // 插件会在 m_pluginMgr 的析构中自动卸载
    // 这里可以添加"是否保存"等确认逻辑

    event->accept();
}
