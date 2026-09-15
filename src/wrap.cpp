#include"wrap.h"

int Socket (int __domain, int __type, int __protocol)
{
int fd;
    fd=socket(__domain,__type,__protocol);
    if(fd==-1)
    {
        perror("socket error");
        exit(1);
    }
    return fd;
}
int Bind (int __fd, __CONST_SOCKADDR_ARG __addr, socklen_t __len)
{
    int ret=bind(__fd,__addr,__len);
    if(ret==-1)
    {
        perror("bind error");
        exit(1);
    }
    return 0;
}
int Listen (int __fd, int __n)
{
    int ret=listen(__fd,__n);
    if(ret==-1)
    {
        perror("listen error");
        exit(1);
    }
    return 0;
}
int Accept (int __fd, __SOCKADDR_ARG __addr,socklen_t *__restrict __addr_len)
           {
int fd=accept(__fd,__addr,__addr_len);
if(fd==-1)
{
    perror("accept error");
    exit(1);
}
return fd;
           }

           int Connect (int __fd, __CONST_SOCKADDR_ARG __addr, socklen_t __len)
           {
            int ret=connect(__fd,__addr,__len);
            if(ret==-1)
            {
                perror("connect error");
                exit(1);
            }
            return 0;
           }

           ssize_t Read (int __fd, void *__buf, size_t __nbytes)
           {
            ssize_t n=read( __fd,__buf,__nbytes);
            if(n==-1)
            {
                perror("read error");
                exit(1);
            }
            return n;
           }
           ssize_t Write (int __fd, const void *__buf, size_t __n)
           {
            ssize_t ret=write(__fd,__buf,__n);
            if(ret==-1)
            {
                perror("write error");
                exit(1);
            }
            return ret;
           }
           int Select (int __nfds, fd_set *__restrict __readfds,
		   fd_set *__restrict __writefds,
		   fd_set *__restrict __exceptfds,
		   struct timeval *__restrict __timeout)
           {
            int ret=select( __nfds,__readfds,__writefds,__exceptfds,__timeout);
            if(ret==-1)
            {
                perror("select error");
                exit(1);
            }
            return ret;
           }
           extern int Epoll_create1 (int __flags) __THROW
           {
            int epfd=epoll_create1(__flags);
            if(epfd==-1)
{
    perror("epoll_create1 error");
    exit(1);
}
return epfd;
           }

           extern int Epoll_wait (int __epfd, struct epoll_event *__events,
		       int __maxevents, int __timeout)
	
    {
        int ret=epoll_wait( __epfd,__events,__maxevents, __timeout);
        if(ret==-1)
        {
            perror("epoll_wait error");
            exit(1);

        }
return ret;
    }
     extern int Epoll_ctl (int __epfd, int __op, int __fd,
		      struct epoll_event *event) __THROW
              {
                int ret=epoll_ctl( __epfd,__op,__fd,event);
if(ret==-1)
{
    perror("epoll_ctl error");
    exit(1);
}
return ret;
              }
