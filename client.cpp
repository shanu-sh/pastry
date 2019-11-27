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
#define ll long long

using namespace std;

const int r = 8;//ro
const int c = 16;//cols in routing table

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
    node_data(){};
    node_data(string nodeid, string ip, string port):nodeid(nodeid), ip(ip), port(port){}
};
inline bool operator < (const node_data& lhs, const node_data& rhs){
    return stoll(lhs.nodeid, 0 , 16) < stoll(rhs.nodeid, 0, 16);
}
inline bool operator > (const node_data& lhs, const node_data& rhs){
    return stoll(lhs.nodeid, 0, 16) > stoll(rhs.nodeid, 0, 16);
}
inline bool operator == (const node_data& lhs, const node_data& rhs){
    return stoll(lhs.nodeid, 0, 16) == stoll(rhs.nodeid, 0, 16);
}
inline bool operator <= (const node_data& lhs, const node_data& rhs){
    return (lhs < rhs) or (lhs == rhs);
}
inline bool operator >= (const node_data& lhs, const node_data& rhs){
    return (lhs > rhs) or (lhs == rhs);
}


struct node_data getdefaul_node()
{
    struct node_data defaul;
    defaul.nodeid="-1";
    defaul.ip="-2";
    defaul.port="-3";
    return defaul;

}
class less_than_cmp{
public:
    bool operator()(const struct node_data& lhs, const struct node_data& rhs){
        return stoll(lhs.nodeid, 0 , 16) < stoll(rhs.nodeid, 0, 16);
    }
};
class greater_than_cmp{
public:
    bool operator()(const struct node_data& lhs, const struct node_data& rhs){
        return stoll(lhs.nodeid, 0 , 16) > stoll(rhs.nodeid, 0, 16);
    }
};

struct node_data routing(struct node_data requesting_node);
void sendrequest(string message,string buddy_ip,string buddy_port,int control);
void copy_to_routing_table(struct node_structure received_table);
void sharetables();
void getkey(string key, struct node_data temp_node);
void setkey(string key,string value);

vector<vector<struct node_data>> get_table()
{
    vector<vector<struct node_data>> v(r,vector<struct node_data>(c, getdefaul_node()));
    return  v;
}

std::vector<struct node_data> getleaf()
{
    std::vector<struct node_data> leafset(c,getdefaul_node());
    return leafset;
}

std::vector<struct node_data> getneighbour()
{
    std::vector<struct node_data> neighbourhoodset(c,getdefaul_node());
    return neighbourhoodset;
}

struct node_structure{
    string ip;
    string port;
    string nodeid;
    struct node_data defaul = getdefaul_node();
    vector<vector<struct node_data>> routing_table=get_table();
    std::vector<struct node_data> leafset=getleaf();
    std::vector<struct node_data> neighbourhoodset=getneighbour();
    unordered_map<string,string> local_hashtable;
};

struct node_structure node_obj;
void printroutable(struct node_structure node_obj);

void update_leaf_set(struct node_structure received_node){
    set<node_data, less_than_cmp> lesser_nodes;
    set<node_data, greater_than_cmp> greater_nodes;
    node_data curr_node = node_data(node_obj.nodeid, node_obj.ip, node_obj.port);
    node_data recv_node = node_data(received_node.nodeid, received_node.ip, received_node.port);

    //insert recv_node as candidate for leaf_set of node_obj
    if(recv_node < curr_node){
        lesser_nodes.insert(recv_node);
    }
    else if(recv_node > curr_node){
        greater_nodes.insert(recv_node);
    }

    for(auto u : received_node.leafset){
        if(u.nodeid.compare("-1")==0)
            continue;
        if(u < curr_node){
            lesser_nodes.insert(u);
        }
        else if( u > curr_node){
            greater_nodes.insert(u);
        }
    }
    for(auto u : node_obj.leafset){
        if(u.nodeid.compare("-1")==0)
            continue;
        if(u < curr_node){
            lesser_nodes.insert(u);
        }
        else if(u > curr_node){
            greater_nodes.insert(u);
        }
    }
    int i=0;
    for(auto it : lesser_nodes){
        node_obj.leafset[i++] = it;
        if(i==c/2) break;
    }
    for(auto it : greater_nodes){
        node_obj.leafset[i++] = it;
        if(i==c) break;
    }
}

// function to serialize state tables

string serialize_tables(struct node_structure sending_node)
{
    string final_result="";
    string leafset_str="";
    string neighbourhoodset_str="";
    string routing_table_str="";

    int i=0;
    while(i<c)
    {
        leafset_str+=sending_node.leafset[i].nodeid;
        leafset_str+=":"+sending_node.leafset[i].ip;
        leafset_str+=":"+sending_node.leafset[i].port+":";
        i++;
    }
    i=0;
    while(i<c)
    {
        neighbourhoodset_str+=sending_node.neighbourhoodset[i].nodeid;
        neighbourhoodset_str+=":"+sending_node.neighbourhoodset[i].ip;
        neighbourhoodset_str+=":"+sending_node.neighbourhoodset[i].port+":";
        i++;
    }
    for(i =0;i<r;i++)
    {
        for(int j=0;j<c;j++)
        {
            //cout<<node_obj.routing_table[i][j].ip<<" "<<node_obj.routing_table[i][j].nodeid<<" "<<node_obj.routing_table[i][j].port<<" \t";
            routing_table_str+=node_obj.routing_table[i][j].nodeid+":"+node_obj.routing_table[i][j].ip+":"+node_obj.routing_table[i][j].port+":";
        }

        //cout<<"\n";
    }
    //cout<<routing_table_str<<endl;
    final_result=sending_node.ip+":";
    final_result+=sending_node.port+":";
    final_result+=sending_node.nodeid+":";
    final_result+=leafset_str+neighbourhoodset_str+routing_table_str;
    //cout<<"in serialize"<<endl;
    
    return final_result;
}
struct node_structure deserialize_tables(string serialstring)
{
    
    std::vector<string> serialstring_vec=splitstring(serialstring,':');
    cout<<"size is "<<serialstring_vec.size()<<endl;
    struct node_structure temp;
    temp.ip=serialstring_vec[0],temp.port=serialstring_vec[1],temp.nodeid= serialstring_vec[2];

    int i=3,j=0;
    while(i<(48+3))
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
    while(i<(96+3))
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
    while(i<(408+3+72) && j<8)
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
   // cout<<"printing in deserialize"<<endl;
    
    return temp;
}

void printleaf()
{
    cout<<"In leafset\n"<<node_obj.leafset.size();
    for(auto x:node_obj.leafset)
    {
        cout<<"Leafset\n";
        cout<<x.ip<<" "<<x.port<<" "<<x.nodeid<<"\n";
    }
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

        if(final_node.nodeid==node_obj.nodeid)
        {
            cout<<"Self node\n";
            string temp=serialize_tables(node_obj);
            temp=temp+" 1 1 1";
            sendrequest(temp,nodedata.ip,nodedata.port,1);
        }

        else
        {
            cout<<"other\n";
            string temp=serialize_tables(node_obj);
            temp=temp+" 1 0 1";
            cout<<"requesting port is "<<nodedata.port<<"\n";
            sendrequest(temp,nodedata.ip,nodedata.port,1);

            string data(buffer);
            data=data+" 2 ";
            cout<<"Final port is "<<final_node.port<<"\n";
            sendrequest(data,final_node.ip,final_node.port,2);   
        }
        

    }
    else if(command==1)
    {
        //update your table

        int buddy,termination,hopcount;
        cout<<"Recieved reques for updating table\n";

        char BUFFER[BUFFSIZE];
        recv(cid,( void*)&BUFFER,sizeof(BUFFER),0);
        cout<<"Recieved value is "<<BUFFER<<"\n";

        char tmp[BUFFSIZE];
        stringstream ss(BUFFER);
        ss>>tmp;
        ss>>buddy>>termination>>hopcount;
        node_structure temp=deserialize_tables(tmp);

        cout<<"\n";
       
        cout<<buddy<<" "<<termination<<" "<<hopcount<<"\n";

        update_leaf_set(temp);
        printleaf();

        copy_to_routing_table(temp);

        cout<<"Your updating table\n";

        printroutable(node_obj);

        if(termination==1)
        {
            sharetables();
            cout<<"\nJoining complete\n";
        }
        
    }
    else if(command==2)
    {
        //Handle join request
        char BUFFER[BUFFSIZE];
        recv(cid,( void*)&BUFFER,sizeof(BUFFER),0);

        stringstream ss(BUFFER);
        struct node_data nodedata;

        ss>>nodedata.nodeid>>nodedata.ip>>nodedata.port;

        int hopcount;
        ss>>hopcount;

        struct node_data final_node=routing(nodedata);

        if(final_node.nodeid==node_obj.nodeid)
        {
            cout<<"Self node\n";
            string temp=serialize_tables(node_obj);
            temp=temp+" 0 1 "+to_string(hopcount);
            sendrequest(temp,nodedata.ip,nodedata.port,1);
        }

        else
        {
            cout<<"routing to other part\n";

            string temp=serialize_tables(node_obj);
            temp=temp+" 0 0 "+to_string(hopcount);
            sendrequest(temp,nodedata.ip,nodedata.port,1);

            string data(buffer);
            hopcount++;
            data=data+to_string(hopcount);
            sendrequest(data,final_node.ip,final_node.port,2);

        }

    }

    else if(command==3)
    {
        char BUFFER[BUFFSIZE];
        recv(cid,( void*)&BUFFER,sizeof(BUFFER),0);
        string key,value;
        stringstream ss(BUFFER);
        ss>>key>>value;
        setkey(key,value);

        
    }

    else if(command==4)
    {
        //getKey
        char BUFFER[BUFFSIZE];
        recv(cid,( void*)&BUFFER,sizeof(BUFFER),0);

        stringstream ss(BUFFER);
        struct node_data temp;

        string key;
        ss>>key;
        ss>>temp.ip>>temp.port;

        getkey(key,temp);
    }

    else if(command==5)
    {
        //UPDATE YOUR TABLE 
        cout<<"updating my table\n";
        char BUFFER[BUFFSIZE];
        recv(cid,( void*)&BUFFER,sizeof(BUFFER),0);

        node_structure temp=deserialize_tables(BUFFER);
        update_leaf_set(temp);
        //temp=temp+" 0 1 "+to_string(hopcount);

        copy_to_routing_table(temp);

        printroutable(node_obj);

        ll original,newnode;

        // stringstream ss;
        // ss<<hex<<node_obj.nodeid;
        // ss>>original;
        original=stoll(node_obj.nodeid, 0, 16);

        // stringstream ss1;
        // ss1<<hex<<temp.nodeid;
        // ss1>>newnode;

        newnode=stoll(temp.nodeid, 0, 16);
        string key;
        string val;

       ll tempval;
       cout<<"\nChecking the hash table \n";
       cout<<"printing nodeid "<<temp.nodeid<<" "<<node_obj.nodeid<<endl;
        for(auto x:node_obj.local_hashtable)
        {
            // stringstream ss2;
            // ss2<<hex<<generate_md5(x.first).substr(0,8);
            // ss2>>tempval;
            tempval=stoll(generate_md5(x.first).substr(0,8), 0, 16);
            cout<<tempval<<" "<<newnode<<" "<<original<<"\n";
            if(abs(tempval-newnode)<abs(tempval-original))
            {
                key=x.first;
                val=x.second;

                node_obj.local_hashtable.erase(key);
                setkey(key,val);
            }
        }
    }

    else if(command==6)
    {
        //Displays the value of the key
        char BUFFER[BUFFSIZE];
        recv(cid,( void*)&BUFFER,sizeof(BUFFER),0);

        cout<<BUFFER<<"\n";
    }
    else if(command==7)
    {
        //Received command to exit node
        char BUFFER[BUFFSIZE];
        recv(cid,( void*)&BUFFER,sizeof(BUFFER),0);
        stringstream ss(BUFFER);
        string key,value;
        ss>>key>>value;
        node_obj.local_hashtable[key]=value;
        cout<<"Value setted \n";
    }
    else if(command==8)
    {
        char BUFFER[BUFFSIZE];
        recv(cid,( void*)&BUFFER,sizeof(BUFFER),0);

        string  id(BUFFER);

        for(int i=0;i<c;i++)
        {
            if(node_obj.leafset[i].nodeid.compare(id)==0)
            {
                node_obj.leafset[i]=getdefaul_node();
            }
        }

        for(int i=0;i<r;i++)
        {
            for(int j=0;j<c;j++)
            {
                if(node_obj.routing_table[i][j].nodeid.compare(id)==0)
                {
                    node_obj.routing_table[i][j]=getdefaul_node();
                }
            }
        }
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
/*
struct node_data isleaf(struct node_data D){
    /*
     *  returns the node with the maximum matching prefix leaf node
     *  collision if resolved by picking numerically smaller node_data
     */
    /*struct node_data minD, maxD, closestD;

    minD = node_obj.leafset[0];
    maxD = node_obj.leafset[c/2];

    closestD = node_data(node_obj.nodeid, node_obj.ip, node_obj.port);

    // if(minD.nodeid.compare("-1") != 0 and maxD.nodeid.compare("-1") != 0) {//minD and maxD are non null
    //     //don't update limits
    // }
    // else if(minD.nodeid.compare("-1") == 0){
    //     minD = closestD;
    // }
    // else if(maxD.nodeid.compare("-1") == 0){
    //     maxD = closestD;
    // }
    // else {//both minD and maxD are null
    //     cout<<"\n------isleaf Error: both minD and maxD are null\nReturning default node";
    //     return getdefaul_node();
    // }

    if(minD <= D and D <= maxD){
        long long D_val = stoll(D.nodeid, 0, 16);
        for(auto u : node_obj.leafset){
            if(u.nodeid.compare("-1") == 0) continue;
            long long u_val = stoll(u.nodeid, 0, 16);
            long long closest_val = stoll(closestD.nodeid, 0, 16);

            if(abs(D_val - u_val) < abs(closest_val - D_val)){
                closestD = u;
            }
        }
        return closestD;
    }
    else{
        cout<<"\n------isleaf: Returning default node";
        return getdefaul_node();
    }
}
*/

struct node_data isleaf(struct node_data D)
{
    ll flag=0,mindiff;

    ll nodeval,dataval,tempval;
    struct node_data result;

    dataval = stoll(D.nodeid, 0, 16);

    mindiff=INT_MAX;
    for(auto x:node_obj.leafset)
    {
        if(x.nodeid.compare("-1")!=0)
        {
            flag=1;
            nodeval = stoll(node_obj.nodeid, 0, 16);
            cout<<"itself id is "<<node_obj.nodeid<<" with int val "<<nodeval<<endl;
            cout<<"data id is "<<D.nodeid<<" with int val "<<dataval<<endl;
            mindiff=abs(dataval-nodeval);
            result=node_data(node_obj.nodeid,node_obj.ip,node_obj.port);
            break;
        }
    }
    cout<<"difference with itself "<<mindiff<<endl;
    for(auto x:node_obj.leafset)
    {
        if(x.nodeid.compare("-1")!=0)
        {
            tempval = stoll(x.nodeid, 0, 16);
            cout<<"difference with "<<x.port<<" "<<abs(dataval-tempval)<<endl;
            if(abs(dataval-tempval)<mindiff)
            {
                mindiff=abs(dataval-tempval);
                cout<<"mindiff updated with "<<x.port<<endl<<"with difference "<<mindiff;
                cout<<"min id is "<<x.nodeid<<" with int val "<<tempval<<endl;
                result=x;
            }
        }
    }

    if(flag==0)
        return getdefaul_node();  
    return result; 
}
void populatestate()
{
    int i=0,j;
    
    while(i<r)
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

    printroutable(node_obj);
   
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
    cout<<"requesting node leaf is "<<requesting_node_leaf.nodeid<<"\n";
    if( requesting_node_leaf.nodeid.compare("-1")!=0){
        cout<<"in leaf\n";
        return requesting_node_leaf;
    }
    else{
        cout<<"Not in leaf\n";
        int i=0;
        for( i=0;i<r;i++){
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
            cout<<"Found in routing table\n";

            return node_obj.routing_table[i][hexval] ;
        }
        else{
            cout<<"Rare case\n";
            int flag=0,leafflag=0;
            int difference_min;
            int position;
            int posk;
            if(i>0)
                i--;
            for(int k=i;k<r;k++){
            for(int j=0;j<c;j++) {
                if(node_obj.routing_table[k][j].nodeid.compare("-1")!=0) {
                     stringstream ss1,ss2;
                     int hexval1,hexval2;
        
                     string requesting_node_id=requesting_node.nodeid;
                     string routing_table_id=node_obj.routing_table[k][j].nodeid;
        
                    ss1<<std::hex<<requesting_node_id<<" ";
                    ss1>>hexval1;
                    ss2<<std::hex<<routing_table_id<<" ";
                    ss2>>hexval2;
                    if(flag==0) { 
                        difference_min = abs(hexval1-hexval2);
                        position = j; 
                        posk=k;
                        flag=1;
                    }
                    else{
                        int  new_min = abs(hexval1-hexval2);
                        if(new_min < difference_min) { 
                            difference_min = new_min;
                            position = j; 
                            posk=k;
                        }
                    }
                }
            }
                   
            }
            for(int x=0;x<c;x++)
            {
                if(node_obj.leafset[x].nodeid.compare("-1")!=0)
                {
                    string leafid=node_obj.leafset[x].nodeid;
                    stringstream ss1,ss2;
                     int hexval1,hexval2;
                     ss1<<std::hex<<requesting_node_id<<" ";
                    ss1>>hexval1;
                    ss2<<std::hex<<leafid<<" ";
                    ss2>>hexval2;
                    int leafmin=abs(hexval1-hexval2);
                    if(leafmin<difference_min)
                    {
                        leafflag=1;
                        position=x;
                        difference_min=leafmin;

                    }


                }
            }
            if(leafflag)
                return node_obj.leafset[position];
            return node_obj.routing_table[posk][position] ;
        }
    }
}
void printroutable(struct node_structure node_obj)
{
    cout<<endl<<"my node id is :"<<node_obj.nodeid<<endl;
    cout<<node_obj.ip<<" "<<node_obj.port<<"\n";
    for(int i =0;i<r;i++)
    {
        for(int j=0;j<c;j++)
        {
            cout<<node_obj.routing_table[i][j].ip<<" "<<node_obj.routing_table[i][j].nodeid<<" "<<node_obj.routing_table[i][j].port<<" \t";
        }
        cout<<"\n";
    }
}

void copy_to_routing_table(struct node_structure received_table)
{   
    int i=0;
    for (i=0; i<r; i++){
         if(node_obj.nodeid[i]!=received_table.nodeid[i]){
            break;
         }
    }
    //<<"copying\n";
    for(int j=0; j<c; j++)
    {
        // cout<<"Node id is "<<node_obj.nodeid<<"\n";
        // cout<<"recived table id "<<received_table.nodeid<<"\n";

        if((node_obj.routing_table[i][j].nodeid.compare("-1")==0 || node_obj.routing_table[i][j].nodeid.compare(node_obj.nodeid)==0))
        {
            if(received_table.routing_table[i][j].nodeid.compare("-1")!=0 && received_table.routing_table[i][j].nodeid.compare(node_obj.routing_table[i][j].nodeid)!=0)
            {
               node_obj.routing_table[i][j] = received_table.routing_table[i][j];
            }
        }
    }
    cout<<"Copy complete\n";
}

void sharetables()
{
    cout<<"Inside share\n";
    set<struct node_data,less_than_cmp> data;

    for(auto x:node_obj.leafset)
    {
        if(x.nodeid.compare("-1")!=0)
        {
            data.insert(x);
        }
    }

    for(auto x:node_obj.neighbourhoodset)
    {
        if(x.nodeid.compare("-1")!=0)
        {
            data.insert(x);
        }
    }

    for(auto x:node_obj.routing_table)
    {
        for(auto y:x)
        {
            if(y.nodeid.compare("-1")!=0&&y.nodeid.compare(node_obj.nodeid)!=0)
            {
                data.insert(y);
            }
        }
    }

    for(auto x:data)
    {
        sendrequest(serialize_tables(node_obj),x.ip,x.port,5);
    }
}


void getkey(string key, struct node_data temp_node)
{
    struct node_data temp_data;
    temp_data.nodeid=key;
    string temp;

    // if(node_obj.local_hashtable.find(key)!=node_obj.local_hashtable.end())
    // {
    //     temp=node_obj.local_hashtable[key];
    //     sendrequest(temp,temp_node.ip,temp_node.port,6);
    // }

    for(auto x:node_obj.local_hashtable)
    {
        if(generate_md5(x.first).substr(0,8).compare(key)==0)
        {
            temp=node_obj.local_hashtable[x.first];
            sendrequest(temp,temp_node.ip,temp_node.port,6);
            return;
        }
    }

    struct node_data final_node=routing(temp_data);
    //cout<<"routing ne diya"<<final_node.nodeid<<"\n";
    
    if(final_node.nodeid.compare(node_obj.nodeid)==0)
    {
        // if(node_obj.local_hashtable.find(key)!=node_obj.local_hashtable.end())
        // {
        //     temp=node_obj.local_hashtable[key];
        // }
        // if()
        // {
            for(auto x:node_obj.local_hashtable)
            {
                if(generate_md5(x.first).substr(0,8).compare(key)==0)
                {
                    temp=node_obj.local_hashtable[x.first];
                    sendrequest(temp,temp_node.ip,temp_node.port,6);
                    return;
                }
            }
        // }
            temp="Not found";
            sendrequest(temp,temp_node.ip,temp_node.port,6);
                    return;
    }
    else
    {
        sendrequest(key+" "+temp_node.ip+" "+temp_node.port,final_node.ip,final_node.port,4);
    }
    
}


void setkey(string key,string value)
{
    string hashkey = generate_md5(key);
    hashkey = hashkey.substr(0,8);
    cout<<"hashvalue of key is "<<hashkey<<endl;
    struct node_data key_obj;
    key_obj.nodeid=hashkey;
    struct node_data final_key_des=routing(key_obj);

    if(final_key_des.nodeid==node_obj.nodeid)
    {
        node_obj.local_hashtable[key]=value;
        cout<<"key set done"<<endl;
    }
    else{
        string msg=key+" "+value;
        sendrequest(msg,final_key_des.ip,final_key_des.port,3);
    }

}
void printhash()
{
    for(auto x:node_obj.local_hashtable)
    {
        cout<<x.first<<" "<<x.second<<endl;
    }
}

void removeme()
{
    set<struct node_data,less_than_cmp> data;

    for(auto x:node_obj.leafset)
    {
        if(x.nodeid.compare("-1")!=0)
        {
            data.insert(x);
        }
    }

    for(auto x:node_obj.neighbourhoodset)
    {
        if(x.nodeid.compare("-1")!=0)
        {
            data.insert(x);
        }
    }

    for(auto x:node_obj.routing_table)
    {
        for(auto y:x)
        {
            if(y.nodeid.compare("-1")!=0&&y.nodeid.compare(node_obj.nodeid)!=0)
            {
                data.insert(y);
            }
        }
    }

    for(auto x:data)
    {
        sendrequest(node_obj.nodeid,x.ip,x.port,8);
    }
    cout<<"Bye...\n";
    exit(1);
}

void shutdown()
{
    ll dataval,nodeval,mindiff;
    mindiff=INT_MAX;
    int flag=0;
    struct node_data result;
    cout<<"Shutting down node\n";
    for(auto y:node_obj.local_hashtable)
    {
        cout<<y.first<<" "<<y.second<<"\n";
        dataval = stoll(generate_md5(y.first).substr(0,8), 0, 16);
        cout<<"dataval is "<<dataval<<endl;
        flag=0;
        for(auto x:node_obj.leafset)
        {
            cout<<"leaf node is "<<x.port<<endl;
            if(x.nodeid.compare("-1")!=0)
            {
                if(flag==0)
                {
                    nodeval = stoll(x.nodeid, 0, 16);
                    cout<<"first leafnode id is "<<x.nodeid<<" with int val "<<nodeval<<endl;
                    // cout<<"data id is "<<D.nodeid<<" with int val "<<dataval<<endl;
                    mindiff=abs(dataval-nodeval);
                    cout<<"mindiff is "<<mindiff<<endl;
                    result=node_data(x.nodeid,x.ip,x.port);
                    flag=1;
                }
                if(flag==1)
                {
                    nodeval = stoll(x.nodeid, 0, 16);   
                    cout<<"leafnode id is "<<x.nodeid<<" with int val "<<nodeval<<endl;
                    if(mindiff>abs(dataval-nodeval))
                    {
                        cout<<"mindiff is "<<mindiff<<endl;
                        mindiff=abs(dataval-nodeval);
                        result=node_data(x.nodeid,x.ip,x.port);
                    }
                }
            }
            
        } 
        cout<<"key is going to"<<result.ip<<" "<<result.port<<endl;
        sendrequest(y.first+" "+y.second,result.ip,result.port,7);
        node_obj.local_hashtable.erase(y.first);
           
    }
    removeme();
}

int main(int argc,char **argv)
{
    
    pthread_t id;
    int data;
    string choice;
    string key,value;
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
            printroutable(node_obj);
        }
        if(choice.compare("printleaf")==0)
        {
            printleaf();
        }

        if(choice.compare("getkey")==0)
        {
            cin>>key;
            key=generate_md5(key).substr(0,8);
            struct node_data temp;
            temp.ip=node_obj.ip;
            temp.port=node_obj.port;
            getkey(key,temp);
        }
        if(choice.compare("set")==0)
        {
            string key,value;
            cin>>key>>value;
            setkey(key,value);

        }
        if(choice.compare("printhash")==0)
        {
            printhash();
        }
        if(choice.compare("shutdown")==0)
        {
            cout<<"Shut down calling\n";
            shutdown();
        }
    }
}
