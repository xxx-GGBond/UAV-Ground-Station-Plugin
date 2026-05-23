#ifndef IUICOMPONENT_H
#define IUICOMPONENT_H

#include <QWidget>
#include <QString>
#include <QJsonObject>

/**
 * @brief UI 组件接口 —— 所有可停靠面板/窗口插件的基础接口
 *
 * 定位：
 *   继承自 QWidget，使每个插件都是一个独立的 Qt 窗口部件，
 *   可以被嵌入到主窗口的 QDockWidget 或 QStackedWidget 中。
 *
 * 为什么需要这个接口？
 *   - 主窗口不需要知道插件的内部实现，只需要把它当作一个 QWidget 放置
 *   - 插件自己管理自己的布局和样式，主窗口只负责"容器"的角色
 *   - 符合 Qt 的 QWidget 体系，可以无缝使用 Qt 的布局和信号槽系统
 */
class IUiComponent : public QWidget
{
    Q_OBJECT

public:
    explicit IUiComponent(QWidget* parent = nullptr) : QWidget(parent) {}
    virtual ~IUiComponent() = default;

    /**
     * @brief 返回此 UI 组件在主窗口中的建议停靠区域
     * @return Qt::DockWidgetArea 枚举值对应的整数值
     *
     * 例如：返回 Qt::LeftDockWidgetArea 表示默认停靠在左侧
     */
    virtual int preferredDockArea() const = 0;

    /**
     * @brief 返回 UI 组件的标题（显示在停靠窗口标题栏上）
     */
    virtual QString windowTitle() const = 0;

    /**
     * @brief 保存当前 UI 状态（如列宽、展开状态等）
     * @return JSON 格式的状态数据
     */
    virtual QJsonObject saveState() const = 0;

    /**
     * @brief 恢复上次保存的 UI 状态
     * @param state 由 saveState() 生成的 JSON 数据
     */
    virtual void restoreState(const QJsonObject& state) = 0;
};

#endif // IUICOMPONENT_H
