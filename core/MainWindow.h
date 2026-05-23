#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <memory>

class PluginManager;
class IParameterProvider;

/**
 * @brief 主窗口 —— 地面站宿主程序的主界面
 *
 * 只负责：
 * 1. 启动时加载插件
 * 2. 提供菜单栏、状态栏等框架 UI
 * 3. 管理插件的生命周期
 *
 * 所有业务逻辑（如参数读写）由插件实现，主窗口不包含业务代码。
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override;

    bool initialize();

protected:
    void closeEvent(QCloseEvent* event) override;

private:
    std::unique_ptr<PluginManager> m_pluginMgr;
};

#endif // MAINWINDOW_H
