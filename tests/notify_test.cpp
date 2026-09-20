#include<iostream>
#include<queue>
#include<mutex>
#include<cstdint>
#include<cerrno>
#include<cstdio>
#include<stdexcept>
#include<sys/eventfd.h>
#include<sys/epoll.h>
#include<unistd.h>
#include"thread_pool.h"

bool run_test(int epfd,int notifyfd)
{

    queue<int> results;
    mutex result_mtx;
    Threadpool pool(2);

    for(int i=1;i<=5;i++)
    {

        bool ok=pool.add_task(
            [i,&results,&result_mtx,notifyfd]()
            {
                int result=i*i;
                {
                    lock_guard<mutex>lock(result_mtx);
                    results.push(result);

                }
                uint64_t value=1;
                while(1)
                {
                    ssize_t n=write(notifyfd,&value,sizeof(value));
                    if(n==(ssize_t)sizeof(value))
                    break;
                    if(n==-1&&errno==EINTR)
                    continue;
                    if(n==-1&&errno==EAGAIN)
                    break;
                    throw runtime_error("eventfd write failfd");

                }
            }
        );

if(!ok)
{
    cerr<<"add task failed"<<endl;
    return false;

}



    }

int completed=0;
int sum=0;

while(completed<5)
{
struct epoll_event events[4]{};
int ret=epoll_wait(epfd,events,4,3000);
if(ret==-1)
{
    if(errno==EINTR)
    continue;
    else
    {
        perror("epoll_wait error");
        return false;

    }
}

if(ret==0)
{
    cerr<<"wait for task results timed out"<<endl;
    return false;

}
for(int i=0;i<ret;i++)
{

    if(events[i].data.fd!=notifyfd)
    continue;
    if(events[i].events&(EPOLLERR|EPOLLHUP))
    {
        cerr<<"eventfd error"<<endl;
    return false;

    }
uint64_t value=0;
ssize_t n;
do
{
     n=read(notifyfd,&value,sizeof(value));
    
}while(n==-1&&errno==EINTR);

if(n!=(ssize_t)sizeof(value))
{
    cerr<<"eventfd read failed"<<endl;

    return false;
}

queue<int>ready;

{
    lock_guard<mutex>lock(result_mtx);
    ready.swap(results);

}

while(!ready.empty())
{
    int result=ready.front();
    ready.pop();

    completed++;
    sum+=result;

}



}


}
cout<<"completed: "<<completed<<endl;
cout<<"sum: "<<sum<<endl;
return completed==5&&sum==55;


}


int main()
{


    int notifyfd=eventfd(0,EFD_NONBLOCK|EFD_CLOEXEC);
    if(notifyfd==-1)
    {
        perror("eventfd error");
        return 1;

    }

    int epfd=epoll_create1(0);
    if(epfd==-1)
    {
        perror("epoll_creat error");
        close(notifyfd);
        return 1;

    }

    struct epoll_event event{};
    event.data.fd=notifyfd;
    event.events=EPOLLIN;
    int ret=epoll_ctl(epfd,EPOLL_CTL_ADD,notifyfd,&event);
    if(ret==-1)
    {
        perror("epoll_ctl_add error");
        close(notifyfd);
        close(epfd);
        return 1;

    }


    bool ok=false;

    try
    {
     
        ok=run_test(epfd,notifyfd);

    }
    catch(const exception& e)
{
    cerr<<e.what()<<endl;

}

close(epfd);
close(notifyfd);
return ok? 0:1;

}