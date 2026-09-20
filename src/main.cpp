#include<iostream>
#include<sys/socket.h>
#include<netinet/in.h>
#include<unistd.h>
#include<cerrno>
#include<cstdio>
#include<string>
#include<sstream>
#include<wrap.h>
#include<fstream>
#include<fcntl.h>
#include<sys/epoll.h>
#include<cstring>
#include<map>
#include"http.h"
#include<sys/stat.h>
#include<chrono>
#include<signal.h>
#include<ctime>
#include<iomanip>
using namespace std;
#define Max_event 1024
int port=9527;
string root ="/home/lyf67/Webserver/www";
volatile sig_atomic_t stop_server=0;
//ofstream access_file;
int logfd=-1;
void stop_handler(int )
{

    stop_server=1;

}

struct epoll
{
    
int fd;
string send_buf;
string recv_buf;
void(*callback)(int fd,uint32_t event,struct epoll* arg);
struct epoll* arg;
size_t total;
int status;
chrono::steady_clock::time_point last_active;
chrono::steady_clock::time_point start_time;

string method;
string path;
string response_status;
};
 epoll g_event[Max_event+1];
int epfd;
void acception(int fd,uint32_t event,struct epoll *ev);
void hander_client(int cfd,uint32_t event,struct epoll *ev);
void send_client(int cfd,uint32_t event,struct epoll *ev);
void eventdel(struct epoll* ev);

bool set_nonblock(int fd)
{
    int flags=fcntl(fd,F_GETFL,0);
    return flags!=-1 && fcntl(fd,F_SETFL,flags|O_NONBLOCK)!=-1;
}
void eventset(int fd,struct epoll *ev,void(*callback)(int ,uint32_t ,struct epoll*),struct epoll* arg)
{
ev->fd=fd;
ev->arg=arg;
ev->callback=callback;
ev->status=0;
ev->recv_buf.clear();
ev->send_buf.clear();
ev->total=0;
ev->last_active=chrono::steady_clock::now();
ev->start_time=chrono::steady_clock::now();
ev->method.clear();
ev->path.clear();
ev->response_status.clear();

}

void eventadd(uint32_t event,int fd,struct epoll *ev)
{
    struct epoll_event epv{};
    epv.events=event | EPOLLET;
    epv.data.ptr=ev;

    // 已经注册的连接，切换读写事件时使用 MOD。
    int op=ev->status==0 ? EPOLL_CTL_ADD : EPOLL_CTL_MOD;
    if(epoll_ctl(epfd,op,fd,&epv)==-1)
    {
        perror("epoll_ctl error");
        eventdel(ev);
        return;
    }
    ev->status=1;
}

void Initserver()
{

    int fd=Socket(AF_INET,SOCK_STREAM,0);
    int opt=1;
    setsockopt(fd,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));
    sockaddr_in addr{};
    addr.sin_family=AF_INET;
    addr.sin_port=htons((uint16_t)port);
    addr.sin_addr.s_addr=htonl(INADDR_ANY);
    Bind(fd,(struct sockaddr*)&addr,sizeof(addr));
    Listen(fd,128);
    if(!set_nonblock(fd))
    {
        perror("fcntl error");
        close(fd);
        exit(1);
    }
    
eventset(fd,&g_event[Max_event],acception,&g_event[Max_event]);
eventadd(EPOLLIN,fd,&g_event[Max_event]);

}
void acception(int fd,uint32_t,struct epoll *)
{
    struct sockaddr_in clin_addr{};
    while(!stop_server)
    {
    socklen_t len=sizeof(clin_addr);
    // 非阻塞 accept 的 EAGAIN 不是致命错误。
    int cfd=accept(fd,(struct sockaddr*)&clin_addr,&len);
    if(cfd==-1)
    {
        if(errno==EINTR)
        {
            continue;
        }
        else if(errno==EAGAIN||errno==EWOULDBLOCK)
        {
            return;
        }
        if(errno!=EINTR && errno!=EAGAIN && errno!=EWOULDBLOCK)
            perror("accept error");
        return;
    }

    int i;
    for(i=0;i<Max_event;i++)
    {
        if(g_event[i].status==0)
            break;
    }
    if(i==Max_event)
    {
        cout<<"too many client"<<endl;
        close(cfd);
        continue;
    }
    if(!set_nonblock(cfd))
    {
        perror("fcntl error");
        close(cfd);
        continue;
    }
    eventset(cfd,&g_event[i],hander_client,&g_event[i]);
    eventadd(EPOLLIN,cfd,&g_event[i]);
    cout<<"connect success"<<endl;
}
}
void eventdel(struct epoll* ev)

{
   epoll_ctl(epfd,EPOLL_CTL_DEL,ev->fd,NULL);
   ev->arg=NULL;
   ev->callback=NULL;
   ev->recv_buf.clear();
   ev->send_buf.clear();
   ev->status=0;
   close(ev->fd);
   ev->fd=-1;
   cout<<"client close"<<endl;
}









void hander_client(int cfd,uint32_t,struct epoll* ev)
{

        char buf[BUFSIZ];
        // 保留未收完整的请求，下次回调继续拼接。
        string& message=ev->recv_buf;

        // 一次 read 不一定收到完整请求，先把数据拼起来。
        while(message.find("\r\n\r\n")==string::npos)
        {
            ssize_t n=read(cfd,buf,sizeof(buf));
            if(n==0)
            {
                eventdel(ev);
                return;
            }
            if(n==-1)
            {
                if(errno==EINTR)
                    continue;
                if(errno==EAGAIN || errno==EWOULDBLOCK)
                    return;
                perror("read error");
                eventdel(ev);
                return;
            }
            message.append(buf,(size_t)n);
ev->last_active=chrono::steady_clock::now();


            // 当前只处理小请求头，避免一直占用内存。
          
size_t end=message.find("\r\n\r\n");
if((end==string::npos&&message.size()>16*1024)||
(end!=string::npos&&end+4>16*1024))
{
    eventdel(ev);
    return;
}


        }

        

        // 只取第一行，例如：GET / HTTP/1.1
        stringstream ss(message.substr(0,message.find("\r\n")));
        string method;
        string path;
        string version;
        string extra;
        string status;
        string body;
        string quary;
string content_type="text/html; charset=utf-8";
bool valid=true;
map<string,string>headers;
        if(!(ss>>method>>path>>version) || (ss>>extra))
        {
            valid=false;
        }

        if(valid)
        {

            if(path.empty()||path[0]!='/')
            {
                valid=false;
            }

            if(version!="HTTP/1.0"&&version!="HTTP/1.1")
            {
                valid=false;
            }
        }

        if(valid)
        {
            size_t pos=path.find('?');
            if(pos!=string::npos)
            {
                quary=path.substr(pos+1);
                path=path.substr(0,pos);


            }

        }
        if(valid)
        {

            valid=parse_headers(message,headers);

        }

if(valid&&version=="HTTP/1.1")
{
auto it=headers.find("host");
if(it==headers.end() || it->second.empty())
valid=false;

}





if(!valid)
{
    status="400 Bad Request";
    body="<h1>Bad Request</h1>";
    
}


else if(path=="/echo")
{
if(method!="POST")
{

    status="405 Method Not Allowed";
    body="<h1>Method Not Allowed</h1>";

}

else if(headers.count("transfer-encoding"))
    {
        // 这一步只支持 Content-Length，不支持 chunked。
        status=headers.count("content-length")
               ? "400 Bad Request" : "501 Not Implemented";
        body="<h1>Unsupported Request Framing</h1>";
    }
    else if(headers.count("expect"))
    {
        // 暂时不实现 100 Continue，直接给出最终响应。
        status="417 Expectation Failed";
        body="<h1>Expectation Failed</h1>";
    }

else if(!headers.count("content-length"))
{

    status="411 Length Required";
    body="<h1>Length Required</h1>";

}

else
{

    size_t content_length=0;
    int result=parse_content_length(
        headers["content-length"],
        content_length
    );
    

    if(result==400)
    {

        status="400 Bad Request";
        body="<h1>Invalid Content-Length</h1>";

    }

    
else if(result==413)
{
    status="413 Content Too Large";
        body="<h1>Content Too Large</h1>";
}


if(status.empty())
{
size_t body_start=message.find("\r\n\r\n")+4;
size_t request_size=body_start+content_length;

while(message.size()<request_size)
{

size_t need=request_size-message.size();
size_t count=need<sizeof(buf) ? need : sizeof(buf);
ssize_t n=recv(cfd,buf,count,0);
if(n==0)
{
    eventdel(ev);
    return;

}

else if(n<0)
{
    if(errno==EINTR)
    continue;

    else if(errno==EAGAIN||errno==EWOULDBLOCK)
    {
        
        return;
    }
else{
    perror("read body error");
    eventdel(ev);
    return;
}

}

else 
{
    message.append(buf,(size_t)n);
    ev->last_active=chrono::steady_clock::now();

}

}
status="200 OK";
body=message.substr(body_start,content_length);
content_type="text/plain; charset=utf-8";
}

}

}


        else if(method!="GET"&&method!="HEAD")
        {
            status="405 Method Not Allowed";
            body="<h1>Method Not Allowed</h1>";
        }
        else
        {
            string filename;
            string filetype;
if(path=="/"||path=="/index.html")
{
filename=root+"/index.html";
filetype="text/html; charset=utf-8";
}
else if(path=="/style.css")
{
    filename=root+"/style.css";
    filetype="text/css; charset=utf-8";
}


if(filename.empty())
{
    status="404 Not Found";
    body="<h1>Not Found</h1>";
}
else
{
    int filefd=open(filename.c_str(),O_RDONLY);
    if(filefd==-1)
    {
        status="404 Not Found";
    body="<h1>Not Found</h1>";
    }

    else{
        status="200 OK";
        char filebuf[BUFSIZ];
        while(1)
        {

            ssize_t filen=read(filefd,filebuf,sizeof(filebuf));
            if(filen>0)
            {
                body.append(filebuf,(size_t)filen);
            }
            else if(filen<0)
            {
if(errno==EINTR)
continue;
status="500 Internal Server Error";
body="<h1>Read File Error</h1>";
break;

            }
            else{
break;
            }
        }
        close(filefd);
    }
}
if(status=="200 OK")
content_type=filetype;
        }
        
            string allow=path=="/echo"?"POST":"GET,HEAD";
      ev->send_buf=make_response(status,body,content_type,method=="HEAD",allow);
ev->method=method;
ev->path=path;
ev->response_status=status;



        ev->total=0;
        ev->callback=send_client;
        eventadd(EPOLLOUT,cfd,ev);
}

void access_log(struct epoll* ev)
{

    auto now=chrono::steady_clock::now();
auto duration=now-ev->start_time;

    auto elapsed=chrono::duration_cast<chrono::milliseconds>
    (duration).count();
time_t current=time(nullptr);
struct tm local{};

if(localtime_r(&current,&local)==nullptr)

{
 
    cerr<<"connot get log time"<<endl;
return;

}

stringstream line;
line<<put_time(&local,"%Y-%m-%d %H:%M:%S")
        <<" | "
        <<(ev->method.empty() ? "-" : ev->method)<<" "
        <<(ev->path.empty() ? "-" : ev->path)
        <<" | "<<ev->response_status
        <<" | sent="<<ev->total<<" bytes"
        <<" | elapsed="<<elapsed<<" ms";

 string text =line.str()+"\n";
 
 size_t total=0;
 while(total<text.size())
 {

ssize_t n=write(logfd,text.data()+total,text.size()-total);
if(n>0)
{
    total+=(size_t)n;

}
else if(n==-1)
{
    if(errno==EINTR)
    continue;
    else
    {
        perror("write access.log error");
        return;

    }
}


else{

    cerr<<"write access.log returned 0"<<endl;
    break;

}
 }
   

}

void send_client(int cfd,uint32_t,struct epoll* ev)
{
    while(ev->total<ev->send_buf.size())
    {
        ssize_t ret=send(cfd,ev->send_buf.data()+ev->total,
                         ev->send_buf.size()-ev->total,MSG_NOSIGNAL);
        if(ret==-1)
        {
            if(errno==EINTR)
                continue;
            // 暂时发不出去，等下一次 EPOLLOUT，继续从 total 发送。
            if(errno==EAGAIN || errno==EWOULDBLOCK)
                return;
            perror("send error");
            eventdel(ev);
            return;
        }
        if(ret==0)
        {
            eventdel(ev);
            return;
        }
        ev->total+=(size_t)ret;
        ev->last_active=chrono::steady_clock::now();


    }
    access_log(ev);

    eventdel(ev);
}

bool parse_args(int argc,char* argv[])
{

    if(argc>3)
    return false;
    if(argc>=2)
    {

        string value=argv[1];
if(value.empty())
{
    return false;

}
int number=0;
for(size_t i=0;i<value.size();i++)
{

    char ch=value[i];
    if(ch>'9'||ch<'0')
    {
        return false;

    }

    int digit=ch-'0';
    if(number>(65535-digit)/10)
    {

        return false;

    }
    number=number*10+digit;

}
if(number==0)
{
    return false;

}
port=number;

    }
if(argc==3)
root=argv[2];

struct stat info{};

if(stat(root.c_str(),&info)==-1)
{
    return false;

}

if(!S_ISDIR(info.st_mode))
{

    return false;

}

while(root.size()>1&&root.back()=='/')
root.pop_back();

return true;


}


int main(int argc,char* argv[])

{
if(!parse_args(argc,argv))
return 1;
struct sigaction action{};

action.sa_flags=0;
sigemptyset(&action.sa_mask);

action.sa_handler=stop_handler;
if(sigaction(SIGINT,&action,nullptr)==-1
||
sigaction(SIGTERM,&action,nullptr)==-1)
{
    perror("sigaction error");
    return 1;

}

    cout<<"WebServer starting..."<<endl;
    cout<<"port: "<<port<<endl;
    cout<<"root: "<<root<<endl;

    logfd=open("/home/lyf67/Webserver/access.log",
    O_WRONLY|O_CREAT|O_APPEND|O_CLOEXEC,0644);

    if(logfd==-1)
    {

        perror("open access.log error");
        return 1;

    }

    struct epoll_event events[Max_event];
    epfd=Epoll_create1(0);
    Initserver();
    while(!stop_server)
    {
        int ret=epoll_wait(epfd,events,Max_event,1000);
        if(ret==-1)
        {
            if(errno==EINTR)
                continue;
            perror("epoll_wait error");
            break;
        }

        if(stop_server)
        break;

for(int i=0;i<ret;i++)
{
    struct epoll *ev=(struct epoll*)events[i].data.ptr;
    if(events[i].events&(EPOLLERR|EPOLLHUP))
    {
        eventdel(ev);
        continue;
    }
    if(ev->status==1 && ev->callback)
        ev->callback(ev->fd,events[i].events,ev->arg);
}
auto now=chrono::steady_clock::now();
for(int i=0;i<Max_event;i++)
{
    if(g_event[i].status==0)
    continue;

if(g_event[i].callback==hander_client)
{
    auto esp=now-g_event[i].start_time;
    if(esp>=chrono::seconds(10))
{
    cout<<"request timeout"<<endl;

    struct epoll* ev=&g_event[i];
    string body="<h1>408 Request Timeout</h1>";

    ev->send_buf=make_response(
        "408 Request Timeout",
        body,
        "text/html; charset=utf-8"
    );
    ev->response_status="408 Request Timeout";

    ev->total=0;
    ev->callback=send_client;
    eventadd(EPOLLOUT,ev->fd,ev);

    continue;
}
}

auto idle=now-g_event[i].last_active;
//auto esp=now-g_event[i].last_active;
//auto seconds=chrono::duration_cast<chrono::seconds>(esp).count();

if(idle>chrono::seconds(30))
{
    cout<<"client timeout"<<endl;
    eventdel(&g_event[i]);
}

}

        }
        
    cout<<"server stopping..."<<endl;
for(int i=0;i<=Max_event;i++)
{

    if(g_event[i].status==1)
    eventdel(&g_event[i]);
}
close(epfd);
if(logfd!=-1)
{

    close(logfd);
    logfd=-1;
}
cout<<"server stopped"<<endl;

    return 0;
}
