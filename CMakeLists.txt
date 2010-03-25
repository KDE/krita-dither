set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules )

find_package(KDE4 REQUIRED)
include(KDE4Defaults)
include(MacroLibrary)
find_package(Krita REQUIRED)
find_package(KOffice REQUIRED)

set( KRITA_IMAGE_LIBS ${KRITAIMAGE_LIBRARY} ${KDE4_KDECORE_LIBS} ${QT_QTCORE_LIBRARY} pigmentcms komain ${KDE4_THREADWEAVER_LIBRARIES} )
set( KRITA_UI_LIBS ${KDE4_KDEUI_LIBS} ${QT_QTGUI_LIBRARY}  ${KDE4_KPARTS_LIBS} ${KDE4_KDE3SUPPORT_LIBS} ${BLITZ_LIBRARIES} ${KRITA_IMAGE_LIBS} kotext ${PNG_LIBRARIES} ${KRITAUI_LIBRARY})

include_directories (${KDE4_INCLUDES} ${CMAKE_CURRENT_BINARY_DIR} ${KOFFICE_INCLUDE_DIR} ${EIGEN2_INCLUDE_DIR})

set(kritaDither_PART_SRCS Dither.cc  DitherConfigurationWidget.cc)
kde4_add_ui_files(kritaDither_PART_SRCS
    DitherConfigurationBaseWidget.ui
    )

kde4_add_plugin(kritaDither ${kritaDither_PART_SRCS})

target_link_libraries(kritaDither ${KRITA_UI_LIBS} )

install(TARGETS kritaDither  DESTINATION ${PLUGIN_INSTALL_DIR})

install( FILES  kritaDither.desktop  DESTINATION ${SERVICES_INSTALL_DIR})
