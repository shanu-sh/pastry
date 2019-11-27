//
// Created by upinder on 28/11/19.
//

#include "PastryNode.h"
#include <iostream>
void PastryNode::print_state(){
    std::cout<<"PastryNode:\n";
    std::cout<<"IP: "<<node.ip<<"\n";
    std::cout<<"Port: "<<node.port<<"\n";
    std::cout<<"Hash: "<<node.node_id<<"\n";
    std::cout<<"Routing table:\n";
    for(auto u : routing_table){
        for(auto v : u){
            std::cout<<v.node_id;
        }
        std::cout<<"\n";
    }
    std::cout<<"\n";
    std::cout<<"Leaf set:\n";
    for(auto u : leaf_set){
        std::cout<<u.node_id<<" ";
    }
    std::cout<<"\n";
    std::cout<<"Neighbourhood set:\n";
    for(auto u : neighbourhood_set){
        std::cout<<u.node_id<<" ";
    }
    std::cout<<"\n";
}