//
// Created by upinder on 28/11/19.
//

#ifndef PASTRY_PASTRYNODE_H
#define PASTRY_PASTRYNODE_H
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include "Node.h"
#include <boost/serialization/set.hpp>
#include <boost/serialization/vector.hpp>

class less_than_cmp{
public:
    bool operator()(const Node& lhs, const Node& rhs){
        return stoll(lhs.node_id, 0 , 16) < stoll(rhs.node_id, 0, 16);
    }
};
class greater_than_cmp{
public:
    bool operator()(const Node& lhs, const Node& rhs){
        return stoll(lhs.node_id, 0 , 16) > stoll(rhs.node_id, 0, 16);
    }
};

class PastryNode {
public:
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version){
        ar & node;
        ar & routing_table;
        ar & leaf_set;
        ar & neighbourhood_set;
    }
    Node node;
    std::vector<std::vector<Node>> routing_table;
    std::set<Node, less_than_cmp> leaf_set;
    std::set<Node, less_than_cmp> neighbourhood_set;
    PastryNode(){}
    PastryNode(std::string ip, std::string port){
        node = Node(ip, port);
    }
    void print_state();
};


#endif //PASTRY_PASTRYNODE_H
