#include <vector>
#include <string>
#include <iostream>
#ifndef USER_H
#define USER_H
using namespace std;

class User {
 public:
	User(int number, string username, int year, int code, vector<int> vec);
	void add_friend(int number);
	void delete_friend(int number);
	int return_id();
	string return_name();
	int return_birth();
	int return_zip();
	vector<int>& return_friends();
	int depth;
	int predecessor;
	int set_member;
	
 private:
	int id;
	string name;
	int birth;
	int zip;
	vector<int> friends;
	
};


#endif
