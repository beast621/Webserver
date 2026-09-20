#pragma once


#include<thread>
#include<utility>
#include<mutex>
#include<condition_variable>
#include<queue>
#include<vector>
#include<functional>
#include<stdexcept>
#include<iostream>
using namespace std;
class Threadpool
{

    private:
vector<thread>workers;

queue<function<void()>>tasks;
mutex mtx;
condition_variable cv;
bool stopping=false;

void stop()
{
{
    lock_guard<mutex>lock(mtx);
stopping=true;

}
     cv.notify_all();
     for(size_t i=0;i<workers.size();i++)
     {
        if(workers[i].joinable())
        workers[i].join();

     }
     

}
void worker()
{

while(1)
{
function<void()>task;
{
    unique_lock<mutex>lock(mtx);
    cv.wait(lock,[this]
        ()
        {
            return stopping||!tasks.empty();
        }
    );
    if(stopping&&tasks.empty())
    {
        return;
    }
    task=move(tasks.front());
    tasks.pop();

}

try
{
    task();
}
catch(const std::exception& e)
{
    cerr <<"task error"<< e.what() <<endl;
}

catch(...)
{
    cerr<<"unknown task error"<<endl;

}

}

}

public:
explicit Threadpool(const size_t& count)
{

    if(count==0)
    {
        throw invalid_argument("thread count must be positive");


    }
    try
    {
        for(size_t i=0;i<count;i++)
        {
            workers.emplace_back(
                [this]()
                {
                    worker();
                }
            );
        }
            
        
    }
    catch(...)
    {
        stop();
        throw;

    }
}
    ~Threadpool()
    {
        stop();

    }
    
    Threadpool(const Threadpool&)=delete;
    Threadpool& operator=(const Threadpool&)=delete;


bool add_task(function<void()> task)
{

    if(!task)
    return false;

    {
        lock_guard<mutex>lock(mtx);
        
        if(stopping||tasks.size()>=1024)
        return false;
        tasks.push(move(task));

    }
    cv.notify_one();
    return true;

}






};