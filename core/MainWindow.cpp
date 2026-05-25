#include "MainWindow.h"
#include "PluginManager.h"
#include "LinkManager.h"
#include "MavlinkService.h"

#include <QCloseEvent>
#include <QMenuBar>
#include <QStatusBar>
#include <QApplication>
#include <QDir>
#include <QDebug>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    setWindowTitle(QStringLiteral("无人机地面站"));
    resize(800, 600);

    statusBar()->showMessage(QStringLiteral("就绪"));

    qDebug() << "[MainWindow] 构造完成";
}

MainWindow::~MainWindow()
{
    qDebug() << "[MainWindow] 析构";
}

bool MainWindow::initialize()
{
    // 插件目录 = exe 所在目录下的 plugins/
    const QString pluginPath = QApplication::applicationDirPath() + "/plugins";
    qDebug() << "[MainWindow] 插件搜索路径:" << pluginPath;

    // 1. 创建插件管理器并加载插件
    m_pluginMgr = new PluginManager(pluginPath, this);
    const int count = m_pluginMgr->loadAll();
    statusBar()->showMessage(QStringLiteral("已加载 %1 个插件").arg(count));
    qDebug() << "[MainWindow] 插件加载完毕，成功:" << count;

    // 2. 创建链路管理器
    m_linkMgr = new LinkManager(this);
    qDebug() << "[MainWindow] 链路管理器已创建";

    // 3. 创建 MAVLink 服务
    m_mavlinkSvc = new MavlinkService(this);
    qDebug() << "[MainWindow] MAVLink 服务已创建";

    return true;
}

void MainWindow::closeEvent(QCloseEvent* event)
{
    if (m_pluginMgr) {
        m_pluginMgr->unloadAll();
    }
    event->accept();
}
