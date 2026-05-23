#include "MainWindow.h"
#include "PluginManager.h"

#include <QCloseEvent>
#include <QMenuBar>
#include <QStatusBar>
#include <QDebug>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    setWindowTitle(QStringLiteral("UAV Ground Station —— 参数读写"));
    resize(800, 600);
    statusBar()->showMessage(QStringLiteral("就绪"));
}

MainWindow::~MainWindow()
{
    // m_pluginMgr 由 unique_ptr 自动析构，析构时卸载全部插件
}

bool MainWindow::initialize()
{
    m_pluginMgr = std::make_unique<PluginManager>("./plugins", this);

    const int count = m_pluginMgr->loadAll();
    statusBar()->showMessage(QStringLiteral("已加载 %1 个插件").arg(count));

    qDebug() << "[MainWindow] 初始化完成，已加载" << count << "个插件";

    // 菜单栏（骨架）
    menuBar()->addMenu(QStringLiteral("文件(&F)"));
    menuBar()->addMenu(QStringLiteral("视图(&V)"));
    menuBar()->addMenu(QStringLiteral("帮助(&H)"));

    return true;
}

void MainWindow::closeEvent(QCloseEvent* event)
{
    event->accept();
}
