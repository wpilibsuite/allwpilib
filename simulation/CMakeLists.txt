if(WITH_GUI)
    add_subdirectory(halsim_gui)
endif()

# DS socket not supported in cmake
#add_subdirectory(halsim_ds_socket)
add_subdirectory(halsim_ws_core)
add_subdirectory(halsim_ws_client)
add_subdirectory(halsim_ws_server)
add_subdirectory(halsim_xrp)
