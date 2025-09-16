# SPDX-License-Identifier: GPL-3.0-only

add_library(map_downloader STATIC
    src/map_downloader/map_downloader.cpp
)

add_executable(hac_map_downloader_test
    src/map_downloader/test/test.cpp
)

add_dependencies(hac_map_downloader_test map_downloader)
target_link_libraries(hac_map_downloader_test map_downloader curl ws2_32 bcrypt crypt32)
set_target_properties(hac_map_downloader_test PROPERTIES LINK_FLAGS "-m32 -static-libgcc -static-libstdc++ -static -lwinpthread")
