include_directories(${CMAKE_CURRENT_BINARY_DIR} )

set(kritaDither_PART_SRCS Dither.cc  DitherConfigurationWidget.cc)
kde3_add_ui_files(kritaDither_PART_SRCS
    DitherConfigurationBaseWidget.ui
    )

kde3_automoc(${kritaDither_PART_SRCS})

kde3_add_kpart(kritaDither ${kritaDither_PART_SRCS})

target_link_libraries(kritaDither ${KRITAUI_LIBRARY} )

install(TARGETS kritaDither  DESTINATION ${PLUGIN_INSTALL_DIR})

install( FILES  kritaDither.desktop  DESTINATION ${SERVICES_INSTALL_DIR})
