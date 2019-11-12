//
// Created by upinder on 12/11/19.
//

#include "Node.h"
#include <string>
#include <iostream>
#include "md5.h"
const int b = 4;

Node::Node(std::string ip, std::string port){
    this->ip = ip;
    this->port = port;
    this->node_id = generate_md5(ip + port);//;.substr(0,b);
}
void Node::print_status(){
    std::cout<<this->ip<<"\n";
    std::cout<<this->port<<"\n";
    std::cout<<this->node_id<<"\n";
}
