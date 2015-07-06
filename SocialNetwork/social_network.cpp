#include "network.h"
#include "user.h"
#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>

using namespace std;

int main(int argc, char *argv[])
{
	if (argc != 2) //check to see if usage is correct
	{
		cerr << "ERROR: Usage: ./social_network input.txt" << endl;
		return -1;
	}
	Network net;
	if (net.read_friends(argv[1]) == -1) //check to see if reading file failed
	{
			return -1;
	}
	int option = 1; //keep track of what option the user last inputted
	while (option <= 9 && option >= 1) //as long as the last option was valid, loop
	{
			BEGIN: cout << "Input a number:" << endl; //return place when error occurs
			cout << '\t' << "Option 1. Add a user" << endl;
			cout << '\t' << "Option 2. Add a friend connection" << endl;
			cout << '\t' << "Option 3. Remove a friend connection" << endl;
			cout << '\t' << "Option 4. Print users" << endl;
			cout << '\t' << "Option 5. List a user's friends" << endl;
			cout << '\t' << "Option 6. Write user database to file" << endl;
			cout << '\t' << "Option 7. Find shortest relational distance between two people" << endl;
			cout << '\t' << "Option 8. Assess groups in the network" << endl;
			cout << '\t' << "Option 9. Suggest friends for a person" << endl;
			cout << "Input any other number to exit the program" << endl;
			cin >> option; //ask for user's selection of options
			vector<User> network_users = net.return_userlist();
			
			//ADD USER
			if (option == 1) 
			{
				string fname, lname, username;
				int birth, zip;
				vector<int> friends;
				cin >> fname >> lname >> birth >> zip;
				if (cin.fail()) //check if input usage was correct
				{
					cerr << "ERROR: Usage: first_name last_name birthyear zipcode";
					cerr << endl << endl;
					goto BEGIN; //go back to the menu if usage was wrong
				}
				username = fname + " " + lname; //combine first name and last name into a string
				net.add_user(network_users.size(), username, birth, zip, friends);
				cout << "User " << username << " added!" << endl << endl;
			}

			//ADD CONNECTION
			if (option == 2) 
			{
				string fname1, lname1, fname2, lname2, username1, username2;
				cin >> fname1 >> lname1 >> fname2 >> lname2;
				username1 = fname1 + " " + lname1; //combine first pair of names
				username2 = fname2 + " " + lname2; //combine second pair of names
				int id1 = net.get_id(username1); 
				int id2 = net.get_id(username2);
				if (id1 == -1) //check if id1 exists
				{
					cerr << "ERROR: Usage: first_name1 last_name1 first_name2 last_name2" << endl;
					cerr << "Username 1 does not exist" << endl << endl;
					goto BEGIN;
				}
				else if (id2 == -1) //check if id2 exists
				{
					cerr << "ERROR: Usage: first_name1 last_name1 first_name2 last_name2" << endl;
					cerr << "Username 2 does not exist" << endl << endl;
					goto BEGIN;
				}
				net.add_connection(id1, id2);
				cout << "Connection added between " << username1 << " and ";
				cout << username2 << "!" << endl << endl;
			}

			//REMOVE CONNECTION
			if (option == 3)
			{
				string fname1, lname1, fname2, lname2, username1, username2;
				cin >> fname1 >> lname1 >> fname2 >> lname2;
				username1 = fname1 + " " + lname1; //combine first pair of names
				username2 = fname2 + " " + lname2; //combine second pair of names
				int id1 = net.get_id(username1);
				int id2 = net.get_id(username2);
				if (id1 == -1) //check if id1 exists
				{
					cerr << "ERROR: Usage: first_name1 last_name1 first_name2 last_name2" << endl;
					cerr << "Username 1 does not exist" << endl << endl;
					goto BEGIN;
				}
				else if (id2 == -1) //check if id2 exists
				{
					cerr << "ERROR: Usage: first_name1 last_name1 first_name2 last_name2" << endl;
					cerr << "Username 2 does not exist" << endl << endl;
					goto BEGIN;
				}
				net.remove_connection(id1, id2);
				cout << "Connection removed between " << username1 << " and ";
				cout << username2 << "!" << endl << endl;
			}

			//PRINT NETWORK'S USERS AND INFO
			if (option == 4)
			{
				ostringstream storage;
				const int id_w = 5; //width of ID column
				const int user_w = 20; //width of username column
				const int year_w = 8; //width of birthyear column
				const int zip_w = 10; //width of zipcode column
				//HEADINGS ROW
				storage << setw(id_w) << "ID"; 
				storage << setw(user_w) << "Name";
				storage << setw(year_w) << "Year";
				storage << setw(zip_w) << "Zip";
				storage << endl;
				//ROW OF "=" 
				storage << setw(50) << setfill('=') << "" << endl;
				storage << setfill(' ');
				//ROWS OF INFORMATION
				for (unsigned int i = 0; i < network_users.size(); i++)
				{
					int user_id = network_users[i].return_id();
					string user_name = network_users[i].return_name();
					int user_birth = network_users[i].return_birth();
					int user_zip = network_users[i].return_zip();
					storage << setw(id_w) << user_id << ".";
					storage << setw(user_w) << user_name;
					storage << setw(year_w) << user_birth;
					storage << setw(zip_w) << user_zip;
					storage << endl;
				}
				cout << storage.str() << endl; //print stringstream contents
				cout << endl;
			}

			//PRINT USER'S FRIENDS AND INFO
			if (option == 5)
			{
				string fname, lname, username;
				cin >> fname >> lname;
				username = fname + " " + lname;
				int user_id = net.get_id(username);
				if (user_id == -1) //check if username exists in network's list of users
				{
					cerr << "ERROR: Usage: first_name last_name" << endl;
					cerr << "ERROR: Specified user does not exist" << endl << endl;
					goto BEGIN;
				}
				ostringstream storage;
				const int id_w = 5; //width of ID column
				const int user_w = 20; //width of username column
				const int year_w = 8; //width of birthyear column
				const int zip_w = 10; //width of zipcode column
				//HEADINGS ROW
				storage << setw(id_w) << "ID";
				storage << setw(user_w) << "Name";
				storage << setw(year_w) << "Year";
				storage << setw(zip_w) << "Zip";
				storage << endl;
				//ROW OF "="
				storage << setw(50) << setfill('=') << "" << endl;
				storage << setfill(' ');
				
				vector<int> user_friends = network_users[user_id].return_friends();
				//ROWS OF INFORMATION
				for (unsigned int i = 0; i < user_friends.size(); i++)
				{
					int friends_id = user_friends[i];
					string friend_name = network_users[friends_id].return_name();
					int friends_birth = network_users[friends_id].return_birth();
					int friends_zip = network_users[friends_id].return_zip();
					storage << setw(id_w) << friends_id << ".";
					storage << setw(user_w) << friend_name;
					storage << setw(year_w) << friends_birth;
					storage << setw(zip_w) << friends_zip;
					storage << endl;
				}
				
				cout << storage.str() << endl; //print stringstream contents
				cout << endl;
			}

			//WRITE NETWORK AND USERS' INFO TO FILE
			if (option == 6)
			{
				string input;
				cin >> input;
				if (cin.fail()) //check if the file to be written is invalid
				{
					cerr << "ERROR: You need to specify a file to write to";
					cerr << endl << endl;
					goto BEGIN;
				}
				//MAKE FILENAME STRING -> CHAR ARRAY TO USE IN WRITE_FRIENDS FXN
				int length = input.length(); //number of letters in filename
				char* filename = new char[length + 1]; //create a char array to hold letters
				for (int i = 0; i < length; i++)
				{
					*(filename + i) = input[i]; //copy letters
				}
				*(filename + length) = '\0'; //add the '\0', very important
				net.write_friends(filename); //call the write_friends fxn
				cout << "Wrote network info to " << filename << "!" << endl << endl;
				delete [] filename; //deallocate memory
			}
			//FIND SHORTEST PATH BETWEEN TWO PEOPLE
			if (option == 7)
			{
				ostringstream storage;
				string fname1, lname1, fname2, lname2, username1, username2;
				cin >> fname1 >> lname1 >> fname2 >> lname2;
				username1 = fname1 + " " + lname1; //combine first pair of names
				username2 = fname2 + " " + lname2; //combine second pair of names
				int id1 = net.get_id(username1);
				int id2 = net.get_id(username2);
				if (id1 == -1) //check if id1 exists
				{
					cerr << "ERROR: Usage: first_name1 last_name1 first_name2 last_name2" << endl;
					cerr << "Username 1 does not exist" << endl << endl;
					goto BEGIN;
				}
				else if (id2 == -1) //check if id2 exists
				{
					cerr << "ERROR: Usage: first_name1 last_name1 first_name2 last_name2" << endl;
					cerr << "Username 2 does not exist" << endl << endl;
					goto BEGIN;
				}
				vector<int> result = net.shortest_path(id1, id2); //use shortest_path fxn
				int depth = result.size() - 1;
				if (result.size() == 0)
				{
					storage << "None" << endl;
				}
				else
				{
					storage << "Depth: " << depth << endl;
					for (unsigned int i = 0; i < result.size(); i++)
					{
						string username = network_users[result[i]].return_name();
						if (i != result.size() - 1) //if it's not the last entry
						{
							storage << username << " --> ";
							//put an arrow after the name
						}
						else
						{
							storage << username << endl;
						}
					}
				}
				cout << storage.str() << endl;
			}
			//DISJOINT GROUPS OF PEOPLE
			if (option == 8)
			{
				ostringstream storage;
				vector<vector<int> > groups = net.groups();
				for (unsigned int i = 0; i < groups.size(); i++)
				{
					vector<int> set = groups[i];
					storage << "Set " << i + 1 << ": ";
					for (unsigned int j = 0; j < set.size(); j++)
					{
						string username = network_users[set[j]].return_name();
						storage << username;
						if (j != set.size() - 1)
						//print a comma if not the last entry
						{
							storage << ", ";
						}
					}
					storage << endl;
				}
				cout << storage.str() << endl;
				cout << endl;
			}
			//SUGGEST FRIENDS
			if (option == 9)
			{
				ostringstream storage;
				string fname, lname, username;
				cin >> fname >> lname;
				username = fname + " " + lname;
				int id = net.get_id(username);
				if (id == -1) //check if username exists
				{
					cerr << "ERROR: Usage: first_name last_name" << endl;
					cerr << "Username does not exist" << endl;
					goto BEGIN;
				}
				int score;
				vector<int> suggestions = net.suggest_friends(id, score);
				storage << "Suggested friend(s) is/are: " << endl;
				if (suggestions.size() == 0) //if no suggestions
				{
					storage << "None" << endl;
				}
				else
				{
					for (unsigned int i = 0; i < suggestions.size(); i++)
					{
						string name = network_users[suggestions[i]].return_name();
						storage << '\t' << name << '\t' << '\t' << "score: " << score << endl;
					}
				}
				cout << storage.str() << endl;
				
			}
	}
	//if option selected was not 1 through 9, exit the program.
	cout << "Exiting program" << endl;
	cout << endl;
  return 0;
} 
