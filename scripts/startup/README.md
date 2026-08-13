# 1. Startup Script Variants

This directory contains service definitions for the init systems currently supported by the installer.

## 1.1. Subdirectories

- [`systemd/`](./systemd/): service unit for modern Linux distributions.
- [`openrc/`](./openrc/): OpenRC service wrapper.
- [`init.d/`](./init.d/): SysVinit compatible startup script.

The top-level installer logic in [`../install_startup_script.cmake`](../install_startup_script.cmake) chooses one of these variants during installation.

## 1.2. Manual Installation Quick Reference

- **systemd:** copy `systemd/falcon-as.service` to `/etc/systemd/system/`, then run `systemctl daemon-reload` and `systemctl enable --now falcon-as`.
- **OpenRC:** copy `openrc/falcon-as` to `/etc/init.d/`, make it executable, then run `rc-update add falcon-as default` and `rc-service falcon-as start`.
- **SysVinit:** copy `init.d/falcon-as` to `/etc/init.d/`, make it executable, then run `update-rc.d falcon-as defaults` and `service falcon-as start`.
