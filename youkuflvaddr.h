#ifndef YOUKUFLVADDR_H
#define YOUKUFLVADDR_H

#include <string>

//用于保存json字符串中的信息
struct JsonInfo {
	int seed;
	std::string fileid;
	std::string key1;
	std::string key2;
};

std::string get_flv_addr(const JsonInfo &json_, const std::string &segNumStr_);

//获取json中的seed等信息，没有错误的话返回视频的段数，有错返回负数
int get_json_info(const int clientSockId_, const std::string &k_flvid_, JsonInfo &jsonInfo_);

int open_socket(const std::string &k_url_, int port_);

void close_socket(int socketId_);

//获取优酷视频地址的id，就是地址中id_后的那段
std::string get_flvid(const std::string &k_youkuAddr_); 

#endif
