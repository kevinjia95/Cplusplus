#include <vector>
#include <string>
#include "network.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
using namespace std;

Network::Network() //Constructor
{
	vector<User> userlist;
}

void Network::add_user(int id, string name, int year, int zip, vector<int> vec) //Take in 5 inputs
{
	User newuser(id, name, year, zip, vec); //Pass those five inputs to the User constructor
	userlist.push_back(newuser); //Add the user to the network's vector of users
}

int Network::add_connection(int id1, int id2)
{
	if ((unsigned int) id1 >= userlist.size()) //Check if id1 exists
	{
		cerr << "ERROR: Specified ID1 does not exist in the network" << endl;
		return -1;
	}
	else if ((unsigned int) id2 >= userlist.size()) //Check if id2 exists
	{
		cerr << "ERROR: Specified ID2 does not exist in the network" << endl;
		return -1;
	}
	userlist[id1].add_friend(id2); //add each other as friends
	userlist[id2].add_friend(id1);
	return 0;
}

int Network::remove_connection(int id1, int id2)
{
	if ((unsigned int) id1 >= userlist.size()) //Check if id1 exists
	{
		cerr << "ERROR: Specified ID1 does not exist in the network" << endl;
		return -1;
	}
	else if ((unsigned int) id2 >= userlist.size()) //Check if id2 exists
	{
		cerr << "ERROR: Specified ID2 does not exist in the network" << endl;
		return -1;
	}
	userlist[id1].delete_friend(id2); //Delete each other as friends
	userlist[id2].delete_friend(id1);
	return 0;
}

int Network::get_id(string name)
{
	for (unsigned int i = 0; i < userlist.size(); i++) //Loop through the network's list of users
	{
		string user = userlist[i].return_name(); //For each user, get the username as a string
		if (user == name) //if the name we want is the user we are looking at
		{
			return i;
		}
	}
	return -1; //if we looped through the entire list of users and didn't find the user
}

int Network::read_friends(char* fname)
{
	ifstream ifile(fname);
	if (ifile.fail() == true) //check to see if the file open failed
	{
		cerr << "ERROR: File " << fname << " could not be opened." << endl;
		return -1;
	}
	string storage;
	getline(ifile, storage); //first line of the file
	istringstream iss(storage); 
	int network_size; 
	iss >> network_size; //should be number of users in the network
	int count = 0;
	while (count < network_size)
	{
		//ID
		int id;
		string storage2;
		getline(ifile, storage2); //Line should have ID
		istringstream iss_id(storage2);
		iss_id >> id;
		//USERNAME
		string first_name, last_name, username;
		string storage3;
		getline(ifile, storage3);
		istringstream iss_name(storage3); //Line should have username
		iss_name >> first_name >> last_name;
		username = first_name + " " + last_name;
		//BIRTHYEAR
		int birth_year;
		string storage4;
		getline(ifile, storage4);
		istringstream iss_birth(storage4); //Line should have birthyear
		iss_birth >> birth_year;
		//ZIPCODE
		int zip;
		string storage5;
		getline(ifile, storage5);
		istringstream iss_zip(storage5); //Line should have zipcode
		iss_zip >> zip;
		//USER'S FRIENDS
		vector<int> friends;
		int onefriend;
		string storage6;
		getline(ifile, storage6);
		istringstream iss_friend(storage6); //Line should have friends
		while (iss_friend >> onefriend) //while we still get inputs of integers
		{
			friends.push_back(onefriend);
		}
		//ABOVE INFO CREATES A USER IN NETWORK
		add_user(id, username, birth_year, zip, friends);
		count++;
	}
	
	ifile.close();
	return 0;
}

int Network::write_friends(char* fname)
{
	ofstream ofile(fname);
	if (ofile.fail())
	{
		cerr << "ERROR: Could not get file" << endl;
		return -1;
	}
	ostringstream storage;
	vector<User> network_users = return_userlist();
	storage << network_users.size() << endl; //first line is size of network
	
	for (unsigned int i = 0; i < network_users.size(); i++)
	{
		storage << network_users[i].return_id() << endl; //second line is ID
		storage << '\t' << network_users[i].return_name() << endl; //third line is username
		storage << '\t' << network_users[i].return_birth() << endl; //fourth line is birthyear
		storage << '\t' << network_users[i].return_zip() << endl; //fifth line is zipcode
		storage << '\t';
		vector<int> friend_network = network_users[i].return_friends();
		for (unsigned int j = 0; j < friend_network.size(); j++)
		{
			storage << friend_network[j] << " "; //sixth line is user's friends
		}
		storage << endl;
	}
	
	ofile << storage.str();
	ofile.close();
	return 0;
}

vector<User>& Network::return_userlist()
{
	vector<User> &a = userlist; //return a reference variable that refers to the vector of users
	return a;
}

vector<int> Network::shortest_path(int from, int to)
{
	int head = 0;
	int tail = 0;
	int target;
	vector<int> bfsq, pred, result, final;
	bfsq.push_back(from);
	pred.push_back(-1);
	tail++;
	vector<bool> added; //keep track of which users have been added
	vector<User> list_of_users = return_userlist();
	for (unsigned int i = 0; i < list_of_users.size(); i++) //fill "added"
	{
		added.push_back(false);
	}
	added[from] = true; //indicate that "from" id has been added
	while (added[to] == false && head != tail)
	//while we haven't added "to" id and we haven't searched our entire queue
	{
		vector<int> friends = list_of_users[bfsq[head]].return_friends();
		//pull up friends of bfsq entries
		for (unsigned int i = 0; i < friends.size(); i++)
		//loop through friends of the bfsq entry
		{
			if (added[friends[i]] == false) //if we haven't added the id
			{
				bfsq.push_back(friends[i]); //add it to bfsq
				pred.push_back(head); //set pred to be current bfsq entry
				added[friends[i]] = true; //indicate id has been added
				if (friends[i] == to)
				{
					target = tail; //keep track of which bfsq entry is "to" id
				}
				tail++;
			}
		}
		head++;	
	}
	
	if (added[to] == true) //if we "found" the "to" id
	{
		while (target != -1)
		{
			result.push_back(bfsq[target]);//backtrace and add id's to result
			target = pred[target];
		}
	}
	else //if we didn't "find" the "to" id
	{
		return final;
	}
	for (int i = (result.size() - 1); i >= 0; i--)
	{
		int entry = result.at(i);
		final.push_back(entry);
	}
	return final;
}

vector<vector<int> > Network::groups()
{
	int pointer = 0;
	vector<vector<int> > sets;
	vector<User> list_of_users = return_userlist();
	vector<bool> added; //indicate which id's have been added
	//note that "added" vector is NOT reset after each bfsq
	//we need to keep track of which id's have been added
	//across all the different breadth first searches
	for (unsigned int i = 0; i < list_of_users.size(); i++)
	{
		added.push_back(false);
	}
	while (pointer != -1)
	{
		vector<int> bfsq, pred;
		bfsq.push_back(pointer);
		added[pointer] = true;
		int head = 0;
		int tail = 1;
		while (head != tail) //while we haven't completed a bfs
		{
			vector<int> friends = list_of_users[bfsq[head]].return_friends();
			//get friends of each bfsq entry
			for (unsigned int i = 0; i < friends.size(); i++)
			{
				if (added[friends[i]] == false)
				{
					bfsq.push_back(friends[i]);
					pred.push_back(head);
					added[friends[i]] = true;
					tail++;
				}
			}
			head++;	
		}
		//once we've completed a breadth first search
		int oldpointer = pointer;
		unsigned int i = 0;
		//we search for a new starting point for a new bfs
		while (pointer == oldpointer && i < list_of_users.size())
		{
			if (added[i] == false)
			//if we haven't encountered an id
			{
				pointer = i; //we set the new starting point as the id
			}
			i++;
			if (i == list_of_users.size() && pointer == oldpointer)
			//if we've gone through all id's
			{
				pointer = -1;
			}
		}
		sets.push_back(bfsq);
		//before we begin the next bfsq, we add the id's of this bfsq
		//as a set in itself
	}
	return sets;
}

vector<int> Network::suggest_friends(int who, int& score)
{
	vector<User> list_of_users = return_userlist();
	vector<int> friends = list_of_users[who].return_friends();
	//get friends of "who" id
	vector<int> count, result;
	for (unsigned int i = 0; i < list_of_users.size(); i++)
	//"i" loosely represents the id of each user in the network
	{
		count.push_back(0);
		//initialize count vector by putting 0 for each id
	}
	for (unsigned int i = 0; i < friends.size(); i++)
	{
		vector<int> friendfriends = list_of_users[friends[i]].return_friends();
		//get friends of each friend of "who" id;
		for (unsigned int j = 0; j < friendfriends.size(); j++)
		{
			if (friendfriends[j] != who)
			{
				count[friendfriends[j]]++;
				//tally how many times an id shows up within the
				//friends of each friend of "who" id
			}
		}
	}
	int maxscore = 0;
	//we need to make sure that we don't suggest people who are already
	//friends with the "who" id however
	for (unsigned int i = 0; i < list_of_users.size(); i++)
	//go through every user id in the network
	{
		for (unsigned int j = 0; j < friends.size(); j++)
		//go through every one of "who" id's friends
		{
			if (i == (unsigned int) friends[j])
			//if the id is already a friend of "who"
			{
				count[i] = 0;
				//reset the id's tally to 0
			}
		} 
	}
	//find out the id(s) with the max tallies
	for (unsigned int i = 0; i < list_of_users.size(); i++)
	{
		maxscore = max(maxscore, count[i]);
	}
	score = maxscore;
	//put all the id(s) with the max score into a result vector
	for (unsigned int i = 0; i < list_of_users.size(); i++)
	{
		if (count[i] == maxscore && count[i] != 0)
		{
			result.push_back(i);
		}
	}
	return result;
}
