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

    if (${CMAKE_BUILD_TYPE} MATCHES Debug)
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

        MACOSX_BUNDLE_GUI_IDENTIFIER org.deez.nuts.${PROJECT_NAME}.vst3
        MACOSX_BUNDLE_COPYRIGHT "Deez Nuts Ltd."
        )

    if (${COPY_AFTER_BUILD})
        message(STATUS "Will copy plugin after every build")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E echo "Installing ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.vst3 to ~/Library/Audio/Plug-Ins/VST3/"
            COMMAND ${CMAKE_COMMAND} -E make_directory "~/Library/Audio/Plug-Ins/VST3"
            COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_BINARY_DIR}/${PROJECT_NAME}.vst3" "~/Library/Audio/Plug-Ins/VST3/${PROJECT_NAME}.vst3"
            )
    endif()
endif()