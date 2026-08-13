# 1. Installation and System Scripts

This directory contains helper scripts used during installation and system preparation.

## 1.1. Contents

- `add_user_group.sh`: creates the runtime user and group expected by the server.
- `cp_etc.sh`: copies packaged configuration files into their target location.
- `install_startup_script.cmake`: selects the startup integration matching the detected init system.
- `mk_www.sh`: prepares the web root used by the example setup.
- `patch_etc_hosts.sh`: adds local hostname mappings used by the sample namespaces.
- `set-transparent-hugepages.sh`: applies hugepage related runtime settings.
- `ulimit.sh`: raises the open-file limit for high-connection workloads.
- [`startup/`](./startup/): init-system specific service definitions.
