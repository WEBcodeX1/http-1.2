# Tests

## Subdirs

| Path          | Short Description              | Description                                       |
|---------------|--------------------------------|---------------------------------------------------|
| /eval         | Evaluation Tests               | Everything which needs evaluation.                |
| /unit         | Unit Tests                     | Unit Tests.                                       |
| /integration  | Integration Tests              | Integration Tests.                                |
| /performance  | Performace Tests               | Tests prooving Performance.                       |

## Prerequsites

To be sure nothing else will corrupt test results, settings for the following features must be adjusted:

- DNS Resolving
- TCP / IP Stack Settings
- Linux Kernel Settings

Currently our Test-VM runs Ubuntu 22.04 with a 6.5 Kernel and 12 CPU Cores on Intel i9 .

### DNS

For testing all Virtual Domain Names will be written to /etc/hosts.

### TCP / IP Stack

Currently TCP_CORK and TCP_NODELAY options are set by default.
This is proposed inside Kernel Documentation and must be re-validated if indeed working correctly.

### Sysctl Values

- Disable Kernel Scheduler Autogrouping
- Disable Kernel Scheduler Energy Awareness
- Prevent Kernel Scheduler from collection Statistics
- Disable Virtual Memory Subsystem Memory Compaction
- Reduce Virtual Memory Subsystem Memory Swap Behaviour

```bash

# Kernel Scheduler Settings
sysctl -w kernel.sched_autogroup_enabled=0
sysctl -w kernel.sched_energy_aware=0
sysctl -w kernel.sched_schedstats=0

# Disable VM Memeory Compation
sysctl -w vm.compact_unevictable_allowed=0
sysctl -w vm.compaction_proactiveness=0

# Disable Watchdogs
sysctl -w kernel.soft_watchdog=0
sysctl -w kernel.watchdog=0

# Reduce Swappiness
sysctl -w vm.swappiness=0

```

## Dump Packets

To check 

