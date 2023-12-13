macro(wpilib_link_macos_gui target)
    if(APPLE)
        set_target_properties(
            ${target}
            PROPERTIES LINK_FLAGS "-framework Metal -framework QuartzCore"
        )
    endif()
endmacro()
