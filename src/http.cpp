#include"http.h"

using namespace std;

string trim(const string& message)
{
size_t first=message.find_first_not_of(" \t");
if(first==string::npos)
return "";
size_t last=message.find_last_not_of(" \t");

string return_message=message.substr(first,last-first+1);

return return_message;
}





bool parse_headers(const string &message,map<string,string>&headers)
{
headers.clear();
size_t pos=message.find("\r\n");
if(pos==string::npos)
return false;

pos+=2;
while(1)
{

size_t end=message.find("\r\n",pos);
if(end==string::npos)
{
    return false;
}
else if(end==pos)
return true;
string line=message.substr(pos,end-pos);
size_t colon=line.find(':');
if(colon==string::npos||colon==0)
return false;
string name=line.substr(0,colon);
string value=trim(line.substr(colon+1));

string symbols="!#$%&*'+-.^_|~`";

for(size_t i=0;i<name.size();i++)
{

    char ch=name[i];
    bool letter=(ch>='a'&&ch<='z')||(ch>='A'&&ch<='Z');
    bool digit=(ch>='0'&&ch<='9');
    if(!letter&&!digit&&symbols.find(ch)==string::npos)
    return false;
    if(ch>='A'&&ch<='Z')
    {
        name[i]=(char)(ch-'A'+'a');

    }
}

for(size_t i=0;i<value.size();i++)
{
   unsigned char ch=(unsigned char)value[i];
    if((ch<32&&ch!='\t')||ch==127)
    return false;

}

if(headers.find(name)!=headers.end())
return false;

headers[name]=value;
pos=end+2;
}


}












int parse_content_length(const string& value,size_t& length)
{
length=0;
const size_t max_body=1024*1024;

if(value.empty())
{
    return 400;
}

for(size_t i=0;i<value.size();i++)
{
if(value[i]>'9'||value[i]<'0')
return 400;

}


for(size_t i=0;i<value.size();i++)
    {
        size_t digit=(size_t)(value[i]-'0');

        if(length>(max_body-digit)/10)
            return 413;

        length=length*10+digit;
    }
return 200;

}









string make_response(const string &status,const string &body,const string &Content_type,bool head,const string &allow)
{

string response;
response+="HTTP/1.1 "+status+"\r\n";
response+="Content-Type: "+Content_type+"\r\n";
response+="Content-Length: "+to_string(body.size())+"\r\n";
if(status=="405 Method Not Allowed")
{
    response+="Allow: "+allow+"\r\n";
}

response+="Connection: close\r\n";
response+="\r\n";
if(!head)
response+=body;

return response;

}