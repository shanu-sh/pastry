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

 vector<vector<struct node_data>> get_table(){
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

void *server(void * arg)
{

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