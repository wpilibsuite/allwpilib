function(wpilib_config)
    cmake_parse_arguments(config "" "" "OPTIONS;REQUIRES" ${ARGN})
    foreach(opt ${config_OPTIONS})
        if(NOT ${opt})
            return()
        endif()
    endforeach()
    foreach(required_opt ${config_REQUIRES})
        if(NOT ${required_opt})
            list(JOIN config_OPTIONS " and " options_list)
            list(LENGTH config_OPTIONS option_len)
            if(option_len GREATER 1)
                set(requires require)
                set(option_msg "one of the listed options")
            else()
                set(requires requires)
                set(option_msg ${options_list})
            endif()

            message(
                FATAL_ERROR
                "
FATAL: ${options_list} ${requires} ${required_opt}.
       Either enable ${required_opt} or disable ${option_msg}.
"
            )
        endif()
    endforeach()
endfunction()
