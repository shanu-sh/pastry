//
// Created by upinder on 12/11/19.
//

#ifndef PASTRY_NODE_H
#define PASTRY_NODE_H
#include <string>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
const std::string INV_IP = "-1";
const std::string INV_PORT = "-2";
const std::string INV_ID = "-3";

class Node {
public:
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version){
        ar & ip;
        ar & port;
        ar & node_id;
    }
    std::string ip;
    std::string port;
    std::string node_id;
    Node():ip(INV_IP),port(INV_PORT),node_id(INV_ID){}
    Node(std::string, std::string);
    bool operator < (const Node& rhs){return stoll(this->node_id, 0, 16) < stoll(rhs.node_id,0,16);}
    bool operator > (const Node& rhs){return stoll(this->node_id, 0, 16) > stoll(rhs.node_id, 0, 16);}
    bool operator == (const Node& rhs){return stoll(this->node_id, 0, 16) == stoll(rhs.node_id, 0, 16);}
};

#endif //PASTRY_NODE_H
