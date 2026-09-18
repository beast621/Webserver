#include<iostream>
#include<string>
#include<map>
#include"http.h"
using namespace std;

int failed=0;

void check(bool condition,const string& name)
{


    if(condition)
    {
        cout<<"pass: "<<name<<endl;

    }

    else{

        cout<<"fail: "<<name<<endl;
        failed++;

    }
}

int main()
{
check(trim(" \t hello   \t")=="hello","trim spaces");
check(trim("  \t    ")=="","trim empty");

map<string,string>headers;
string request=
"GET / HTTP/1.1\r\n"
"host: localhost:9527\r\n"
"Connection: close\r\n"
"\r\n";

bool result=parse_headers(request,headers);

check(result,"parse headers");

check(headers["host"]=="localhost:9527","header name lowercase");
check(headers["connection"]=="close","header value");

check(!parse_headers(
        "GET / HTTP/1.1\r\n"
        "Host localhost\r\n"
        "\r\n",
        headers
    ),"reject missing colon");


    check(!parse_headers(
        "GET / HTTP/1.1\r\n"
        "Host: localhost\r\n"
        "host: example.com\r\n"
        "\r\n",
        headers
    ),"reject duplicate header");


    size_t length=0;

    check(parse_content_length("5",length)==200&&length==5,"length normal");
    check(parse_content_length("0",length)==200&&length==0,"length zero");
    check(parse_content_length("",length)==400,"reject empty length");
     check(parse_content_length("-1",length)==400,
          "reject negative length");

    check(parse_content_length("12a",length)==400,
          "reject invalid length");

    check(parse_content_length("1048576",length)==200 &&
          length==1048576,
          "length at limit");

    check(parse_content_length("1048577",length)==413,
          "reject oversized length");

          string response=make_response("200 OK","hello","text/plain");
          check(response.find("Content-Length: 5\r\n")!=string::npos,"response length");

size_t pos=response.find("\r\n\r\n");

    check(pos!=string::npos && response.substr(pos+4)=="hello",
          "response body");

    string head=make_response("200 OK","hello","text/plain",true);

    check(head.find("Content-Length: 5\r\n")!=string::npos,
          "HEAD length");

    pos=head.find("\r\n\r\n");

    check(pos!=string::npos && head.size()==pos+4,
          "HEAD has no body");

    string error=make_response(
        "405 Method Not Allowed",
        "not allowed",
        "text/plain",
        false,
        "POST"
    );

    check(error.find("Allow: POST\r\n")!=string::npos,
          "405 allowed methods");


          cout<<"failed: "<<failed<<endl;

          return failed==0 ? 0:1;

}