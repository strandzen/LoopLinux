kconfig_add_kcfg_files(loop_lite_kcfg_SRCS loopliteconfig.kcfgc)

kcoreaddons_add_plugin(kwin_effect_loop_lite
    SOURCES
        loopliteeffect.cpp
        snapcalculator.cpp
        ${loop_lite_kcfg_SRCS}
    INSTALL_NAMESPACE "kwin/effects/plugins"
)

qt6_add_resources(kwin_effect_loop_lite loop_lite_qml
    PREFIX "/qml"
    BASE "qml"
    FILES
        qml/Outline.qml
        qml/Indicator.qml
)

target_link_libraries(kwin_effect_loop_lite PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Qml
    Qt6::Quick
    KF6::ConfigCore
    KF6::ConfigGui
    KF6::CoreAddons
    KF6::GlobalAccel
    KWin::kwin
)
