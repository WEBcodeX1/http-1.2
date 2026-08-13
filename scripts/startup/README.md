# 1. Startup Script Variants

This directory contains service definitions for the init systems currently supported by the installer.

## 1.1. Subdirectories

- [`systemd/`](./systemd/): service unit for modern Linux distributions.
- [`openrc/`](./openrc/): OpenRC service wrapper.
- [`init.d/`](./init.d/): SysVinit compatible startup script.

The top-level installer logic in `../install_startup_script.cmake` chooses one of these variants during installation.
