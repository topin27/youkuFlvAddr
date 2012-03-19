#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <cstring>
#include <cstdio>
#include <ctime>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include "youkuflvaddr.h"
using std::string;
using std::cout;
using std::endl;

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

static string gen_sid() {
	int i1 = 1000 + rand() % 999; 
	int i2 = 1000 + rand() % 9000;
	srand((unsigned)time(NULL));
	std::stringstream strStrm;
	strStrm << time(NULL) << rand() % 1000 << i1 << i2;
	return strStrm.str();
}

static string gen_fileid(int seed_, const string &k_fileid_) {
	string mixed;
	string source = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ/\\:._-1234567890";
	int index, len = (int)source.size();

	for (int i = 0; i < len; ++i) {
		seed_ = (seed_ * 211 + 30031) % 65536; 
		index = (int)(((double)seed_) / 65536 * source.size());
		mixed.push_back(source[index]);
		source.erase(index, 1);
	}
	
	string::const_iterator it1 = k_fileid_.begin(), it2;
	it2 = it1;
	std::vector<int> ids;
	while (it1 != k_fileid_.end()) {
		if (*it1 == '*') {
			ids.push_back(atoi((k_fileid_.substr(it2 - k_fileid_.begin(), it1 - it2)).c_str()));
			++it1;
			it2 = it1;
		}
		else
			++it1;
	}

	string fileid = "";
	for (size_t i = 0; i < ids.size(); ++i)
		fileid.push_back(mixed[ids[i]]);
	return fileid;
}

static string gen_key(const string &k_key1_, const string &k_key2_) {
	unsigned int keyNum = (unsigned int)strtoul(k_key1_.c_str(), NULL, 16);
	keyNum ^= 0xA55AA5A5;
	string key = itoa(keyNum, 16); 
	key = k_key2_ + key;
	return key;
}

int open_socket(const string &k_url_, int port_) {
	sockaddr_in serverAddr;
	memset((char *)&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons((u_short)port_);
	hostent *p_ipInfo = gethostbyname(k_url_.c_str());
	if (p_ipInfo == NULL)
		return -3;
	memcpy(&serverAddr.sin_addr, p_ipInfo->h_addr, p_ipInfo->h_length);
	//cout << k_url_ << "的ip是：" << inet_ntoa(serverAddr.sin_addr) << endl;
	int clientSocketId = 0;
	if (0 > (clientSocketId = socket(AF_INET, SOCK_STREAM, 0)))
		return -2;
	//cout << "socket id is: " << clientSocketId << endl;
	if (connect(clientSocketId, (sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
		return -1;
	return clientSocketId;
}

void close_socket(int socketId_) {
	close(socketId_);
}

string get_flvid(const string &k_youkuAddr_) {
	size_t start = 0;
	size_t end = 0;
	start = k_youkuAddr_.find("v.youku.com/v_show/id_");
	if (start == string::npos) {
		return "";
	}
	start += 22;
	end = k_youkuAddr_.find(".html");
	return k_youkuAddr_.substr(start, end - start);
}

//获取http头中的content-length字段的值
static int get_content_length(const string &responseHeader_) {
	size_t pos = responseHeader_.find("Content-Length");
	if (string::npos == pos)
		return -1;
	pos += 15;
	return atoi(responseHeader_.substr(pos, (responseHeader_.find("\r\n", pos) - pos)).c_str());
}

int get_json_info(const int clientSockId_, const string &k_flvid_, JsonInfo &jsonInfo_) {
	if (clientSockId_ < 0)
		return -2;
	//获取网址http://v.youku.com/player/getPlayList/VideoIDS/的http信息
	string temp = "http://v.youku.com/player/getPlayList/VideoIDS/";
	temp += k_flvid_;
	string httpHeader = "GET ";
	httpHeader += temp.substr(18);
	httpHeader += " HTTP/1.1\r\nHOST:";
	httpHeader += "v.youku.com\r\n\r\n";
	send(clientSockId_, httpHeader.c_str(), (int)httpHeader.size(), 0);
	char buf[2048]; //这里的buf要设置的足够大保证第一次就能接收到所有的http头
	memset(buf, '\0', sizeof(buf));
	temp = "";
	int recvLen = recv(clientSockId_, buf, sizeof(buf), 0);
	temp += buf;
	int contentLen = get_content_length(temp);
	contentLen -= (int)(recvLen - temp.find("\r\n\r\n") - 4);
	while (contentLen > 0) {
		memset(buf, '\0',sizeof(buf));
		recvLen = recv(clientSockId_, buf, sizeof(buf), 0);
		contentLen -= recvLen;
		temp += buf;
	}

	//填入json信息===============
	size_t pos;
	if ((pos = temp.find("\"seed\":")) == string::npos) {
		jsonInfo_.seed = 0;
		return -1;
	}
	jsonInfo_.seed = atoi(temp.substr(pos + 7, temp.find_first_of(',', pos + 7) - pos - 7).c_str());
	if ((pos = temp.find("\"streamfileids\":")) == string::npos) {
		jsonInfo_.fileid = "";
		return -1;
	}
	if ((pos = temp.find("\"flv\":\"", pos)) == string::npos) {
		jsonInfo_.fileid = "";
		return -1;
	}
	jsonInfo_.fileid = temp.substr(pos + 7, temp.find_first_of('"', pos + 7) - pos - 7);
	if ((pos = temp.find("\"key1\":\"")) == string::npos) {
		jsonInfo_.key1 = "";
		return -1;
	}
	jsonInfo_.key1 = temp.substr(pos + 8, temp.find_first_of('"', pos + 8) - pos - 8);
	if ((pos = temp.find("\"key2\":\"", pos)) == string::npos) {
		jsonInfo_.key2 = "";
		return -1;
	}
	jsonInfo_.key2 = temp.substr(pos + 8, temp.find_first_of('"', pos + 8) - pos -8);

	//获取视频的段数============
	pos = temp.find("\"segs\":");
	pos = temp.find("\"flv\":", pos);
	if (pos == string::npos)
		return -3;
	pos += 6;
	int segNum = 0;
	while (temp[pos] != ']') {
		if (temp[pos] == '{')
			++segNum;
		++pos;
	}

	//cout << "json info: ";
	//cout << jsonInfo_.seed << "\t" << jsonInfo_.fileid << "\t" \
	//	<< jsonInfo_.key1 << "\t" << jsonInfo_.key2 <<endl;
	return segNum;
}

string get_flv_addr(const JsonInfo &json_, const string &segNumStr_) {
	string flvAddr = "f.youku.com/player/getFlvPath/sid/";
	flvAddr += gen_sid();
	flvAddr += "_";
	flvAddr += segNumStr_;
	flvAddr += "/st/flv/fileid/";
	if (json_.seed == 0 || json_.fileid == "")
		return "";
	flvAddr += gen_fileid(json_.seed, json_.fileid);
	flvAddr += "?K=";
	if (json_.key1 == "" || json_.key2 == "")
		return "";
	flvAddr += gen_key(json_.key1, json_.key2);
	return flvAddr;
}
