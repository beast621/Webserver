#include<iostream>
#include<atomic>
#include"thread_pool.h"
using namespace std;

int main()
{

atomic<int> count{0};

{
Threadpool pool(4);


for(int i=0;i<100;i++)
{
bool ok=pool.add_task(
    [&count]()
    {
        count.fetch_add(1);

    }
);

if(!ok)
{
    cerr<<"add task failed"<<endl;
    return 1;

}

}


}

cout<<"complete: "<<count.load()<<endl;
if(count.load()!=100)
{
    cerr<<"test failed"<<endl;
    return 1;

}
cout<<"thread pool test passed"<<endl;
return 0;

}