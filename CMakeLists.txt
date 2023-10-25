cmake_minimum_required(VERSION 3.16)
project(VST3Test VERSION 0.0.1 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)

add_library(${PROJECT_NAME} MODULE src/plugin.cpp)

target_include_directories(${PROJECT_NAME} PRIVATE
    modules/vst3sdk
    modules/vst3sdk/vstgui4
    )

set(COPY_AFTER_BUILD TRUE)

if (APPLE)
    target_link_libraries(${PROJECT_NAME} PRIVATE "-framework CoreFoundation")

    set(CMAKE_OSX_DEPLOYMENT_TARGET "10.11" CACHE STRING "Minimum OS X deployment target")
    if (${CMAKE_BUILD_TYPE} MATCHES Release)
        # If running a DAW through Rosetta on an Apple Silicon mac, the plugin must also support x86
        # eg. Abelton 10 (Intel only) won't show plugins only built for Apple Silicon
        set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64")
    elseif(${CMAKE_BUILD_TYPE} MATCHES Debug)
        # Apple clang doesn't add this for whatever reason...?
        # Required by vst3sdk/base/source/fdebug.h
        target_compile_definitions(${PROJECT_NAME} PRIVATE _DEBUG)
    endif()

    set_target_properties(${PROJECT_NAME} PROPERTIES
        BUNDLE True
        BUNDLE_EXTENSION vst3
        MACOSX_BUNDLE_BUNDLE_NAME ${PROJECT_NAME}
        MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
        MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION}
        MACOSX_BUNDLE_LONG_VERSION_STRING ${PROJECT_VERSION}
        MACOSX_BUNDLE_INFO_STRING ${PROJECT_VERSION}
        MACOSX_BUNDLE_INFO_PLIST ${CMAKE_SOURCE_DIR}/misc/info.plist

        MACOSX_BUNDLE_GUI_IDENTIFIER com.deeznuts.${PROJECT_NAME}.vst3
        MACOSX_BUNDLE_COPYRIGHT "Deez Nuts Ltd."
        )

    if (${CMAKE_BUILD_TYPE} MATCHES Release)
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E echo "Signing bundle at ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.vst3"
            COMMAND xcrun codesign -s - "${CMAKE_BINARY_DIR}/${PROJECT_NAME}.vst3")
    endif()

    if (${COPY_AFTER_BUILD})
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E echo "Installing ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.vst3 to ~/Library/Audio/Plug-Ins/VST3/"
            COMMAND ${CMAKE_COMMAND} -E make_directory "~/Library/Audio/Plug-Ins/VST3"
            COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_BINARY_DIR}/${PROJECT_NAME}.vst3" "~/Library/Audio/Plug-Ins/VST3/${PROJECT_NAME}.vst3"
            )
    endif()
endif()