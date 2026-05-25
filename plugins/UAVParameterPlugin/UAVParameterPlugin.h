#ifndef UAVPARAMETERPLUGIN_H
#define UAVPARAMETERPLUGIN_H

#include <QObject>
#include <QWidget>

#include "interfaces/IPlugin.h"
#include "interfaces/IParameterProvider.h"

/**
 * @brief 参数读写插件 —— 示例插件
 *
 * 同时实现 IPlugin（用于被宿主加载）和 IParameterProvider（参数读写）。
 * 当前阶段仅显示一个 QWidget，文本为"参数插件已加载"。
 */
class UAVParameterPlugin : public QObject,
                           public IPlugin,
                           public IParameterProvider
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID IPLUGIN_IID FILE "parameter.json")
    Q_INTERFACES(IPlugin IParameterProvider)

public:
    explicit UAVParameterPlugin(QObject* parent = nullptr);
    ~UAVParameterPlugin() override;

    // ── IPlugin 接口 ──
    bool    initialize() override;
    void    shutdown() override;
    QString pluginId() const override;
    QString pluginName() const override;

    // ── IParameterProvider 接口（占位） ──
    void     requestList() override;
    void     setParam(const QString& id, const QVariant& value) override;
    QVariant paramValue(const QString& id) const override;

private:
    QWidget* m_widget = nullptr;   ///< 插件显示的占位窗口
};

#endif // UAVPARAMETERPLUGIN_H
