#include"static_file.h"
#include<iostream>
#include<string>
#include<cstdlib>
#include<fcntl.h>
#include<unistd.h>
#include<cerrno>

using namespace std;

bool write_all(int fd,const string& body)
{
    size_t total=0;

    while(total<body.size())
    {
        ssize_t n=write(fd,body.data()+total,body.size()-total);

        if(n>0)
        {
            total+=(size_t)n;
        }
        else if(n==-1 && errno==EINTR)
        {
            continue;
        }
        else
        {
            return false;
        }
    }

    return true;
}

int main()
{
    char dirname[]="/tmp/webserver-static-test-XXXXXX";

    if(mkdtemp(dirname)==nullptr)
    {
        perror("mkdtemp error");
        return 1;
    }

    string directory=dirname;
    string filename=directory+"/test.txt";
    int failed=0;

    // 创建一个普通文件。
    int fd=open(filename.c_str(),O_WRONLY|O_CREAT|O_EXCL,0600);

    if(fd==-1)
    {
        perror("open error");
        rmdir(directory.c_str());
        return 1;
    }

    string expected="hello webserver\n";

    if(!write_all(fd,expected))
    {
        perror("write error");
        close(fd);
        unlink(filename.c_str());
        rmdir(directory.c_str());
        return 1;
    }

    close(fd);

    // 1. 正常读取，原来的 body 内容应该被清除。
    string body="old data";
    int code=read_static_file(filename,body);

    if(code!=200 || body!=expected)
    {
        cerr<<"normal file test failed"<<endl;
        failed++;
    }

    // 2. 不存在的文件返回 404。
    body="old data";
    code=read_static_file(directory+"/missing.txt",body);

    if(code!=404 || !body.empty())
    {
        cerr<<"missing file test failed"<<endl;
        failed++;
    }

    // 3. 目录不能作为普通文件读取。
    body="old data";
    code=read_static_file(directory,body);

    if(code!=404 || !body.empty())
    {
        cerr<<"directory test failed"<<endl;
        failed++;
    }

    // 调整刚才创建的测试文件大小。
    fd=open(filename.c_str(),O_WRONLY);

    if(fd==-1)
    {
        perror("open error");
        unlink(filename.c_str());
        rmdir(directory.c_str());
        return 1;
    }

    const off_t max_file=4*1024*1024;

    // 4. 恰好 4MB，允许读取。
    if(ftruncate(fd,max_file)==-1)
    {
        perror("ftruncate error");
        failed++;
    }
    else
    {
        code=read_static_file(filename,body);

        if(code!=200 || body.size()!=(size_t)max_file)
        {
            cerr<<"size boundary test failed"<<endl;
            failed++;
        }
    }

    // 5. 超过 4MB，按当前实现返回 500，并清空正文。
    if(ftruncate(fd,max_file+1)==-1)
    {
        perror("ftruncate error");
        failed++;
    }
    else
    {
        code=read_static_file(filename,body);

        if(code!=500 || !body.empty())
        {
            cerr<<"oversized file test failed"<<endl;
            failed++;
        }
    }

    close(fd);

    // 只删除这个测试自己创建的文件和目录。
    unlink(filename.c_str());
    rmdir(directory.c_str());

    if(failed!=0)
    {
        cerr<<failed<<" tests failed"<<endl;
        return 1;
    }

    cout<<"static file tests passed"<<endl;
    return 0;
}