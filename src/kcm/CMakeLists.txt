kcmutils_add_qml_kcm(kwin_effect_sirkel_config
    SOURCES
        sirkelconfigmodule.cpp
        ${sirkel_kcfg_SRCS}
    INSTALL_NAMESPACE "kwin/effects/configs"
    DISABLE_DESKTOP_FILE_GENERATION
)

target_include_directories(kwin_effect_sirkel_config PRIVATE ${CMAKE_BINARY_DIR}/src)

target_link_libraries(kwin_effect_sirkel_config PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Qml
    KF6::ConfigCore
    KF6::ConfigGui
    KF6::CoreAddons
    KF6::KCMUtils
    KF6::KCMUtilsQuick
)
