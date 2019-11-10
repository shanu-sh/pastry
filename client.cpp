#include<iostream>
#include<unistd.h>
#include<pthread.h>
#include<openssl/sha.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<vector>
#include<cmath>

using namespace std;

struct node_data
{
    string nodeid;
    string ip;
    string port;
};
struct node_data getdefaul_node()
{
    struct node_data defaul;
    defaul.nodeid="...";
    defaul.ip=".";
    defaul.port="..";
    return defaul;

}

 vector<vector<struct node_data>> get_table()
 {
    vector<vector<struct node_data>>v(8,vector<struct node_data>(16,getdefaul_node()));
    return  v;
 }

std::vector<struct node_data> getleaf()
{
    std::vector<struct node_data> leafset(4,getdefaul_node());
    return leafset;
}

std::vector<struct node_data> getneighbour()
{
    std::vector<struct node_data> neighbourhoodset(4,getdefaul_node());
    return neighbourhoodset;
}

struct node_structur{
    string ip;
    string port;
    string nodeid;
    struct node_data defaul = getdefaul_node();
    int n=4;
    vector<vector<struct node_data>> routing_table=get_table();
    std::vector<struct node_data> leafset=getleaf();
    std::vector<struct node_data> neighbourhoodset=getneighbour();
};


int b;
#define BUFFSIZE 512

void processrequest(int cid)
{
    int command;
    recv(cid,( void*)&command,sizeof(command),0);

    char buffer[BUFFSIZE];

    if(command==0)
    {
        //pastryInit
    }
    else if(command==1)
    {
        //route(msg,key)
    }
    else if(command==2)
    {
        //deliver(msg,key)
    }
    else if(command==3)
    {
        //forward(msg,key,nextId)
    }
    else if(command==4)
    {
        //newLeafs(leafSet)
    }
}

void *server(void * arg)
{
    //server started

    int sockid=socket(AF_INET,SOCK_STREAM,0);
    int len,cid;

    struct host_data *data=(struct node_data*)arg;
    struct sockaddr_in addr;

    addr.sin_family=AF_INET;
    addr.sin_port=htons(stoi(data->port));
    addr.sin_addr.s_addr=inet_addr(data->ip.c_str());

    len=sizeof(addr);
    bind(sockid,(struct sockaddr*)&addr,sizeof(addr));

    listen(sockid,3);

    int count=0;

    while(1)
    {
        cid=accept(sockid,(struct sockaddr*)&addr,(socklen_t*)&len);
        processrequest(cid);   
    }
}

int main(int argc,char **argv)
{
    bool bootstrapnode=false;
    string new_ip;
    string new_port;
    
    string node_ip;
    string node_port;

    string choice;

    b=2;
    int entry=pow(2,b);

    if(argc<3)
    {
        cout<<"Please enter 3 arguments\n";
        return 1;
    }

    if(argc==3)
    {
        bootstrapnode=true;
        new_ip=argv[1];
        new_port=argv[2];
    }
    else if(argc==5)
    {
        bootstrapnode=false;
        new_ip=argv[1];
        new_port=argv[2];
        node_ip=argv[3];
        node_port=argv[4];
    }

    pthread_t id;
    int data;

    pthread_create(&id,NULL,server,(void*)&data);
    pthread_detach(id);

    if(bootstrapnode)
    {
        vector<node_data> temp(entry);
    }
    else
    {
        /* code */
    }
    

    while(1)
    {
        cin>>choice;

        
    }
}