# Detect Linux init system and install appropriate startup script
# This script runs at install time to detect the init system and install the correct startup script

message(STATUS "========================================")
message(STATUS "Installing startup script...")
message(STATUS "========================================")

# Detect systemd
if(EXISTS "/run/systemd/system" OR EXISTS "/usr/lib/systemd/system")
    set(INIT_SYSTEM "systemd")
    set(INIT_SCRIPT_SRC "${CMAKE_CURRENT_LIST_DIR}/startup/systemd/falcon-as.service")
    set(INIT_SCRIPT_DEST "/etc/systemd/system/falcon-as.service")
    message(STATUS "Detected init system: systemd")
    
# Detect OpenRC
elseif((EXISTS "/etc/init.d" AND EXISTS "/etc/runlevels") OR EXISTS "/sbin/openrc-run" OR EXISTS "/usr/sbin/openrc-run")
    set(INIT_SYSTEM "openrc")
    set(INIT_SCRIPT_SRC "${CMAKE_CURRENT_LIST_DIR}/startup/openrc/falcon-as")
    set(INIT_SCRIPT_DEST "/etc/init.d/falcon-as")
    message(STATUS "Detected init system: OpenRC")
    
# Fallback to SysVinit
elseif(EXISTS "/etc/init.d")
    set(INIT_SYSTEM "sysvinit")
    set(INIT_SCRIPT_SRC "${CMAKE_CURRENT_LIST_DIR}/startup/init.d/falcon-as")
    set(INIT_SCRIPT_DEST "/etc/init.d/falcon-as")
    message(STATUS "Detected init system: SysVinit")
    
else()
    message(WARNING "Could not detect init system. Defaulting to SysVinit.")
    set(INIT_SYSTEM "sysvinit")
    set(INIT_SCRIPT_SRC "${CMAKE_CURRENT_LIST_DIR}/startup/init.d/falcon-as")
    set(INIT_SCRIPT_DEST "/etc/init.d/falcon-as")
endif()

# Check if source script exists
if(NOT EXISTS "${INIT_SCRIPT_SRC}")
    message(FATAL_ERROR "Startup script not found: ${INIT_SCRIPT_SRC}")
endif()

# Install the startup script
message(STATUS "Installing ${INIT_SYSTEM} startup script...")
message(STATUS "  Source: ${INIT_SCRIPT_SRC}")
message(STATUS "  Destination: ${INIT_SCRIPT_DEST}")

execute_process(
    COMMAND ${CMAKE_COMMAND} -E copy ${INIT_SCRIPT_SRC} ${INIT_SCRIPT_DEST}
    RESULT_VARIABLE INSTALL_RESULT
    ERROR_VARIABLE INSTALL_ERROR
)

if(INSTALL_RESULT EQUAL 0)
    message(STATUS "✓ Startup script installed successfully")
    
    # Make executable for init.d scripts
    if(INIT_SYSTEM STREQUAL "openrc" OR INIT_SYSTEM STREQUAL "sysvinit")
        execute_process(
            COMMAND chmod +x ${INIT_SCRIPT_DEST}
            RESULT_VARIABLE CHMOD_RESULT
        )
        if(CHMOD_RESULT EQUAL 0)
            message(STATUS "✓ Made script executable")
        endif()
    endif()
    
    # Print instructions for the user
    message(STATUS "")
    message(STATUS "========================================")
    if(INIT_SYSTEM STREQUAL "systemd")
        message(STATUS "To enable and start the service:")
        message(STATUS "  sudo systemctl daemon-reload")
        message(STATUS "  sudo systemctl enable falcon-as")
        message(STATUS "  sudo systemctl start falcon-as")
        message(STATUS "")
        message(STATUS "To check service status:")
        message(STATUS "  sudo systemctl status falcon-as")
    elseif(INIT_SYSTEM STREQUAL "openrc")
        message(STATUS "To enable and start the service:")
        message(STATUS "  sudo rc-update add falcon-as default")
        message(STATUS "  sudo rc-service falcon-as start")
        message(STATUS "")
        message(STATUS "To check service status:")
        message(STATUS "  sudo rc-service falcon-as status")
    elseif(INIT_SYSTEM STREQUAL "sysvinit")
        message(STATUS "To enable and start the service:")
        message(STATUS "  sudo update-rc.d falcon-as defaults")
        message(STATUS "  sudo service falcon-as start")
        message(STATUS "")
        message(STATUS "To check service status:")
        message(STATUS "  sudo service falcon-as status")
    endif()
    message(STATUS "========================================")
    
else()
    message(WARNING "Failed to install startup script: ${INSTALL_ERROR}")
    message(WARNING "You may need to run 'make install' with sudo/root privileges.")
    message(WARNING "Or manually copy the script:")
    message(WARNING "  sudo cp ${INIT_SCRIPT_SRC} ${INIT_SCRIPT_DEST}")
endif()
