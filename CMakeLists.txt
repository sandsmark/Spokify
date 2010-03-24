project(spokify)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMake/Modules/")
 
find_package(KDE4 REQUIRED)
find_package(libspotify REQUIRED)
include_directories(${KDE4_INCLUDES} ${LIBSPOTIFY_INCLUDE_DIR})
 
set(spokify_SRCS
    alsa-audio.cpp
    login.cpp
    main.cpp
    mainwidget.cpp
    mainwindow.cpp
    playlistmodel.cpp
    proxymodel.cpp
    tabledelegate.cpp
    trackmodel.cpp)
 
kde4_add_executable(spokify ${spokify_SRCS})
 
target_link_libraries(spokify ${KDE4_KDEUI_LIBS} ${LIBSPOTIFY_LIBRARIES} phonon asound)
 
install(TARGETS spokify DESTINATION ${BIN_INSTALL_DIR})
install(FILES spokifyui.rc DESTINATION ${DATA_INSTALL_DIR}/spokify)
install(FILES images/spotify-core-logo-128x128.png DESTINATION ${DATA_INSTALL_DIR}/spokify/images)
