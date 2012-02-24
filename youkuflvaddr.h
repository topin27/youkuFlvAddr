#ifndef YOUKUFLVADDR_H
#define YOUKUFLVADDR_H

#include <string>

void gen_sid(std::string &sid_); 
std::string &gen_fileid(int seed_, std::string &fileid_); 
std::string &gen_key(const std::string &k_key1_, std::string &key2_);

#endif
