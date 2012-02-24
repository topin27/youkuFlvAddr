#include <iostream>
#include <sstream>
#include <string>
#include <cstdio>
#include <ctime>
#include <vector>
#include <algorithm>
#include "youkuflvaddr.h"
using std::string;

void gen_sid(string &sid_) {
	int i1 = 1000 + rand() % 999; //1000?
	int i2 = 1000 + rand() % 9000;
	srand((unsigned)time(NULL));
	std::stringstream strStrm;
	strStrm << time(NULL) << rand() % 1000 << i1 << i2;
	sid_ = strStrm.str();
}

string &gen_fileid(int seed_, string &fileid_) {
	string mixed;
	string source = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ/\\:._-1234567890";
	int index, len = (int)source.size();

	for (int i = 0; i < len; ++i) {
		seed_ = (seed_ * 211 + 30031) % 65536; //TODO
		index = (int)(((double)seed_) / 65536 * source.size()); //TODO
		mixed.push_back(source[index]);
		source.erase(index, 1);
	}
	
	string::iterator it1 = fileid_.begin(), it2;
	it2 = it1;
	std::vector<int> ids;
	while (it1 != fileid_.end()) {
		if (*it1 == '*') {
			ids.push_back(atoi((fileid_.substr(it2 - fileid_.begin(), it1 - it2)).c_str()));
			++it1;
			it2 = it1;
		}
		else
			++it1;
	}

	fileid_ = "";
	for (size_t i = 0; i < ids.size(); ++i)
		fileid_.push_back(mixed[ids[i]]);
	return fileid_;
}

//linux下C++版的itoa函数，原文地址：http://www.jb.man.ac.uk/~slowe/cpp/itoa.html
static string itoa(int value_, int base_) {
	string buf;
	if(base_ < 2 || base_ > 16)
		return buf;
	enum {maxDigits = 35};
	buf.reserve(maxDigits);
	int quotient = value_;
	do {
		buf += "0123456789abcdef"[std::abs(quotient % base_)];
		quotient /= base_;
	} while (quotient);
	if (value_ < 0)
		buf += '-';
	std::reverse(buf.begin(), buf.end());
	return buf;
}

string &gen_key(const string &k_key1_, string &key2_) {
	unsigned int key = (unsigned int)strtoul(k_key1_.c_str(), NULL, 16);
	key ^= 0xA55AA5A5;
	key2_ += itoa(key, 16);
	return key2_;
}
