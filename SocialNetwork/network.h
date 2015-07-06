#include <vector>
#include <string>
#include "user.h"
#ifndef NETWORK_H
#define NETWORK_H
using namespace std;

class Network {
 public:
	Network();
	void add_user(int id, string name, int year, int zip, vector<int> vec);
	int add_connection(int id1, int id2);
	int remove_connection(int id1, int id2);
	int get_id(string name);
	int read_friends(char* fname);
	int write_friends(char* fname);
	vector<User>& return_userlist();
	vector<int> shortest_path(int from, int to);
	vector<vector<int> > groups();
	vector<int> suggest_friends(int who, int& score);
 private:
	vector<User> userlist;
};


#endif
