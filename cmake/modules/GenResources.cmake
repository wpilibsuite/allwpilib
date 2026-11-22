set(scripts_dir "${CMAKE_CURRENT_LIST_DIR}/../scripts")
macro(
    generate_resources
    inputDir
    outputDir
    prefix
    namespace
    outputFiles
)
    if(${ARGC} GREATER 5)
        set(inputFiles)
        foreach(pattern ${ARGN})
            file(GLOB_RECURSE patternFiles CONFIGURE_DEPENDS ${inputDir}/${pattern})
            list(APPEND inputFiles ${patternFiles})
        endforeach()
    else()
        file(GLOB inputFiles ${inputDir}/*)
    endif()
    set(${outputFiles})
    foreach(input ${inputFiles})
        get_filename_component(inputBase ${input} NAME)
        if("${inputBase}" MATCHES "^\\.")
            continue()
        endif()
        set(output "${outputDir}/${inputBase}.cpp")
        list(APPEND ${outputFiles} "${output}")

        add_custom_command(
            OUTPUT ${output}
            COMMAND
                ${CMAKE_COMMAND} "-Dinput=${input}" "-Doutput=${output}" "-Dprefix=${prefix}"
                "-Dnamespace=${namespace}" -P "${scripts_dir}/GenResource.cmake"
            MAIN_DEPENDENCY ${input}
            DEPENDS ${scripts_dir}/GenResource.cmake
            VERBATIM
        )
    endforeach()
endmacro()
