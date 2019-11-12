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
#include"splitstring.h"

#define INET_ADDRSTRLEN 16
#define BUFFSIZE 8092

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

struct node_data routing(struct node_data requesting_node);
void sendrequest(string message,string buddy_ip,string buddy_port,int control);

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
    unordered_map<string,string> local_hashtable;
};

struct node_structure node_obj;
void printroutable(struct node_structure node_obj);
int b;

// function to serialize state tables
void printvec(std::vector<string> v)
{
    int j=0;
    for(int i=0;i<v.size();i++)
    {
        cout<<"number "<<j++<<" "<<v[i]<<endl;
    }
}
string serialize_tables(struct node_structure sending_node)
{
    string final_result="";
    string leafset_str="";
    string neighbourhoodset_str="";
    string routing_table_str="";

    int i=0;
    while(i<4)
    {
        leafset_str+=sending_node.leafset[i].nodeid;
        leafset_str+=":"+sending_node.leafset[i].ip;
        leafset_str+=":"+sending_node.leafset[i].port+":";
        i++;
    }
    i=0;
    while(i<4)
    {
        neighbourhoodset_str+=sending_node.neighbourhoodset[i].nodeid;
        neighbourhoodset_str+=":"+sending_node.neighbourhoodset[i].ip;
        neighbourhoodset_str+=":"+sending_node.neighbourhoodset[i].port+":";
        i++;
    }
    for(i =0;i<8;i++)
    {
        for(int j=0;j<16;j++)
        {
            //cout<<node_obj.routing_table[i][j].ip<<" "<<node_obj.routing_table[i][j].nodeid<<" "<<node_obj.routing_table[i][j].port<<" \t";
            routing_table_str+=node_obj.routing_table[i][j].ip+":"+node_obj.routing_table[i][j].nodeid+":"+node_obj.routing_table[i][j].port+":";
        }

        //cout<<"\n";
    }
    //cout<<routing_table_str<<endl;
    
    final_result=leafset_str+neighbourhoodset_str+routing_table_str;
    cout<<"in serialize"<<endl;
    cout<<final_result<<endl;

    return final_result;
}
void deserialize_tables(string serialstring)
{
    
    std::vector<string> serialstring_vec=splitstring(serialstring,':');
    //printvec(serialstring_vec);
    cout<<"size is "<<serialstring_vec.size()<<endl;
    struct node_structure temp;
    int i=0,j=0;
    while(i<12)
    {
        if((i%3)==0)
        temp.leafset[j].nodeid=serialstring_vec[i];
        else if((i%3)==1)
        temp.leafset[j].ip=serialstring_vec[i];
        else
        temp.leafset[j++].port=serialstring_vec[i];

        i++;
    }
    j=0;
    while(i<24)
    {
        if((i%3)==0)
        temp.neighbourhoodset[j].nodeid=serialstring_vec[i];
        else if((i%3)==1)
        temp.neighbourhoodset[j].ip=serialstring_vec[i];
        else
        temp.neighbourhoodset[j++].port=serialstring_vec[i];

        i++;

    }
    j=0;
    int k=0;
    while(i<408 && j<8)
    {
        if((i%3)==0)
        temp.routing_table[j][k].nodeid=serialstring_vec[i];
        else if((i%3)==1)
        temp.routing_table[j][k].ip=serialstring_vec[i];
        else
        temp.routing_table[j][k++].port=serialstring_vec[i];

        if(k>=16)
        {
            k=0;
            j++;
        }

        i++;

    }
    cout<<"printing in deserialize"<<endl;
    //printroutable(temp);

}
void processrequest(int cid)
{
    int command;
    recv(cid,( void*)&command,sizeof(command),0);

    cout<<"Command recv in server is "<<command<<"\n";
    char buffer[BUFFSIZE];

    if(command==0)
    {
        //pastry node joining

        cout<<"Received join request\n";
        struct node_data nodedata;
        recv(cid,buffer,BUFFSIZE,0);
        stringstream ss(buffer);
        ss>>nodedata.nodeid>>nodedata.ip>>nodedata.port;

        cout<<"Buffer is "<<buffer<<"\n";
        struct node_data final_node=routing(nodedata);
        
        string temp="";
        
        //sendrequest(temp,nodedata.ip,nodedata.port,1);

        if(final_node.nodeid==node_obj.nodeid)
        {
            cout<<"Self node\n";
            string temp=serialize_tables(node_obj);
            sendrequest(temp,nodedata.ip,nodedata.port,1);
        }

        else
        {
            cout<<"other\n";
            string temp=serialize_tables(node_obj);
            sendrequest(temp,nodedata.ip,nodedata.port,1);

            string data(buffer);
            sendrequest(data,final_node.ip,final_node.port,2);   
        }
        

    }
    else if(command==1)
    {
        //update your table
        cout<<"Recieved reques for updating table\n";
    }
    else if(command==2)
    {
        //Handle join request

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
    int len=10,cid;
    cout<<node_obj.port<<"\n";
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
            cout<<node_obj.routing_table[i][j].ip<<" "<<node_obj.routing_table[i][j].nodeid<<" "<<node_obj.routing_table[i][j].port<<" \t";
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

    cout<<"Buddy port "<<buddy_port<<"\n";
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
    strcpy(data,message.c_str());
    send(sockid,(const void*)data,BUFFSIZE,0);
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
void printroutable(struct node_structure node_obj)
{
    for(int i =0;i<8;i++)
    {
        for(int j=0;j<16;j++)
        {
            cout<<node_obj.routing_table[i][j].ip<<" "<<node_obj.routing_table[i][j].nodeid<<" "<<node_obj.routing_table[i][j].port<<" \t";
        }
        cout<<"\n";
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
            cout<<"temp is "<<temp<<"\n";
            sendrequest(temp,buddy_ip,buddy_port,0);
        }
        if(choice.compare("serialize")==0)
        {
            string a=serialize_tables(node_obj);
            cout<<"a is "<<a;
            deserialize_tables(a);
        }
        if(choice.compare("printroutable")==0)
        {
            //printroutable(node_obj);
        }
        
    }
}
