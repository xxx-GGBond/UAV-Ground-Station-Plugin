#include "UAVParameterPlugin.h"

#include <QLabel>
#include <QVBoxLayout>
#include <QDebug>

UAVParameterPlugin::UAVParameterPlugin(QObject* parent)
    : QObject(parent)
{
}

UAVParameterPlugin::~UAVParameterPlugin()
{
    shutdown();
}

// ── IPlugin 接口实现 ────────────────────────────

bool UAVParameterPlugin::initialize()
{
    // 创建一个简单的占位窗口
    m_widget = new QWidget(nullptr);
    m_widget->setWindowTitle(QStringLiteral("参数插件"));
    m_widget->resize(400, 300);

    auto* layout = new QVBoxLayout(m_widget);
    auto* label  = new QLabel(QStringLiteral("测试"), m_widget);
    label->setAlignment(Qt::AlignCenter);
    QFont font = label->font();
    font.setPointSize(16);
    label->setFont(font);
    layout->addWidget(label);

    m_widget->show();

    qDebug() << "[UAVParameterPlugin] 初始化完成";
    return true;
}

void UAVParameterPlugin::shutdown()
{
    if (m_widget) {
        m_widget->close();
        delete m_widget;
        m_widget = nullptr;
    }
    qDebug() << "[参数插件测试] 已关闭";
}

QString UAVParameterPlugin::pluginId() const
{
    return QStringLiteral("com.uav.parameter");
}

QString UAVParameterPlugin::pluginName() const
{
    return QStringLiteral("参数插件");
}

// ── IParameterProvider 接口实现（占位） ────────

void UAVParameterPlugin::requestList()
{
    qDebug() << "[UAVParameterPlugin] requestList —— 暂未实现";
}

void UAVParameterPlugin::setParam(const QString& id, const QVariant& value)
{
    qDebug() << "[UAVParameterPlugin] setParam:" << id << value << "—— 暂未实现";
}

QVariant UAVParameterPlugin::paramValue(const QString& id) const
{
    Q_UNUSED(id);
    return QVariant();
}
