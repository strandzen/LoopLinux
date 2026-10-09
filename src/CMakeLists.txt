kconfig_add_kcfg_files(sirkel_kcfg_SRCS GENERATE_MOC sirkelconfig.kcfgc)

kcoreaddons_add_plugin(kwin_effect_sirkel
    SOURCES
        sirkeleffect.cpp
        snapcalculator.cpp
        ${sirkel_kcfg_SRCS}
    INSTALL_NAMESPACE "kwin/effects/plugins"
)

qt6_add_resources(kwin_effect_sirkel sirkel_qml
    PREFIX "/qml"
    BASE "qml"
    FILES
        qml/Outline.qml
        qml/Indicator.qml
)

target_link_libraries(kwin_effect_sirkel PRIVATE
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

add_subdirectory(kcm)
