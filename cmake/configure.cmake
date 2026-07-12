set(CONFIG_INSTALL_DIR ${CMAKE_INSTALL_LIBDIR}/cmake/libnsocket)

configure_package_config_file(
    "${CMAKE_SOURCE_DIR}/cmake/libnsocketConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/libnsocketConfig.cmake"
    INSTALL_DESTINATION ${CONFIG_INSTALL_DIR}
)

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/libnsocketConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY ${VERSION_CHECK_METHOD}
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/libnsocketConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/libnsocketConfigVersion.cmake"
    DESTINATION ${CONFIG_INSTALL_DIR}
)