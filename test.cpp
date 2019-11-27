//
// Created by upinder on 12/11/19.
//
#include <iostream>
#include <string>
#include "utils.h"
#include "PastryNode.h"
#include <fstream>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/set.hpp>
#include <boost/serialization/vector.hpp>

int main(int argc, char* argv[]){
    if(argc != 2){
        std::cerr<<"Usage: ./test.o <port_no>\n";
        return 0;
    }
    std::string ip;
    std::string port;
    ip = utils::extractPublicIP();
    port = std::string(argv[1]);
    PastryNode n(ip, port);
    for(int i=0; i<4; i++){
        n.routing_table.push_back(std::vector<Node>(4, Node()));
    }
    n.print_state();

    std::ofstream ofs("filename");
    {
        boost::archive::text_oarchive oa(ofs);
        oa << n;
    }

    PastryNode n2;
    {
        // create and open an archive for input
        std::ifstream ifs("filename");
        boost::archive::text_iarchive ia(ifs);
        // read class state from archive
        ia >> n2;
        // archive and stream closed when destructors are called
    }
    n2.print_state();
    return 0;
}
