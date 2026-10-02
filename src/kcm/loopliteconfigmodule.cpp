#include "loopliteconfigmodule.h"
#include "loopliteconfig.h"

#include <KPluginFactory>

LoopLiteConfigModule::LoopLiteConfigModule(QObject *parent, const KPluginMetaData &metaData)
    : KQuickConfigModule(parent, metaData)
{
    setButtons(KAbstractConfigModule::Default);
}

bool LoopLiteConfigModule::enableTopBottomHalves() const
{
    return LoopLiteConfig::enableTopBottomHalves();
}

void LoopLiteConfigModule::setEnableTopBottomHalves(bool value)
{
    LoopLiteConfig::setEnableTopBottomHalves(value);
    LoopLiteConfig::self()->save();
    Q_EMIT settingsChanged();
}

int LoopLiteConfigModule::horizontalDeadzone() const
{
    return LoopLiteConfig::horizontalDeadzone();
}

void LoopLiteConfigModule::setHorizontalDeadzone(int value)
{
    LoopLiteConfig::setHorizontalDeadzone(value);
    LoopLiteConfig::self()->save();
    Q_EMIT settingsChanged();
}

int LoopLiteConfigModule::verticalDeadzone() const
{
    return LoopLiteConfig::verticalDeadzone();
}

void LoopLiteConfigModule::setVerticalDeadzone(int value)
{
    LoopLiteConfig::setVerticalDeadzone(value);
    LoopLiteConfig::self()->save();
    Q_EMIT settingsChanged();
}

int LoopLiteConfigModule::windowPadding() const
{
    return LoopLiteConfig::windowPadding();
}

void LoopLiteConfigModule::setWindowPadding(int value)
{
    LoopLiteConfig::setWindowPadding(value);
    LoopLiteConfig::self()->save();
    Q_EMIT settingsChanged();
}

int LoopLiteConfigModule::indicatorCornerRadius() const
{
    return LoopLiteConfig::indicatorCornerRadius();
}

void LoopLiteConfigModule::setIndicatorCornerRadius(int value)
{
    LoopLiteConfig::setIndicatorCornerRadius(value);
    LoopLiteConfig::self()->save();
    Q_EMIT settingsChanged();
}

int LoopLiteConfigModule::indicatorRingWidth() const
{
    return LoopLiteConfig::indicatorRingWidth();
}

void LoopLiteConfigModule::setIndicatorRingWidth(int value)
{
    LoopLiteConfig::setIndicatorRingWidth(value);
    LoopLiteConfig::self()->save();
    Q_EMIT settingsChanged();
}

bool LoopLiteConfigModule::showIndicatorText() const
{
    return LoopLiteConfig::showIndicatorText();
}

void LoopLiteConfigModule::setShowIndicatorText(bool value)
{
    LoopLiteConfig::setShowIndicatorText(value);
    LoopLiteConfig::self()->save();
    Q_EMIT settingsChanged();
}

int LoopLiteConfigModule::outlineBorderWidth() const
{
    return LoopLiteConfig::outlineBorderWidth();
}

void LoopLiteConfigModule::setOutlineBorderWidth(int value)
{
    LoopLiteConfig::setOutlineBorderWidth(value);
    LoopLiteConfig::self()->save();
    Q_EMIT settingsChanged();
}

int LoopLiteConfigModule::outlineCornerRadius() const
{
    return LoopLiteConfig::outlineCornerRadius();
}

void LoopLiteConfigModule::setOutlineCornerRadius(int value)
{
    LoopLiteConfig::setOutlineCornerRadius(value);
    LoopLiteConfig::self()->save();
    Q_EMIT settingsChanged();
}

QColor LoopLiteConfigModule::outlineColor() const
{
    return LoopLiteConfig::outlineColor();
}

void LoopLiteConfigModule::setOutlineColor(const QColor &value)
{
    LoopLiteConfig::setOutlineColor(value);
    LoopLiteConfig::self()->save();
    Q_EMIT settingsChanged();
}

QString LoopLiteConfigModule::keybindScheme() const
{
    return LoopLiteConfig::keybindScheme();
}

void LoopLiteConfigModule::setKeybindScheme(const QString &value)
{
    LoopLiteConfig::setKeybindScheme(value);
    LoopLiteConfig::self()->save();
    Q_EMIT settingsChanged();
}

QStringList LoopLiteConfigModule::keybindSchemes() const
{
    return {QStringLiteral("arrows"), QStringLiteral("wasd"), QStringLiteral("hjkl")};
}

bool LoopLiteConfigModule::showIndicator() const
{
    return LoopLiteConfig::showIndicator();
}

void LoopLiteConfigModule::setShowIndicator(bool value)
{
    LoopLiteConfig::setShowIndicator(value);
    LoopLiteConfig::self()->save();
    Q_EMIT settingsChanged();
}

bool LoopLiteConfigModule::showOutline() const
{
    return LoopLiteConfig::showOutline();
}

void LoopLiteConfigModule::setShowOutline(bool value)
{
    LoopLiteConfig::setShowOutline(value);
    LoopLiteConfig::self()->save();
    Q_EMIT settingsChanged();
}

bool LoopLiteConfigModule::indicatorFollowsMouse() const
{
    return LoopLiteConfig::indicatorFollowsMouse();
}

void LoopLiteConfigModule::setIndicatorFollowsMouse(bool value)
{
    LoopLiteConfig::setIndicatorFollowsMouse(value);
    LoopLiteConfig::self()->save();
    Q_EMIT settingsChanged();
}

void LoopLiteConfigModule::load()
{
    Q_EMIT settingsChanged();
}

void LoopLiteConfigModule::save()
{
    LoopLiteConfig::self()->save();
}

void LoopLiteConfigModule::defaults()
{
    LoopLiteConfig::self()->setDefaults();
    LoopLiteConfig::self()->save();
    Q_EMIT settingsChanged();
}

K_PLUGIN_CLASS_WITH_JSON(LoopLiteConfigModule, "kwin_effect_loop_lite_config.json")

#include "loopliteconfigmodule.moc"
