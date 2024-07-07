#!/bin/sh

server_user="falcon-http"
server_group="${server_user}"
server_dir="/var/www"
user_id=2000
user_gid=2000

mkdir -p ${server_dir}

groupadd -g ${user_gid} ${server_user}
useradd -d ${server_dir} -s /bin/false -g ${user_gid} -M -u ${user_id} ${server_user}
usermod -a -G ${server_group} ${server_user}

chown ${user_id}:${user_gid} ${server_dir}
chmod 775 ${server_dir}

