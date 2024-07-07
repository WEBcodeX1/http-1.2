#ifndef Constant_hpp
#define Constant_hpp

#define RUNAS_USER_DEFAULT 2000
#define RUNAS_GROUP_DEFAULT 2000

#define LOG_LEVEL 100

#define SERVER_VERSION "v0.1alpha"

#define CLIENTS_MAX 4096*2
#define NET_BACKLOG CLIENTS_MAX

#define BUFFER_SIZE 4096*8

#define EPOLL_FD_COUNT_MAX 128

#define IDLE_SLEEP_MILLISECONDS 1

#define HTTP_REQUEST_MAX_SIZE 4096

#define SHMEM_PROCESS_QUEUE_SIZE EPOLL_FD_COUNT_MAX*HTTP_REQUEST_MAX_SIZE*4

#define STATICFS_SUBDIR "/static"

#define USE_NONBLOCKING_COROUTINES false

#endif