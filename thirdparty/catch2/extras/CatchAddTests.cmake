# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

# Because natively using CMake's JSON processing for arrays leads to quadratic
# running times, we do terrible hack and split JSON array by object boundary
# + commas and try to reconstruct valid JSON objects. During this, we need
# to replace CMake characters that could be in the test name/tags with
# placeholder, so it doesn't affect CMake's processing of the strings/lists
# we create during the parsing.
#
# We use 0x01, 0x02, 0x03, and 0x04 as placeholder bytes, as those cannot
# exist in JSON unescaped.
#
#   0x01  <=> ';'   (CMake list separator)
#   0x02  ==  element boundary marker used while splitting the tests array
#   0x03  <-> '['   (opens a CMake bracket-argument context)
#   0x04  <-> ']'   (closes a CMake bracket-argument context)
#
string(ASCII 1 _SemicolonEscape)
string(ASCII 2 _BoundaryEscape)
string(ASCII 3 _OpenBracketEscape)
string(ASCII 4 _CloseBracketEscape)


# Placeholder bytes in the listing would break our parsing hack, so
# we check they don't exist. They shouldn't exist in valid JSON, but
# the reporter might not be escaping them properly.
function(validate_input_noescapes listing_var)
  foreach(byte "${_SemicolonEscape}" "${_BoundaryEscape}" "${_OpenBracketEscape}" "${_CloseBracketEscape}")
    string(FIND "${${listing_var}}" "${byte}" found)
    if(NOT found EQUAL -1)
      message(FATAL_ERROR
        "The test listing contains raw control byte (0x01-0x04) which should not "
        "be there. This means either bad escaping in JSON reporter, or corrupted file. "
      )
    endif()
  endforeach()
endfunction()


# Replaces relevant characters with their placeholders, see the top of this file.
# Modifies argument `var` in place.
function(magic_escape_chars var)
  set(value "${${var}}")
  string(REPLACE ";" "${_SemicolonEscape}" value "${value}")
  string(REPLACE "[" "${_OpenBracketEscape}" value "${value}")
  string(REPLACE "]" "${_CloseBracketEscape}" value "${value}")
  set(${var} "${value}" PARENT_SCOPE)
endfunction()


# Turns placeholders back into original characters, see the top of this file.
# Modifies argument `var` in place.
function(magic_unescape_chars var)
  set(value "${${var}}")
  string(REPLACE "${_SemicolonEscape}" ";" value "${value}")
  string(REPLACE "${_OpenBracketEscape}" "[" value "${value}")
  string(REPLACE "${_CloseBracketEscape}" "]" value "${value}")
  set(${var} "${value}" PARENT_SCOPE)
endfunction()


# Abuses knowledge of Catch2's JSON reporter output for listing tests to
# split JSON array of the test listings into a CMake list of strings,
# with each element being the JSON string of one array entry.
#
# This avoids the terrible quadratic running time of using CMake's JSON
# support to parse the JSON reporter output "properly", where the whole
# JSON array of tests is parsed again for every element. Instead, we can
# use the CMake's API to only parse the individual test's objects, which
# are usually small and only have to be reparsed fixed number of times
# (once for test names, once for labels).
#
# We process the string representing the JSON array by splitting it on
# `}<ws>*,<ws>*{` and then checking for each chunk whether it is a valid
# JSON object representing Catch2's test. If not (e.g. because we split
# on the presence of `}<ws>*,<ws>*{` inside a test name), then we append
# the next chunk to the current one and check again. And again, until
# we get back to a valid JSON.
#
# Note that to support passing the object strings back from the function,
# they will still contain the placeholders and need to be unescaped before
# further processing (e.g. sending them into CMake's JSON parsing API).
function(split_json_array json_array_var out_var)
  # We have to pass the input by var name to avoid CMake processing
  # the input as an arg.
  set(json_in "${${json_array_var}}")

  # Strip the array brackets at the start and end of the JSON array.
  # Must happen before we escape the other [] instances below from the
  # actual array data.
  string(REGEX REPLACE "^[ \t\r\n]*\\[" "" json_in "${json_in}")
  string(REGEX REPLACE "\\][ \t\r\n]*$" "" json_in "${json_in}")

  magic_escape_chars(json_in)

  # We need to keep the whitespace around comma around, so that if we
  # split inside the test object, we can reconstruct it losslessly.
  string(REGEX REPLACE "(}[ \t\r\n]*)[,]([ \t\r\n]*{)" "\\1${_BoundaryEscape}\\2" json_in "${json_in}")

  # We escaped all list separators above, so now we can turn the JSON
  # string into a CMake list of fragments in single pass.
  string(REPLACE "${_BoundaryEscape}" ";" fragments "${json_in}")

  # And now we have to reconstruct the actual JSON structure from fragments.
  set(array_elements "")
  set(accumulator "")
  foreach(next_fragment IN LISTS fragments)
    if(accumulator)
      set(accumulator "${accumulator},${next_fragment}")
    else()
      set(accumulator "${next_fragment}")
    endif()

    # Because the fragments (might) contain invalid JSON characters due
    # to escaping, we have to unescape it before checking if we can parse it.
    set(maybe_json "${accumulator}")
    magic_unescape_chars(maybe_json)
    string(JSON unused ERROR_VARIABLE err GET "${maybe_json}" "name")
    if(err STREQUAL "NOTFOUND")
      list(APPEND array_elements "${accumulator}")
      set(accumulator "")
    endif()
  endforeach()

  set(${out_var} "${array_elements}" PARENT_SCOPE)
endfunction()

# TBD: Further possible optimization is that most arguments for per-test
#      `prepare_command` call are constant across one invocation of
#      `catch_discover_tests`, and thus need checking and escaping only
#      once, instead of for each test.
#      This would provide nice speed-up of the actual command preparation,
#      but it will make the script much harder to read, and it is utterly
#      dwarfed by the quadratic scaling of parsing JSON arrays in CMake.


# Prepare command with escaped (bracketed) arguments and return it via `_Command` out variable.
#
# To avoid quadratic performance when concatenating all commands together,
# the actual concatenation must be done by the caller, by appending it
# into a string of all other commands.
function(prepare_command NAME)
  set(_args "")
  # use ARGV* instead of ARGN, because ARGN splits arrays into multiple arguments
  math(EXPR _last_arg ${ARGC}-1)
  foreach(_n RANGE 1 ${_last_arg})
    set(_arg "${ARGV${_n}}")
    if(_arg MATCHES "[^-./:a-zA-Z0-9_]")
      set(_args "${_args} [==[${_arg}]==]") # form a bracket_argument
    else()
      set(_args "${_args} ${_arg}")
    endif()
  endforeach()
  set(_Command "${NAME}(${_args})\n" PARENT_SCOPE)
endfunction()

# Generates random filename in the temp folder.
# Temp folder is retrieved by checking env vars from various platforms.
function(make_temp_file_path OUT_VARIABLE FALLBACK_PATH)
  set(TEMP_DIR "")
  set(ENV_VARS
    # From XDG base dir specification
    XDG_RUNTIME_DIR
    # From POSIX standard
    TMPDIR
    # From Windows
    TMP
    TEMP
  )

  foreach(var ${ENV_VARS})
    if(DEFINED ENV{${var}} AND NOT "$ENV{${var}}" STREQUAL "")
        set(TEMP_DIR "$ENV{${var}}")
        break()
    endif()
  endforeach()

  # If all checks fail, we use the fallback path
  if(TEMP_DIR STREQUAL "")
    set(TEMP_DIR "${FALLBACK_PATH}")
  endif()

  file(TO_CMAKE_PATH "${TEMP_DIR}" TEMP_DIR)

  # Generate the random file name
  string(RANDOM LENGTH 8 RAND_ID)
  set(FINAL_TEMP_PATH "${TEMP_DIR}/Catch2-test-listing.${RAND_ID}.json")

  set(${OUT_VARIABLE} "${FINAL_TEMP_PATH}" PARENT_SCOPE)
endfunction()

function(catch_discover_tests_impl)
  cmake_parse_arguments(
    ""
    ""
    "TEST_EXECUTABLE;TEST_WORKING_DIR;TEST_OUTPUT_DIR;TEST_OUTPUT_PREFIX;TEST_OUTPUT_SUFFIX;TEST_PREFIX;TEST_REPORTER;TEST_SPEC;TEST_SUFFIX;TEST_LIST;CTEST_FILE"
    "TEST_EXTRA_ARGS;TEST_PROPERTIES;TEST_EXECUTOR;TEST_DL_PATHS;TEST_DL_FRAMEWORK_PATHS;ADD_TAGS_AS_LABELS"
    ${ARGN}
  )

  # We periodically append to the output file below, so we have to ensure
  # that it is empty at the start, or we get duplicated test scripts.
  file(REMOVE "${_CTEST_FILE}")
  # Size (in Bytes) at which the intermediate `script` var is dumped to file.
  set(_WriteToFileThreshold 50000)

  set(add_tags "${_ADD_TAGS_AS_LABELS}")
  set(prefix "${_TEST_PREFIX}")
  set(suffix "${_TEST_SUFFIX}")
  set(spec ${_TEST_SPEC})
  set(extra_args ${_TEST_EXTRA_ARGS})
  set(properties ${_TEST_PROPERTIES})
  set(reporter ${_TEST_REPORTER})
  set(output_dir ${_TEST_OUTPUT_DIR})
  set(output_prefix ${_TEST_OUTPUT_PREFIX})
  set(output_suffix ${_TEST_OUTPUT_SUFFIX})
  set(dl_paths ${_TEST_DL_PATHS})
  set(dl_framework_paths ${_TEST_DL_FRAMEWORK_PATHS})
  set(environment_modifications "")
  set(script)
  set(suite)
  set(tests)

  if(WIN32)
    set(dl_paths_variable_name PATH)
  elseif(APPLE)
    set(dl_paths_variable_name DYLD_LIBRARY_PATH)
  else()
    set(dl_paths_variable_name LD_LIBRARY_PATH)
  endif()

  # Run test executable to get list of available tests
  if(NOT EXISTS "${_TEST_EXECUTABLE}")
    message(FATAL_ERROR
      "Specified test executable '${_TEST_EXECUTABLE}' does not exist"
    )
  endif()

  if(dl_paths)
    cmake_path(CONVERT "$ENV{${dl_paths_variable_name}}" TO_NATIVE_PATH_LIST env_dl_paths)
    list(PREPEND env_dl_paths "${dl_paths}")
    cmake_path(CONVERT "${env_dl_paths}" TO_NATIVE_PATH_LIST paths)
    set(ENV{${dl_paths_variable_name}} "${paths}")
  endif()

  if(APPLE AND dl_framework_paths)
    cmake_path(CONVERT "$ENV{DYLD_FRAMEWORK_PATH}" TO_NATIVE_PATH_LIST env_dl_framework_paths)
    list(PREPEND env_dl_framework_paths "${dl_framework_paths}")
    cmake_path(CONVERT "${env_dl_framework_paths}" TO_NATIVE_PATH_LIST paths)
    set(ENV{DYLD_FRAMEWORK_PATH} "${paths}")
  endif()

  make_temp_file_path(listing_output_path "${_TEST_WORKING_DIR}")

  execute_process(
    COMMAND ${_TEST_EXECUTOR} "${_TEST_EXECUTABLE}" ${spec}
      --list-tests
      --reporter json
      --out "${listing_output_path}"
      --order lex # Make sure the output order, and thus test registration order, is consistent across runs.
    OUTPUT_VARIABLE listing_output
    RESULT_VARIABLE result
    WORKING_DIRECTORY "${_TEST_WORKING_DIR}"
  )
  if(NOT ${result} EQUAL 0)
    message(FATAL_ERROR
      "Error listing tests from executable '${_TEST_EXECUTABLE}':\n"
      "  Result: ${result}\n"
      "  Output: ${listing_output}\n"
    )
  endif()

  # Read the JSON output back from the output file and validate it.
  file(READ ${listing_output_path} listing_output)
  file(REMOVE ${listing_output_path})
  validate_input_noescapes(listing_output)

  # Prepare reporter
  if(reporter)
    set(reporter_arg "--reporter ${reporter}")

    # Run test executable to check whether reporter is available
    # note that the use of --list-reporters is not the important part,
    # we only want to check whether the execution succeeds with ${reporter_arg}
    execute_process(
      COMMAND ${_TEST_EXECUTOR} "${_TEST_EXECUTABLE}" ${spec} ${reporter_arg} --list-reporters
      OUTPUT_VARIABLE reporter_check_output
      RESULT_VARIABLE reporter_check_result
      WORKING_DIRECTORY "${_TEST_WORKING_DIR}"
    )
    if(${reporter_check_result} EQUAL 255)
      message(FATAL_ERROR
        "\"${reporter}\" is not a valid reporter!\n"
      )
    elseif(NOT ${reporter_check_result} EQUAL 0)
      message(FATAL_ERROR
        "Error checking for reporter in test executable '${_TEST_EXECUTABLE}':\n"
        "  Result: ${reporter_check_result}\n"
        "  Output: ${reporter_check_output}\n"
      )
    endif()
  endif()

  # Prepare output dir
  if(output_dir AND NOT IS_ABSOLUTE ${output_dir})
    set(output_dir "${_TEST_WORKING_DIR}/${output_dir}")
    if(NOT EXISTS ${output_dir})
      file(MAKE_DIRECTORY ${output_dir})
    endif()
  endif()

  if(dl_paths)
    foreach(path ${dl_paths})
      cmake_path(NATIVE_PATH path native_path)
      list(PREPEND environment_modifications "${dl_paths_variable_name}=path_list_prepend:${native_path}")
    endforeach()
  endif()

  if(APPLE AND dl_framework_paths)
    foreach(path ${dl_framework_paths})
      cmake_path(NATIVE_PATH path native_path)
      list(PREPEND environment_modifications "DYLD_FRAMEWORK_PATH=path_list_prepend:${native_path}")
    endforeach()
  endif()

  # Parse JSON output for list of tests/class names/tags
  string(JSON version GET "${listing_output}" "version")
  if(NOT version STREQUAL "1")
    message(FATAL_ERROR "Unsupported catch output version: '${version}'")
  endif()

  # Extract just the JSON array with tests and then split them into
  # individual objects.
  string(JSON test_listing GET "${listing_output}" "listings" "tests")
  split_json_array(test_listing tests)

  # Exit early if no tests are detected
  if(NOT tests)
    file(WRITE "${_CTEST_FILE}" "")
    return()
  endif()

  # Each element in the tests is JSON-string representing one test object.
  # We have to parse it and then turn it into CTest script commands.
  foreach(single_test IN LISTS tests)
    # Because appending to the same string in CMake has quadratic runtime,
    # we flush the script into the file periodically to avoid the worst case.
    string(LENGTH "${script}" script_len)
    if (script_len GREATER _WriteToFileThreshold)
      file(APPEND "${_CTEST_FILE}" "${script}")
      set(script "")
    endif()

    # The elements are still escaped and contain JSON-invalid characters,
    # they have to be unescaped before parsing them as JSON.
    magic_unescape_chars(single_test)
    if(add_tags)
      string(JSON test_tags GET "${single_test}" "tags")
    endif()
    string(JSON plain_name GET "${single_test}" "name")

    # Escape characters in test case names that would be parsed by Catch2
    # Note that the \ escaping must happen FIRST! Do not change the order.
    set(escaped_name "${plain_name}")
    foreach(char \\ , [ ] ;)
      string(REPLACE ${char} "\\${char}" escaped_name "${escaped_name}")
    endforeach(char)
    # ...add output dir
    if(output_dir)
      string(REGEX REPLACE "[^A-Za-z0-9_]" "_" escaped_name_clean "${escaped_name}")
      set(output_dir_arg "--out ${output_dir}/${output_prefix}${escaped_name_clean}${output_suffix}")
    endif()

    # ...and add to script
    prepare_command(add_test
      "${prefix}${plain_name}${suffix}"
      ${_TEST_EXECUTOR}
      "${_TEST_EXECUTABLE}"
      "${escaped_name}"
      ${extra_args}
      "${reporter_arg}"
      "${output_dir_arg}"
    )
    string(APPEND script "${_Command}")
    prepare_command(set_tests_properties
      "${prefix}${plain_name}${suffix}"
      PROPERTIES
      WORKING_DIRECTORY "${_TEST_WORKING_DIR}"
      ${properties}
    )
    string(APPEND script "${_Command}")

    if(add_tags)
      string(JSON num_tags LENGTH "${test_tags}")
      math(EXPR num_tags "${num_tags} - 1")
      set(tag_list "")
      if(num_tags GREATER_EQUAL "0")
        foreach(tag_idx RANGE ${num_tags})
          string(JSON a_tag GET "${test_tags}" "${tag_idx}")
          # Catch2's tags can contain semicolons, which are list element separators
          # in CMake, so we have to escape them. Ideally we could use the [=[...]=]
          # syntax for this, but CTest currently keeps the square quotes in the label
          # name. So we add 2 backslashes to escape it instead.
          # **IMPORTANT**: The number of backslashes depends on how many layers
          #                of CMake the tag goes. If this script is changed, the
          #                number of backslashes to escape may change as well.
          string(REPLACE ";" "\\;" a_tag "${a_tag}")
          list(APPEND tag_list "${a_tag}")
        endforeach()

        prepare_command(set_tests_properties
          "${prefix}${plain_name}${suffix}"
          PROPERTIES
          LABELS "${tag_list}"
        )
        string(APPEND script "${_Command}")
      endif()
    endif(add_tags)

    if(environment_modifications)
      prepare_command(set_tests_properties
        "${prefix}${plain_name}${suffix}"
        PROPERTIES
        ENVIRONMENT_MODIFICATION "${environment_modifications}")
      string(APPEND script "${_Command}")
    endif()

    list(APPEND tests "${prefix}${plain_name}${suffix}")
  endforeach()

  # Create a list of all discovered tests, which users may use to e.g. set
  # properties on the tests
  prepare_command(set ${_TEST_LIST} ${tests})
  string(APPEND script "${_Command}")

  # Write any script leftovers we have
  file(APPEND "${_CTEST_FILE}" "${script}")
endfunction()

# To enable `include`ing this file in the unit test scripts, we only run
# the impl if an actual `TEST_EXECUTABLE` is provided.
if(CMAKE_SCRIPT_MODE_FILE AND DEFINED TEST_EXECUTABLE)
  catch_discover_tests_impl(
    TEST_EXECUTABLE ${TEST_EXECUTABLE}
    TEST_EXECUTOR ${TEST_EXECUTOR}
    TEST_WORKING_DIR ${TEST_WORKING_DIR}
    TEST_SPEC ${TEST_SPEC}
    TEST_EXTRA_ARGS ${TEST_EXTRA_ARGS}
    TEST_PROPERTIES ${TEST_PROPERTIES}
    TEST_PREFIX ${TEST_PREFIX}
    TEST_SUFFIX ${TEST_SUFFIX}
    TEST_LIST ${TEST_LIST}
    TEST_REPORTER ${TEST_REPORTER}
    TEST_OUTPUT_DIR ${TEST_OUTPUT_DIR}
    TEST_OUTPUT_PREFIX ${TEST_OUTPUT_PREFIX}
    TEST_OUTPUT_SUFFIX ${TEST_OUTPUT_SUFFIX}
    TEST_DL_PATHS ${TEST_DL_PATHS}
    TEST_DL_FRAMEWORK_PATHS ${TEST_DL_FRAMEWORK_PATHS}
    CTEST_FILE ${CTEST_FILE}
    ADD_TAGS_AS_LABELS ${ADD_TAGS_AS_LABELS}
  )
endif()
