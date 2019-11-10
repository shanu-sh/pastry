#include <stdio.h>
#include <string.h>
#include <openssl/md5.h>
#include<iostream>
#include<string.h>
using namespace std;

string generate_md5(string s){

	unsigned char digest[16];  
    char string[s.length() + 1]; 
    strcpy(string, s.c_str());

    //openssl md5 fn
	MD5((unsigned char*)&string, strlen(string), (unsigned char*)&digest);    

    char md5hash[33];

    for(int i = 0; i < 16; i++)
         sprintf(&md5hash[i*2], "%02x", (unsigned int)digest[i]);
    return md5hash;
    
}

