//
// Created by upinder on 12/11/19.
//

#ifndef PASTRY_NODE_H
#define PASTRY_NODE_H
#include <string>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

class Node {
private:
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
public:
    Node():ip("-1"),port("-2"),node_id("-3"){}
    Node(std::string, std::string);
    void print_status();
};


#endif //PASTRY_NODE_H
