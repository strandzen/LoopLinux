#include "loopliteconfigmodule.h"
#include "loopliteconfig.h"

#include <KPluginFactory>

LoopLiteConfigModule::LoopLiteConfigModule(QObject *parent, const KPluginMetaData &metaData)
    : KQuickConfigModule(parent, metaData)
{
    setButtons(KAbstractConfigModule::Default);
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

int LoopLiteConfigModule::paddingHorizontal() const
{
    return LoopLiteConfig::paddingHorizontal();
}

void LoopLiteConfigModule::setPaddingHorizontal(int value)
{
    LoopLiteConfig::setPaddingHorizontal(value);
    LoopLiteConfig::self()->save();
    Q_EMIT settingsChanged();
}

int LoopLiteConfigModule::paddingVertical() const
{
    return LoopLiteConfig::paddingVertical();
}

void LoopLiteConfigModule::setPaddingVertical(int value)
{
    LoopLiteConfig::setPaddingVertical(value);
    LoopLiteConfig::self()->save();
    Q_EMIT settingsChanged();
}

int LoopLiteConfigModule::indicatorSize() const
{
    return LoopLiteConfig::indicatorSize();
}

void LoopLiteConfigModule::setIndicatorSize(int value)
{
    LoopLiteConfig::setIndicatorSize(value);
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

int LoopLiteConfigModule::indicatorPointerLength() const
{
    return LoopLiteConfig::indicatorPointerLength();
}

void LoopLiteConfigModule::setIndicatorPointerLength(int value)
{
    LoopLiteConfig::setIndicatorPointerLength(value);
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

int LoopLiteConfigModule::outlineFillOpacity() const
{
    return LoopLiteConfig::outlineFillOpacity();
}

void LoopLiteConfigModule::setOutlineFillOpacity(int value)
{
    LoopLiteConfig::setOutlineFillOpacity(value);
    LoopLiteConfig::self()->save();
    Q_EMIT settingsChanged();
}

int LoopLiteConfigModule::outlineAnimationDuration() const
{
    return LoopLiteConfig::outlineAnimationDuration();
}

void LoopLiteConfigModule::setOutlineAnimationDuration(int value)
{
    LoopLiteConfig::setOutlineAnimationDuration(value);
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
