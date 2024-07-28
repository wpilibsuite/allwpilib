macro(add_source_jar target)
    set(oneValueArgs OUTPUT_NAME OUTPUT_DIR)
    cmake_parse_arguments(SOURCE_JAR "" "${oneValueArgs}" "BASE_DIRECTORIES" ${ARGN})
    foreach(base_package_dir ${SOURCE_JAR_BASE_DIRECTORIES})
        file(GLOB_RECURSE directories LIST_DIRECTORIES true ${base_package_dir}/*.directoriesonly)
        # Find all packages
        foreach(directory ${directories})
            cmake_path(
                RELATIVE_PATH
                directory
                BASE_DIRECTORY ${base_package_dir}
                OUTPUT_VARIABLE package_name
            )
            file(GLOB package_sources ${directory}/*.java)
            if(package_sources STREQUAL "")
                continue()
            endif()
            # If package sources are scattered across different places, consolidate them under one package
            list(FIND packages ${package_name} index)
            if(index EQUAL -1)
                list(APPEND packages ${package_name})
            endif()
            list(APPEND ${package_name} ${package_sources})
        endforeach()
    endforeach()
    set(resources "")
    foreach(package ${packages})
        string(APPEND resources "NAMESPACE \"${package}\" ${${package}} ")
    endforeach()
    cmake_language(
        EVAL CODE
            "add_jar(${target} RESOURCES ${resources} OUTPUT_NAME ${SOURCE_JAR_OUTPUT_NAME} OUTPUT_DIR ${SOURCE_JAR_OUTPUT_DIR})"
    )
endmacro()
