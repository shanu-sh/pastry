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
#include<sstream>

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

        if((ifa->ifa_addr->sa_family == AF_INET) && (ifa->ifa_name[0] == 'w' || ifa->ifa_name[0] == 'e')){
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
    defaul.nodeid="-1";
    defaul.ip="-2";
    defaul.port="-3";
    return defaul;

}

vector<vector<struct node_data>> get_table()
{
    vector<vector<struct node_data>> v(8,vector<struct node_data>(16,getdefaul_node()));
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
        //pastry node joining

        struct node_data nodedata;
        recv(cid,buffer,BUFFSIZE,0);
        stringstream ss(buffer);
        ss>>nodedata.nodeid>>nodedata.ip>>nodedata.port;

        //struct node_data final_node=route(nodedata);
        //

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

    struct sockaddr_in addr;

    addr.sin_family=AF_INET;
    addr.sin_port=htons(stoi(node_obj.port));
    addr.sin_addr.s_addr=inet_addr(node_obj.ip.c_str());

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

struct node_data isleaf(struct node_data node)
{
    int i,j,id,mdiff,temp;
    int lower=-1,upper=-1;

    node_data default_node=getdefaul_node();
    node_data res;

    stringstream ss;

    for(auto x:node_obj.leafset)
    {
        if(x.nodeid.compare(default_node.nodeid)!=0)
        {
            if(lower==-1)
            {
                ss<<hex<<x.nodeid;
                ss>>i;
                lower=1;
                upper=1;
                j=i;
            }
            else if(lower==1)
            {
                ss<<hex<<x.nodeid;
                ss>>j;
            }
        }
    }

    if(lower ==-1 && upper==-1)
        return default_node;

    ss<<hex<<node.nodeid;
    ss>>id;

    if(id>=i&&id<=j)
    {
        res.ip=node_obj.ip;
        res.nodeid=node_obj.nodeid;
        res.port=node_obj.port;

        ss<<hex<<res.nodeid;
        ss>>temp;
        mdiff=abs(temp-id);
        
        for(auto x:node_obj.leafset)
        {
            ss<<hex<<x.nodeid;
            ss>>temp;
            if(abs(id-temp)<mdiff)
            {
                mdiff=abs(id-temp);
                res=x;
            }
        }
    }
    return res;
}

void populatestate()
{
    int i=0,j;
    
    while(i<8)
    {
        stringstream ss;

        struct node_data temp;

        temp.ip=node_obj.ip;
        temp.nodeid=node_obj.nodeid;
        temp.port=node_obj.port;

        ss<<hex<<temp.nodeid[i];
        ss>>j;
        node_obj.routing_table[i][j]=temp;
        i++;
    }

    for(i =0;i<8;i++)
    {
        for(j=0;j<16;j++)
        {
            cout<<node_obj.routing_table[i][j].ip<<" "<<node_obj.routing_table[i][j].nodeid<<" \t";
        }
        cout<<"\n";
    }
   
}


void sendrequest(string message,string buddy_ip,string buddy_port,int control)
{
    int sockid,status;
    string temp="";

    sockid=socket(AF_INET,SOCK_STREAM,0);
    if(sockid<0)
    {
        perror("Error in socket creation\n");
        return;
    }

    struct sockaddr_in serveraddr;
    serveraddr.sin_family=AF_INET;
    serveraddr.sin_port=htons(stoi(buddy_port));
    serveraddr.sin_addr.s_addr=inet_addr(buddy_ip.c_str());
    
    status=connect(sockid,(struct sockaddr*)&serveraddr,sizeof(serveraddr));

    if(status<0)
    {
        perror("Unable to reach server\n");
        return;  
    }

    send(sockid,(const void*)&control,sizeof(control),0);
    char data[BUFFSIZE];
    strcpy(data,temp.c_str());
    send(sockid,(const void*)data,sizeof(data),0);
}

struct node_data routing(struct node_data requesting_node){
    
    struct node_data requesting_node_leaf = isleaf(requesting_node); 
    if( requesting_node_leaf.nodeid.compare("-1")!=0){
        return requesting_node_leaf;
    }
    else{
        int i=0;
        for( i=0;i<8;i++){
            if(node_obj.nodeid[i]!=requesting_node.nodeid[i]){
                break;
            }

        }

        stringstream ss;
        int hexval;
        
        string requesting_node_id=requesting_node.nodeid;
        

        ss<<std::hex<<requesting_node_id[i]<<" ";
        ss>>hexval;
        

        if(node_obj.routing_table[i][hexval].nodeid.compare("-1")!=0){
            return node_obj.routing_table[i][hexval] ;
        }
        else{
            int flag=0;
            int difference_min;
            int position;
            for(int j=0;j<16;j++) {
                if(node_obj.routing_table[i][hexval].nodeid.compare("-1")!=0) {
                     stringstream ss1,ss2;
                     int hexval1,hexval2;
        
                     string requesting_node_id=requesting_node.nodeid;
                     string routing_table_id=node_obj.routing_table[i][j].nodeid;
        
                    ss1<<std::hex<<requesting_node_id<<" ";
                    ss1>>hexval1;
                    ss2<<std::hex<<routing_table_id<<" ";
                    ss1>>hexval2;
                    if(flag==0) { 
                        difference_min = abs(hexval1-hexval2);
                         position = j; 
                    }
                    else{
                        int  new_min = abs(hexval1-hexval2);
                        if(new_min < difference_min) { 
                            difference_min = new_min;
                            position = j; 
                        }
                    }
                }
                   
            }
        }
    }



}

int main(int argc,char **argv)
{
    
    pthread_t id;
    int data;
    string choice;

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
            node_obj.nodeid=generate_md5(input_ip+input_port).substr(0,8);
            cout<<" IP is :"<<input_ip<<endl;
            cout<<"port is : "<<input_port<<endl;
            cout<<" node_id is:"<<node_obj.nodeid<<endl;

        }

        if(choice.compare("create")==0)
        {
            populatestate();
            int temp=0;
            pthread_create(&id,NULL,server,(void*)&temp);
            pthread_detach(id);
        }

        if(choice.compare("join")==0)
        {
            string buddy_ip,buddy_port,temp;
            cin>>buddy_ip>>buddy_port;
            temp=node_obj.nodeid+" "+node_obj.ip+" "+node_obj.port;
            sendrequest(temp,buddy_ip,buddy_port,0);
        }
    }
}
