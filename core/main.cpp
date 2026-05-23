/**
 * @brief 地面站入口
 *
 * 启动流程：
 *   QApplication → MainWindow → PluginManager → 加载插件
 */
#include <QApplication>
#include <QDebug>
#include "MainWindow.h"

int main(int argc, char* argv[])
{
#if QT_VERSION >= QT_VERSION_CHECK(5, 6, 0)
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

    QApplication app(argc, argv);
    app.setApplicationName(QStringLiteral("UAV Ground Station"));

    MainWindow mainWindow;
    if (!mainWindow.initialize()) {
        qCritical() << "初始化失败，退出";
        return -1;
    }

    mainWindow.show();
    return app.exec();
}
