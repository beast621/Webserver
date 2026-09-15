#pragma once
#include<iostream>
#include<unistd.h>
#include <sys/stat.h>
#include<fcntl.h>
#include<string>
#include<dirent.h>
#include <cstring>
#include<pthread.h>
#include<mutex>
#include<thread>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<cctype>
#include<sys/epoll.h>
int Socket (int __domain, int __type, int __protocol);
int Bind (int __fd, __CONST_SOCKADDR_ARG __addr, socklen_t __len);
int Listen (int __fd, int __n);
int Accept (int __fd, __SOCKADDR_ARG __addr,
		   socklen_t *__restrict __addr_len);
           ssize_t Read (int __fd, void *__buf, size_t __nbytes);
           ssize_t Write (int __fd, const void *__buf, size_t __n);
           int Connect (int __fd, __CONST_SOCKADDR_ARG __addr, socklen_t __len);
           int Select (int __nfds, fd_set *__restrict __readfds,
		   fd_set *__restrict __writefds,
		   fd_set *__restrict __exceptfds,
		   struct timeval *__restrict __timeout);
        extern int Epoll_create1 (int __flags) __THROW;
        extern int Epoll_ctl (int __epfd, int __op, int __fd,
		      struct epoll_event *__event) __THROW;
              extern int Epoll_wait (int __epfd, struct epoll_event *__events,
		       int __maxevents, int __timeout)
	__attr_access ((__write_only__, 2, 3)) __nonnull ((2));
        