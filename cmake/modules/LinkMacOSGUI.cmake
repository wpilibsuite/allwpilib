set(WPILIB_MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_LIST_DIR}/WPILibMacOSXBundleInfo.plist.in")

macro(wpilib_link_macos_gui target)
    if(APPLE)
        set_target_properties(
            ${target}
            PROPERTIES
                LINK_FLAGS "-framework Metal -framework QuartzCore"
                MACOSX_BUNDLE_INFO_PLIST "${WPILIB_MACOSX_BUNDLE_INFO_PLIST}"
        )
    endif()
endmacro()
