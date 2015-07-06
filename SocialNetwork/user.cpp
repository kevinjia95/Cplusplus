#include <vector>
#include <string>
#include "user.h"
#include <iostream>
using namespace std;

User::User(int number, string username, int year, int code, vector<int> vec) //Constructor
{
	id = number; //pass the inputs directly to the private members
	name = username;
	birth = year;
	zip = code;
	friends = vec;
}

void User::add_friend(int number)
{
	friends.push_back((int) number); //add ID to friends
}

void User::delete_friend(int number)
{
	for (unsigned int i = 0; i < friends.size(); i++) //search among friends for ID
	{
		if (friends[i] == number)
		{
			for (unsigned int j = i; j < (friends.size() - 1); j++)
			{
				friends[j] = friends[j + 1]; //shift friend vector
			}
			friends.pop_back(); //reduce size of friend vector by 1;
		}
	}
}

int User::return_id()
{
	return id;
}

string User::return_name()
{
	return name;
}

int User::return_birth()
{
	return birth;
}

int User::return_zip()
{
	return zip;
}

vector<int>& User::return_friends()
{
	vector<int> &a = friends;
	return a;
}
