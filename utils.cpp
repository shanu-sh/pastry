//
// Created by upinder on 13/11/19.
//

#include "utils.h"
#include <iostream>
#include <string.h>
#include <netdb.h>
#include <ifaddrs.h>
#define INET_ADDRSTRLEN 16

std::string utils:: extractPublicIP(){
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
        }
        ifa = ifa->ifa_next;
    }
    return std::string(host);
}
