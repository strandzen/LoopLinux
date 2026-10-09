#include "sirkelconfigmodule.h"
#include "sirkelconfig.h"

#include <KConfigGroup>
#include <KPluginFactory>

#include <QGuiApplication>
#include <QPalette>

namespace
{
// See resolveOutlineColor() in sirkeleffect.cpp — same "never explicitly
// saved yet" check, duplicated here (rather than shared) because this is a
// separate process (systemsettings/kcmshell6, not kwin_wayland) with its
// own copy of SirkelConfig::self().
QColor resolveOutlineColor()
{
    const bool hasCustomColor = SirkelConfig::self()->config()->group(QStringLiteral("Effect-sirkel")).hasKey(QStringLiteral("OutlineColor"));
    if (!hasCustomColor) {
        return QGuiApplication::palette().color(QPalette::Highlight);
    }
    return SirkelConfig::outlineColor();
}
}

SirkelConfigModule::SirkelConfigModule(QObject *parent, const KPluginMetaData &metaData)
    : KQuickConfigModule(parent, metaData)
{
    setButtons(KAbstractConfigModule::Default);
}

int SirkelConfigModule::horizontalDeadzone() const
{
    return SirkelConfig::horizontalDeadzone();
}

void SirkelConfigModule::setHorizontalDeadzone(int value)
{
    SirkelConfig::setHorizontalDeadzone(value);
    SirkelConfig::self()->save();
    Q_EMIT settingsChanged();
}

int SirkelConfigModule::verticalDeadzone() const
{
    return SirkelConfig::verticalDeadzone();
}

void SirkelConfigModule::setVerticalDeadzone(int value)
{
    SirkelConfig::setVerticalDeadzone(value);
    SirkelConfig::self()->save();
    Q_EMIT settingsChanged();
}

int SirkelConfigModule::paddingHorizontal() const
{
    return SirkelConfig::paddingHorizontal();
}

void SirkelConfigModule::setPaddingHorizontal(int value)
{
    SirkelConfig::setPaddingHorizontal(value);
    SirkelConfig::self()->save();
    Q_EMIT settingsChanged();
}

int SirkelConfigModule::paddingVertical() const
{
    return SirkelConfig::paddingVertical();
}

void SirkelConfigModule::setPaddingVertical(int value)
{
    SirkelConfig::setPaddingVertical(value);
    SirkelConfig::self()->save();
    Q_EMIT settingsChanged();
}

int SirkelConfigModule::indicatorSize() const
{
    return SirkelConfig::indicatorSize();
}

void SirkelConfigModule::setIndicatorSize(int value)
{
    SirkelConfig::setIndicatorSize(value);
    SirkelConfig::self()->save();
    Q_EMIT settingsChanged();
}

int SirkelConfigModule::indicatorCornerRadius() const
{
    return SirkelConfig::indicatorCornerRadius();
}

void SirkelConfigModule::setIndicatorCornerRadius(int value)
{
    SirkelConfig::setIndicatorCornerRadius(value);
    SirkelConfig::self()->save();
    Q_EMIT settingsChanged();
}

int SirkelConfigModule::indicatorRingWidth() const
{
    return SirkelConfig::indicatorRingWidth();
}

void SirkelConfigModule::setIndicatorRingWidth(int value)
{
    SirkelConfig::setIndicatorRingWidth(value);
    SirkelConfig::self()->save();
    Q_EMIT settingsChanged();
}

int SirkelConfigModule::indicatorPointerLength() const
{
    return SirkelConfig::indicatorPointerLength();
}

void SirkelConfigModule::setIndicatorPointerLength(int value)
{
    SirkelConfig::setIndicatorPointerLength(value);
    SirkelConfig::self()->save();
    Q_EMIT settingsChanged();
}

bool SirkelConfigModule::showIndicatorText() const
{
    return SirkelConfig::showIndicatorText();
}

void SirkelConfigModule::setShowIndicatorText(bool value)
{
    SirkelConfig::setShowIndicatorText(value);
    SirkelConfig::self()->save();
    Q_EMIT settingsChanged();
}

int SirkelConfigModule::outlineBorderWidth() const
{
    return SirkelConfig::outlineBorderWidth();
}

void SirkelConfigModule::setOutlineBorderWidth(int value)
{
    SirkelConfig::setOutlineBorderWidth(value);
    SirkelConfig::self()->save();
    Q_EMIT settingsChanged();
}

int SirkelConfigModule::outlineCornerRadius() const
{
    return SirkelConfig::outlineCornerRadius();
}

void SirkelConfigModule::setOutlineCornerRadius(int value)
{
    SirkelConfig::setOutlineCornerRadius(value);
    SirkelConfig::self()->save();
    Q_EMIT settingsChanged();
}

QColor SirkelConfigModule::outlineColor() const
{
    return resolveOutlineColor();
}

void SirkelConfigModule::setOutlineColor(const QColor &value)
{
    SirkelConfig::setOutlineColor(value);
    SirkelConfig::self()->save();
    Q_EMIT settingsChanged();
}

int SirkelConfigModule::outlineFillOpacity() const
{
    return SirkelConfig::outlineFillOpacity();
}

void SirkelConfigModule::setOutlineFillOpacity(int value)
{
    SirkelConfig::setOutlineFillOpacity(value);
    SirkelConfig::self()->save();
    Q_EMIT settingsChanged();
}

int SirkelConfigModule::outlineAnimationDuration() const
{
    return SirkelConfig::outlineAnimationDuration();
}

void SirkelConfigModule::setOutlineAnimationDuration(int value)
{
    SirkelConfig::setOutlineAnimationDuration(value);
    SirkelConfig::self()->save();
    Q_EMIT settingsChanged();
}

QString SirkelConfigModule::keybindScheme() const
{
    return SirkelConfig::keybindScheme();
}

void SirkelConfigModule::setKeybindScheme(const QString &value)
{
    SirkelConfig::setKeybindScheme(value);
    SirkelConfig::self()->save();
    Q_EMIT settingsChanged();
}

QStringList SirkelConfigModule::keybindSchemes() const
{
    return {QStringLiteral("arrows"), QStringLiteral("wasd"), QStringLiteral("hjkl"), QStringLiteral("none")};
}

bool SirkelConfigModule::showIndicator() const
{
    return SirkelConfig::showIndicator();
}

void SirkelConfigModule::setShowIndicator(bool value)
{
    SirkelConfig::setShowIndicator(value);
    SirkelConfig::self()->save();
    Q_EMIT settingsChanged();
}

bool SirkelConfigModule::showOutline() const
{
    return SirkelConfig::showOutline();
}

void SirkelConfigModule::setShowOutline(bool value)
{
    SirkelConfig::setShowOutline(value);
    SirkelConfig::self()->save();
    Q_EMIT settingsChanged();
}

bool SirkelConfigModule::indicatorFollowsMouse() const
{
    return SirkelConfig::indicatorFollowsMouse();
}

void SirkelConfigModule::setIndicatorFollowsMouse(bool value)
{
    SirkelConfig::setIndicatorFollowsMouse(value);
    SirkelConfig::self()->save();
    Q_EMIT settingsChanged();
}

void SirkelConfigModule::load()
{
    Q_EMIT settingsChanged();
}

void SirkelConfigModule::save()
{
    SirkelConfig::self()->save();
}

void SirkelConfigModule::defaults()
{
    SirkelConfig::self()->setDefaults();
    SirkelConfig::self()->save();
    Q_EMIT settingsChanged();
}

K_PLUGIN_CLASS_WITH_JSON(SirkelConfigModule, "kwin_effect_sirkel_config.json")

#include "sirkelconfigmodule.moc"
