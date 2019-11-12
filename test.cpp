//
// Created by upinder on 12/11/19.
//
#include <iostream>
#include <string>
#include "Node.h"
#include <fstream>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

int main(){
    std::string ip;
    std::string port;
    std::cin>>ip;
    std::cin>>port;
    Node n(ip, port);
    n.print_status();

    std::ofstream ofs("filename");
    {
        boost::archive::text_oarchive oa(ofs);
        oa << n;
    }

    Node n2;
    {
        // create and open an archive for input
        std::ifstream ifs("filename");
        boost::archive::text_iarchive ia(ifs);
        // read class state from archive
        ia >> n2;
        // archive and stream closed when destructors are called
    }
    n2.print_status();
    return 0;
}
