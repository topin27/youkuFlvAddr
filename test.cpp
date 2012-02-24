#include <iostream>
#include <string>
#include "youkuflvaddr.h"
using namespace std;

int main()
{
	int seed = 6302;
	string key1 = "bd7c3d19";
	string key2 = "de1515a31372faac";
	string fileid = "13*18*13*13*13*11*13*42*13*13*39*44*41*41*47*41*18*29*13*60*44*42*13*39*17*33*39*56*47*56*56*39*17*42*33*44*44*17*54*33*17*39*11*54*41*44*17*39*54*18*55*55*44*54*38*13*57*38*55*56*47*39*55*55*33*42*";
	string sid = "";
	gen_sid(sid);
	cout << "sid = " << sid << endl;
	cout << "===================\n";
	cout << "fileid = " << gen_fileid(seed, fileid) << endl;
	cout << "===================\n";
	cout << "key = " << gen_key(key1, key2) << endl;
}
