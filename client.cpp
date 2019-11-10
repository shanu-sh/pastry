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
#include<bits/stdc++.h>
#include <netdb.h>
#include <ifaddrs.h>
#include"md5.h"
#define INET_ADDRSTRLEN 16


using namespace std;
std::string extractPublicIP(){
    struct ifaddrs *ifaddr, *ifa;
    int family, s, n;
    char host[INET_ADDRSTRLEN];

    if (getifaddrs(&ifaddr) == -1) {
        perror("getifaddrs");
        exit(EXIT_FAILURE);
    }
    ifa = ifaddr;
    while(ifa!= NULL){
        if(ifa->ifa_addr == NULL)
            continue;

        if((ifa->ifa_addr->sa_family == AF_INET) && ifa->ifa_name[0] == 'w'){
            s = getnameinfo(ifa->ifa_addr, sizeof(struct sockaddr_in), host, INET_ADDRSTRLEN, NULL, 0, NI_NUMERICHOST);
            // std::cout<<host<<"\n";
        }
        ifa = ifa->ifa_next;
    }
    cout<<string(host)<<endl;
    return std::string(host);
}

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

struct node_structure{
    string ip;
    string port;
    string nodeid;
    struct node_data defaul = getdefaul_node();
    int n=4;
    vector<vector<struct node_data>> routing_table=get_table();
    std::vector<struct node_data> leafset=getleaf();
    std::vector<struct node_data> neighbourhoodset=getneighbour();
    unordered_map<string,string>local_hashtable;
};

struct node_structure node_obj;
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

    struct node_data *data=(struct node_data*)arg;
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
    
    pthread_t id;
    int data;
    string choice;

    //pthread_create(&id,NULL,server,(void*)&data);
    //pthread_detach(id);

    
    

    while(1)
    {
        cin>>choice;
        if(choice.compare("port")==0)
        {
            string input_port;
            cin>>input_port;
            string input_ip=extractPublicIP();
            node_obj.port=input_port;
            node_obj.ip=input_ip;
            node_obj.nodeid=generate_md5(input_ip+input_port);
            cout<<" IP is :"<<input_ip<<endl;
            cout<<"port is : "<<input_port<<endl;
            cout<<" node_id is:"<<node_obj.nodeid<<endl;

        }

        
    }
}
