# Startup Scripts

This directory contains startup scripts for the Falcon HTTP Application Server.

## SystemD (Ubuntu, Debian 12+)

To install and enable the systemd service:

```bash
# Copy service file to systemd directory
sudo cp systemd/falcon-as.service /etc/systemd/system/

# Reload systemd configuration
sudo systemctl daemon-reload

# Enable service to start on boot
sudo systemctl enable falcon-as

# Start the service
sudo systemctl start falcon-as

# Check service status
sudo systemctl status falcon-as
```

## OpenRC (Devuan, Gentoo, Alpine)

To install and enable the OpenRC init script:

```bash
# Copy init script to init.d directory
sudo cp openrc/falcon-as /etc/init.d/

# Make it executable (if not already)
sudo chmod +x /etc/init.d/falcon-as

# Add to default runlevel
sudo rc-update add falcon-as default

# Start the service
sudo rc-service falcon-as start

# Check service status
sudo rc-service falcon-as status
```

## SysVinit (Debian <= 11, Ubuntu <= 14.04, older systems)

To install and enable the SysVinit init script:

```bash
# Copy init script to init.d directory
sudo cp init.d/falcon-as /etc/init.d/

# Make it executable (if not already)
sudo chmod +x /etc/init.d/falcon-as

# Enable service to start on boot
sudo update-rc.d falcon-as defaults

# Start the service
sudo service falcon-as start
# or
sudo /etc/init.d/falcon-as start

# Check service status
sudo service falcon-as status
# or
sudo /etc/init.d/falcon-as status

# Stop the service
sudo service falcon-as stop

# Restart the service
sudo service falcon-as restart
```

## Important Notes

All startup scripts include:
- **ulimit settings**: Sets maximum open files to 1,000,000
- **Transparent hugepages**: Sets vm.nr_hugepages to 1024
- **User/Group**: Runs as `falcon-http` user and group
- **Working directory**: `/var/www`

Make sure the `falcon-http` user and group exist before starting the service (see `/scripts/add_user_group.sh`).
