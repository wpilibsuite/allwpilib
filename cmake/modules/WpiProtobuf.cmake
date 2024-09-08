function(wpi_protobuf_generate)
    set(_singleargs PROTOC_OUT_DIR PLUGIN DEPENDENCIES)
    if(COMMAND target_sources)
        list(APPEND _singleargs TARGET)
    endif()
    set(_multiargs PROTOS)

    cmake_parse_arguments(
        wpi_protobuf_generate
        "${_options}"
        "${_singleargs}"
        "${_multiargs}"
        "${ARGN}"
    )

    if(NOT wpi_protobuf_generate_PROTOS)
        message(SEND_ERROR "Error: protobuf_generate called without any targets or source files")
        return()
    endif()

    if(NOT wpi_protobuf_generate_TARGET)
        message(SEND_ERROR "Error: wpi_protobuf_generate called without a target")
        return()
    endif()

    if(NOT wpi_protobuf_generate_PROTOC_OUT_DIR)
        message(SEND_ERROR "Error: protobuf_generate called without a protoc out directory")
        return()
    endif()

    if(NOT wpi_protobuf_generate_PLUGIN)
        message(SEND_ERROR "Error: wpi_protobuf_generate called without a plugin")
        return()
    endif()

    set(_generate_extensions .pb.h .pb.cc)

    # Create an include path for each file specified
    foreach(_file ${wpi_protobuf_generate_PROTOS})
        get_filename_component(_abs_file ${_file} ABSOLUTE)
        get_filename_component(_abs_dir ${_abs_file} DIRECTORY)
        list(FIND _protobuf_include_path ${_abs_dir} _contains_already)
        if(${_contains_already} EQUAL -1)
            list(APPEND _protobuf_include_path -I ${_abs_dir})
        endif()
    endforeach()

    set(_generated_srcs_all)
    foreach(_proto ${wpi_protobuf_generate_PROTOS})
        get_filename_component(_abs_file ${_proto} ABSOLUTE)
        get_filename_component(_abs_dir ${_abs_file} DIRECTORY)
        get_filename_component(_basename ${_proto} NAME_WLE)
        file(RELATIVE_PATH _rel_dir ${CMAKE_CURRENT_SOURCE_DIR} ${_abs_dir})

        set(_possible_rel_dir)

        set(_generated_srcs)
        foreach(_ext ${_generate_extensions})
            list(
                APPEND
                _generated_srcs
                "${wpi_protobuf_generate_PROTOC_OUT_DIR}/${_possible_rel_dir}${_basename}${_ext}"
            )
        endforeach()

        list(APPEND _generated_srcs_all ${_generated_srcs})

        set(_comment "Running WPILib protocol buffer compiler on ${_proto}")

        add_custom_command(
            OUTPUT ${_generated_srcs}
            COMMAND protobuf::protoc
            ARGS
                --cpp_out ${wpi_protobuf_generate_PROTOC_OUT_DIR} --wpilib_out
                ${wpi_protobuf_generate_PROTOC_OUT_DIR}
                --plugin=protoc-gen-wpilib=${wpi_protobuf_generate_PLUGIN} ${_protobuf_include_path}
                ${_abs_file}
            DEPENDS
                ${_abs_file}
                protobuf::protoc
                ${wpi_protobuf_generate_DEPENDENCIES}
                ${wpi_protobuf_generate_PLUGIN}
            COMMENT ${_comment}
            VERBATIM
        )
    endforeach()

    set_source_files_properties(${_generated_srcs_all} PROPERTIES GENERATED TRUE)
    if(wpi_protobuf_generate_TARGET)
        target_sources(${wpi_protobuf_generate_TARGET} PRIVATE ${_generated_srcs_all})
    endif()
endfunction()
