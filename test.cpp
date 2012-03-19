#include <iostream>
#include <string>
#include "youkuflvaddr.h"
using namespace std;

int main()
{
	int sockId = open_socket("v.youku.com", 80);
	string addr = "http://v.youku.com/v_show/id_XMjUyODAzNDg0.html";
	JsonInfo ji;
	if (get_json_info(sockId, get_flvid(addr), ji) < 0)
		return -1;
	cout << get_flv_addr(ji, "00") << endl;
	close_socket(sockId);
}
