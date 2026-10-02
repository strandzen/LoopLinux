#pragma once

#include <KQuickConfigModule>

#include <QColor>
#include <QStringList>

// System Settings page for Loop Lite, under Desktop Effects' configure
// button for the effect (wired via metadata.json's X-KDE-ConfigModule).
// Every property here is a thin, uncached read/write pass-through to
// LoopLiteConfig::self() — "instant apply" (writes + saves immediately on
// change), per the documented KQuickConfigModule pattern of exposing
// settings as plain properties on the module itself, accessed from QML via
// the framework-provided "kcm" object.
class LoopLiteConfigModule : public KQuickConfigModule
{
    Q_OBJECT

    Q_PROPERTY(bool enableTopBottomHalves READ enableTopBottomHalves WRITE setEnableTopBottomHalves NOTIFY settingsChanged)
    Q_PROPERTY(int horizontalDeadzone READ horizontalDeadzone WRITE setHorizontalDeadzone NOTIFY settingsChanged)
    Q_PROPERTY(int verticalDeadzone READ verticalDeadzone WRITE setVerticalDeadzone NOTIFY settingsChanged)
    Q_PROPERTY(int windowPadding READ windowPadding WRITE setWindowPadding NOTIFY settingsChanged)
    Q_PROPERTY(int indicatorCornerRadius READ indicatorCornerRadius WRITE setIndicatorCornerRadius NOTIFY settingsChanged)
    Q_PROPERTY(int indicatorRingWidth READ indicatorRingWidth WRITE setIndicatorRingWidth NOTIFY settingsChanged)
    Q_PROPERTY(bool showIndicatorText READ showIndicatorText WRITE setShowIndicatorText NOTIFY settingsChanged)
    Q_PROPERTY(int outlineBorderWidth READ outlineBorderWidth WRITE setOutlineBorderWidth NOTIFY settingsChanged)
    Q_PROPERTY(int outlineCornerRadius READ outlineCornerRadius WRITE setOutlineCornerRadius NOTIFY settingsChanged)
    Q_PROPERTY(QColor outlineColor READ outlineColor WRITE setOutlineColor NOTIFY settingsChanged)
    Q_PROPERTY(QString keybindScheme READ keybindScheme WRITE setKeybindScheme NOTIFY settingsChanged)
    Q_PROPERTY(QStringList keybindSchemes READ keybindSchemes CONSTANT)
    Q_PROPERTY(bool showIndicator READ showIndicator WRITE setShowIndicator NOTIFY settingsChanged)
    Q_PROPERTY(bool showOutline READ showOutline WRITE setShowOutline NOTIFY settingsChanged)
    Q_PROPERTY(bool indicatorFollowsMouse READ indicatorFollowsMouse WRITE setIndicatorFollowsMouse NOTIFY settingsChanged)

public:
    explicit LoopLiteConfigModule(QObject *parent, const KPluginMetaData &metaData);

    bool enableTopBottomHalves() const;
    void setEnableTopBottomHalves(bool value);

    int horizontalDeadzone() const;
    void setHorizontalDeadzone(int value);

    int verticalDeadzone() const;
    void setVerticalDeadzone(int value);

    int windowPadding() const;
    void setWindowPadding(int value);

    int indicatorCornerRadius() const;
    void setIndicatorCornerRadius(int value);

    int indicatorRingWidth() const;
    void setIndicatorRingWidth(int value);

    bool showIndicatorText() const;
    void setShowIndicatorText(bool value);

    int outlineBorderWidth() const;
    void setOutlineBorderWidth(int value);

    int outlineCornerRadius() const;
    void setOutlineCornerRadius(int value);

    QColor outlineColor() const;
    void setOutlineColor(const QColor &value);

    // Stored value, not display label — presets intentionally fixed rather
    // than arbitrary rebinding, see Todo.md's keybinds item.
    QString keybindScheme() const;
    void setKeybindScheme(const QString &value);
    QStringList keybindSchemes() const;

    bool showIndicator() const;
    void setShowIndicator(bool value);

    bool showOutline() const;
    void setShowOutline(bool value);

    bool indicatorFollowsMouse() const;
    void setIndicatorFollowsMouse(bool value);

    void load() override;
    void save() override;
    void defaults() override;

Q_SIGNALS:
    void settingsChanged();
};
