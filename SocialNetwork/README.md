This program is a text-based social network. It parses a txt file of users and stores it in a database. The social network has 9 functions:

1. Add a user to the database
2. Add a friend connection between two specificed users
3. Remove a friend connection between two specified users
4. Print all users on the database
5. List a specified user's friends
6. Export all information on the database involving all users to a specified file
7. Find the shortest relational distance (the smallest number of degrees) between two specified people
8. Assess groups of friends (possible cliques) on the network
9. Suggest friends for a specified user

The users.txt are in the "users" directory. There are three possible users.txt:

users_small.txt: 8 users
users_large.txt: 44 users
users_large_disjoint.txt: 44 users with disjoint groups of friends

To compile, use "make". Then run as follows:

	./social_network users.txt

To choose an option, input one of the valid option numbers. Then, give the correct parameters.
