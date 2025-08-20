#ifndef Constant_hpp
#define Constant_hpp

#define RUNAS_USER_DEFAULT 2000
#define RUNAS_GROUP_DEFAULT 2000

#define LOG_LEVEL 251

#define SERVER_VERSION "v0.1"

#define CLIENTS_MAX 4096*2
#define NET_BACKLOG CLIENTS_MAX

#define BUFFER_SIZE 4096*8

#define EPOLL_FD_COUNT_MAX 256

#define IDLE_SLEEP_MICROSECONDS 500

#define HTTP_REQUEST_MAX_SIZE 4096
#define HTTP_RESPONSE_MAX_SIZE 4096*2

#define SHMEM_STATICFS_SIZE EPOLL_FD_COUNT_MAX*HTTP_REQUEST_MAX_SIZE*4

#define STATICFS_SUBDIR "/static"
#define BACKEND_ROOT_PATH "/backend"

#define CONFIG_FILE "/etc/falcon-http/config.json"
#endif
