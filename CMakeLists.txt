project(spokify)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMake/Modules/")
add_definitions(-fexceptions)
 
find_package(KDE4 REQUIRED)
find_package(libspotify REQUIRED)
find_package(LibLastFm REQUIRED)
include_directories(${KDE4_INCLUDES} ${LIBSPOTIFY_INCLUDE_DIR} ${LIBLASTFM_INCLUDE_DIR})
 
set(spokify_SRCS
    login.cpp
    main.cpp
    mainwidget.cpp
    mainwindow.cpp
    playlistmodel.cpp
    soundfeeder.cpp
    trackmodel.cpp
    playpausebutton.cpp
    coverlabel.cpp
    trackviewdelegate.cpp
    trackview.cpp
    scrobblingsettingsdialog.cpp
    scrobbler.cpp
    slider.cpp
    searchhistorymodel.cpp
    mimedata.cpp
    playlistview.cpp)
 
kde4_add_executable(spokify ${spokify_SRCS})
 
target_link_libraries(spokify ${KDE4_KDEUI_LIBS} ${LIBSPOTIFY_LIBRARIES} ${LIBLASTFM_LIBRARY} asound)
 
install(TARGETS spokify DESTINATION ${BIN_INSTALL_DIR})
install(FILES spokifyui.rc
              spokify.notifyrc DESTINATION ${DATA_INSTALL_DIR}/spokify)
install(FILES images/spotify-core-logo-128x128.png
              images/nocover-200x200.png
              images/nocover.svg
              images/cover-loading.gif
              images/media-playback-pause.png
              images/media-playback-start.png
              images/media-playback-stop.png
              images/media-seek-backward.png
              images/media-seek-forward.png
              images/media-skip-backward.png
              images/media-skip-forward.png
              images/slider_left.png
              images/slider_right.png
              images/slider_body_background.png
              images/slider_body_left.png
              images/slider_body_right.png
              images/slider_body.png
              images/dialog-information.png
              images/searching.png DESTINATION ${DATA_INSTALL_DIR}/spokify/images)
install(FILES spokify.desktop DESTINATION ${XDG_APPS_INSTALL_DIR})
