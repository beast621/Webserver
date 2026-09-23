#include<iostream>
#include<sys/timerfd.h>
#include<sys/epoll.h>
#include<unistd.h>
#include<cstdint>
#include<cerrno>

using namespace std;

bool timer_test(int fd,int epfd)
{

    struct itimerspec value{};
    value.it_interval.tv_sec=1;
    value.it_value.tv_sec=1;
    struct epoll_event ev{};
    ev.events=EPOLLIN|EPOLLET;
    ev.data.fd=fd;
    int ret=epoll_ctl(epfd,EPOLL_CTL_ADD,fd,&ev);
if(ret==-1)
{
    perror("epoll_ctl error");
    return false;

}

if(timerfd_settime(fd,0,&value,nullptr)==-1)
{

    return false;

}

int round=0;
while(round<2)
{

    struct epoll_event event{};
    int ret=epoll_wait(epfd,&event,1,3000);
    if(ret==-1&&errno==EINTR)
    continue;

    if(ret!=1)
    {
        cerr<<"no thing"<<endl;
        return false;

    }
if(!(event.events&EPOLLIN)||(event.events&(EPOLLERR|EPOLLHUP))||(event.data.fd!=fd))

{
    cerr<<"error"<<endl;
    return false;

}

bool expired=false;

while(1)
{
uint64_t count=0;
    ssize_t n=read(fd,&count,sizeof(count));
    if(n==-1&&errno==EINTR)
    {
continue;
    }

    if(n==-1&&errno==EAGAIN)
    break;

    if(n==(ssize_t)sizeof(count))
    {
        if(count==0)
        return false;

        expired=true;
        continue;


    }

    cerr<<"fail"<<endl;\
    return false;

}

if(!expired)
return false;
round++;

cout<<"第"<<round<<"轮定时通知通过"<<endl;


}
return true;

}

int main()
{

    int fd=timerfd_create(CLOCK_MONOTONIC,
    TFD_NONBLOCK|TFD_CLOEXEC);

    if(fd==-1)
    {
        perror("timerfd_create error");
        return 1;

    }

    int epfd=epoll_create1(0);
    if(epfd==-1)
    {
        perror("epoll_create error");
        close(fd);
        return 1;

    }


    bool ok=timer_test(fd,epfd);

    close(fd);
    close(epfd);
    if(!ok)
    {
        cerr<<"timer test failed"<<endl;
        return 1;

    }

    cout<<"timer_test passed"<<endl;
    return 0;
}