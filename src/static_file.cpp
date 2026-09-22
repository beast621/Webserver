#include"static_file.h"
#include<fcntl.h>
#include<unistd.h>
#include<sys/stat.h>
#include<cerrno>

using namespace std;

int read_static_file(const string& filename,string& body)
{
    body.clear();

    int filefd=open(filename.c_str(),O_RDONLY|O_NONBLOCK|O_CLOEXEC);
    if(filefd==-1)
    {
        if(errno==ENOENT || errno==ENOTDIR)
            return 404;

        return 500;
    }

    struct stat info{};
    if(fstat(filefd,&info)==-1)
    {
        close(filefd);
        return 500;
    }

    if(!S_ISREG(info.st_mode))
    {
        close(filefd);
        return 404;
    }

    const size_t max_file=4*1024*1024;
    char buf[8192];
    int result=200;

    while(1)
    {
        ssize_t n=read(filefd,buf,sizeof(buf));

        if(n>0)
        {
            if((size_t)n>max_file-body.size())
            {
                result=500;
                break;
            }

            body.append(buf,(size_t)n);
        }
        else if(n==0)
        {
            break;
        }
        else
        {
            if(errno==EINTR)
                continue;

            result=500;
            break;
        }
    }

    close(filefd);

    if(result!=200)
        body.clear();

    return result;
}