set(SCRIPTS_DIR "${CMAKE_CURRENT_LIST_DIR}/../scripts")
MACRO(GENERATE_RESOURCES inputDir outputDir prefix namespace outputFiles)
  FILE(GLOB inputFiles ${inputDir}/*)
  SET(${outputFiles})
  FOREACH(input ${inputFiles})
    GET_FILENAME_COMPONENT(inputBase ${input} NAME)
    IF("${inputBase}" MATCHES "^\\.")
      CONTINUE()
    ENDIF()
    SET(output "${outputDir}/${inputBase}.cpp")
    LIST(APPEND ${outputFiles} "${output}")

    ADD_CUSTOM_COMMAND(
      OUTPUT ${output}
      COMMAND ${CMAKE_COMMAND}
        "-Dinput=${input}"
        "-Doutput=${output}"
        "-Dprefix=${prefix}"
        "-Dnamespace=${namespace}"
        -P "${SCRIPTS_DIR}/GenResource.cmake"
      MAIN_DEPENDENCY ${input}
      DEPENDS ${SCRIPTS_DIR}/GenResource.cmake
      VERBATIM
    )
  ENDFOREACH()
ENDMACRO()
