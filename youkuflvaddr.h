#ifndef YOUKUFLVADDR_H
#define YOUKUFLVADDR_H

#include <string>

void gen_sid(std::string &sid_); 
std::string gen_fileid(int seed_, const std::string &k_fileid_);
std::string gen_key(const std::string &k_key1_, const std::string &k_key2_);

#endif
