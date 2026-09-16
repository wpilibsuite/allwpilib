# Code-generation helpers for the AOS SDK.
#
# These mirror the Bazel macros of the same name (see aos/defs.bzl), including
# the layout of generated files: schemas are named relative to a single project
# root, and generated headers land in a mirrored tree, so a schema that includes
# another schema resolves the same way it does under Bazel.

include_guard(GLOBAL)

# AOS_FLATC_ARGS comes from here. AosConfig.cmake includes it too; the guard in
# that file makes the double include free.
include("${CMAKE_CURRENT_LIST_DIR}/AosGeneratedSettings.cmake")

# Root that schema names are recorded relative to. Every aos_static_flatbuffer()
# in a project must agree on this, or cross-schema includes break.
if(NOT DEFINED AOS_FBS_ROOT)
    set(AOS_FBS_ROOT "${PROJECT_SOURCE_DIR}")
endif()

# Single mirrored output tree, added to the include path of every generated
# library so `#include "some/dir/other_static.h"` resolves.
if(NOT DEFINED AOS_GENERATED_DIR)
    set(AOS_GENERATED_DIR "${PROJECT_BINARY_DIR}/aos_generated")
endif()

# aos_static_flatbuffer(<name> SRCS <schema.fbs>... [DEPS <target>...])
#
# Generates <stem>_generated.h, <stem>_static.h and <stem>.bfbs for each schema
# and defines <name> as an INTERFACE library carrying the include path. The
# .bfbs files are recorded on the target so aos_config() can find them, both
# directly and through DEPS.
function(aos_static_flatbuffer NAME)
    cmake_parse_arguments(ARG "" "" "SRCS;DEPS" ${ARGN})

    if(NOT ARG_SRCS)
        message(FATAL_ERROR "aos_static_flatbuffer(${NAME}): SRCS is required")
    endif()

    set(_generated_headers "")
    set(_bfbs_files "")

    foreach(_src ${ARG_SRCS})
        get_filename_component(_src_abs "${_src}" ABSOLUTE)
        get_filename_component(_stem "${_src}" NAME_WE)

        # Where this schema sits relative to the project root, which is both the
        # name flatc records in the reflection data and the subdirectory the
        # generated headers go into.
        file(RELATIVE_PATH _src_rel "${AOS_FBS_ROOT}" "${_src_abs}")
        get_filename_component(_src_rel_dir "${_src_rel}" DIRECTORY)

        if(_src_rel MATCHES "^\\.\\.")
            message(
                FATAL_ERROR
                "aos_static_flatbuffer(${NAME}): ${_src} is outside AOS_FBS_ROOT "
                "(${AOS_FBS_ROOT}). Set AOS_FBS_ROOT to a directory containing "
                "every schema in the project."
            )
        endif()

        set(_out_dir "${AOS_GENERATED_DIR}/${_src_rel_dir}")
        set(_bfbs "${_out_dir}/${_stem}.bfbs")
        set(_generated_h "${_out_dir}/${_stem}_generated.h")
        set(_static_h "${_out_dir}/${_stem}_static.h")

        add_custom_command(
            OUTPUT "${_bfbs}" "${_generated_h}"
            COMMAND ${CMAKE_COMMAND} -E make_directory "${_out_dir}"
            COMMAND
                "${AOS_FLATC}" -I "${AOS_FBS_ROOT}" -I "${AOS_FLATBUFFERS_INCLUDE_DIR}"
                ${AOS_FLATC_ARGS} --cpp --schema --binary
                # Without this the schema records no declaration filename, and
                # the static generator dereferences a null instead of erroring.
                --bfbs-filenames "${AOS_FBS_ROOT}" -o "${_out_dir}" "${_src_abs}"
            DEPENDS "${_src_abs}" "${AOS_FLATC}"
            COMMENT "flatc ${_src_rel}"
            VERBATIM
        )

        add_custom_command(
            OUTPUT "${_static_h}"
            COMMAND
                "${AOS_STATIC_FLATBUFFER_GENERATE}" --reflection_bfbs "${_bfbs}" --output_file
                "${_static_h}" --base_file_name "${_src_rel}"
            DEPENDS "${_bfbs}" "${AOS_STATIC_FLATBUFFER_GENERATE}"
            COMMENT "Generating static flatbuffer header for ${_src_rel}"
            VERBATIM
        )

        list(APPEND _generated_headers "${_generated_h}" "${_static_h}")
        list(APPEND _bfbs_files "${_bfbs}")
    endforeach()

    add_custom_target(${NAME}_generate DEPENDS ${_generated_headers})

    add_library(${NAME} INTERFACE)
    add_dependencies(${NAME} ${NAME}_generate)
    target_include_directories(${NAME} INTERFACE "${AOS_GENERATED_DIR}")
    target_link_libraries(${NAME} INTERFACE aos::aos ${ARG_DEPS})

    set_target_properties(
        ${NAME}
        PROPERTIES AOS_FBS_BFBS "${_bfbs_files}" AOS_FBS_DEPS "${ARG_DEPS}"
    )
endfunction()

# Walks FLATBUFFERS and their transitive AOS_FBS_DEPS, accumulating .bfbs paths
# and the targets that produce them.
function(_aos_collect_schemas OUT_BFBS OUT_TARGETS)
    set(_bfbs "")
    set(_targets "")
    set(_queue ${ARGN})

    while(_queue)
        list(POP_FRONT _queue _target)

        if(_target IN_LIST _targets)
            continue()
        endif()

        if(NOT TARGET ${_target})
            message(
                FATAL_ERROR
                "aos_config: '${_target}' is not a target. FLATBUFFERS takes "
                "aos_static_flatbuffer() targets, not file paths."
            )
        endif()

        get_target_property(_target_bfbs ${_target} AOS_FBS_BFBS)
        if(NOT _target_bfbs)
            message(
                FATAL_ERROR
                "aos_config: target '${_target}' was not created by "
                "aos_static_flatbuffer(), so it has no schemas to contribute."
            )
        endif()

        list(APPEND _targets ${_target})
        list(APPEND _bfbs ${_target_bfbs})

        get_target_property(_target_deps ${_target} AOS_FBS_DEPS)
        if(_target_deps)
            list(APPEND _queue ${_target_deps})
        endif()
    endwhile()

    set(${OUT_BFBS} "${_bfbs}" PARENT_SCOPE)
    set(${OUT_TARGETS} "${_targets}" PARENT_SCOPE)
endfunction()

# aos_config(<name> SRC <config.json> [FLATBUFFERS <target>...] [IMPORT_DIR <dir>])
#
# Flattens <config.json> into <name>.json, <name>.stripped.json and <name>.bfbs
# in the current binary directory, inlining the schema of every channel. Defines
# <name> as a custom target and records the flattened config path in the
# <name>_CONFIG variable in the caller's scope.
#
# The SDK's core schemas are always included, so a config can import "aos.json"
# and use the /aos channels without listing anything.
function(aos_config NAME)
    cmake_parse_arguments(ARG "" "SRC;IMPORT_DIR" "FLATBUFFERS" ${ARGN})

    if(NOT ARG_SRC)
        message(FATAL_ERROR "aos_config(${NAME}): SRC is required")
    endif()

    if(NOT ARG_IMPORT_DIR)
        set(ARG_IMPORT_DIR "${AOS_SHARE_DIR}")
    endif()

    get_filename_component(_src_abs "${ARG_SRC}" ABSOLUTE)
    get_filename_component(_src_dir "${_src_abs}" DIRECTORY)
    get_filename_component(_src_name "${_src_abs}" NAME)

    _aos_collect_schemas(_user_bfbs _schema_targets ${ARG_FLATBUFFERS})

    file(GLOB _core_bfbs "${AOS_SCHEMA_DIR}/*.bfbs")
    if(NOT _core_bfbs)
        message(FATAL_ERROR "AOS SDK has no schemas in ${AOS_SCHEMA_DIR}; every config needs them.")
    endif()

    set(_full "${CMAKE_CURRENT_BINARY_DIR}/${NAME}.json")
    set(_stripped "${CMAKE_CURRENT_BINARY_DIR}/${NAME}.stripped.json")
    set(_binary "${CMAKE_CURRENT_BINARY_DIR}/${NAME}.bfbs")

    # The config path is passed as "./<name>" from the config's own directory,
    # rather than as an absolute path. An "imports" entry is resolved against the
    # importing config's folder, and an absolute result is never retried against
    # the extra import directory -- so an absolute path here would make
    # `"imports": ["aos.json"]` fail instead of falling back to the SDK's copy.
    add_custom_command(
        OUTPUT "${_full}" "${_stripped}" "${_binary}"
        COMMAND
            "${AOS_CONFIG_FLATTENER}" "--full_output=${_full}" "--stripped_output=${_stripped}"
            "--binary_output=${_binary}" "./${_src_name}" "${ARG_IMPORT_DIR}" ${_core_bfbs}
            ${_user_bfbs}
        DEPENDS
            "${_src_abs}"
            "${AOS_CONFIG_FLATTENER}"
            ${_schema_targets}
            ${_core_bfbs}
            ${_user_bfbs}
        WORKING_DIRECTORY "${_src_dir}"
        COMMENT "Flattening config ${NAME}"
        VERBATIM
    )

    add_custom_target(${NAME} ALL DEPENDS "${_full}" "${_stripped}" "${_binary}")

    set(${NAME}_CONFIG "${_full}" PARENT_SCOPE)
    set(${NAME}_STRIPPED_CONFIG "${_stripped}" PARENT_SCOPE)
    set(${NAME}_BINARY_CONFIG "${_binary}" PARENT_SCOPE)
endfunction()
