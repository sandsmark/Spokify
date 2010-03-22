project(spokify)
 
find_package(KDE4 REQUIRED)
include_directories(${KDE4_INCLUDES})
 
set(spokify_SRCS
    login.cpp
    main.cpp
    mainwindow.cpp)
 
kde4_add_executable(spokify ${spokify_SRCS})
 
target_link_libraries(spokify ${KDE4_KDEUI_LIBS} spotify phonon)
 
install(TARGETS spokify DESTINATION ${BIN_INSTALL_DIR})
install(FILES spokifyui.rc DESTINATION ${DATA_INSTALL_DIR}/spokify)
install(FILES images/spotify-core-logo-128x128.png DESTINATION ${DATA_INSTALL_DIR}/spokify/images)
