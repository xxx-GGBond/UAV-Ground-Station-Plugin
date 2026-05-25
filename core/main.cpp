/**
 * @brief 地面站程序入口
 *
 * 启动顺序：QApplication → MainWindow(init→PluginManager/LinkManager/MavlinkService) → show → exec
 */
#include <QApplication>
#include "MainWindow.h"

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName(QStringLiteral("UAV Ground Station"));

    MainWindow mainWindow;
    if (!mainWindow.initialize()) {
        return -1;
    }

    mainWindow.show();
    return app.exec();
}
