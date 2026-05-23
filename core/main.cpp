/**
 * @file main.cpp
 * @brief 地面站应用程序入口
 *
 * 启动流程：
 *   1. 创建 QApplication 事件循环
 *   2. 创建 MainWindow 主窗口
 *   3. 初始化通信中枢和插件系统
 *   4. 进入 Qt 事件循环（exec）
 *
 * QApplication vs QCoreApplication：
 *   QApplication 继承自 QCoreApplication，额外提供了 GUI 事件处理、
 *   窗口系统集成、剪贴板、拖放等功能。地面站是 GUI 程序，必须用 QApplication。
 */
#include <QApplication>
#include <QDebug>

#include "MainWindow.h"

int main(int argc, char* argv[])
{
    // 启用高 DPI 缩放（Qt 5.6+ 支持）
#if QT_VERSION >= QT_VERSION_CHECK(5, 6, 0)
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

    // 创建 Qt 应用程序对象
    // 注意：QApplication 必须在所有 QWidget 之前创建
    QApplication app(argc, argv);
    app.setApplicationName(QStringLiteral("UAV Ground Station"));
    app.setApplicationVersion(QStringLiteral("1.0.0"));
    app.setOrganizationName(QStringLiteral("UAV-GCS"));

    // 创建主窗口（在栈上，生命周期与 app 一致）
    MainWindow mainWindow;
    if (!mainWindow.initialize()) {
        qCritical() << "主窗口初始化失败，程序退出";
        return -1;
    }

    mainWindow.show();

    // 进入 Qt 事件循环 —— 此后所有代码由信号与槽驱动
    return app.exec();
}
