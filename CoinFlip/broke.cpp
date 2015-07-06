#include <iostream>
#include <cmath>
#include <cstdlib>
#include <ctime>

using namespace std;

/* DESCRIPTION OF PROGRAM

This is a program that simulates a game where 3 boys flip a coin from their bag of a user-defined number of coins. The boy with the odd side up gets a coin from each of the other two boys. This continues until one boy has all of the coins. The game then calculates and outputs the average number of turns per game.

User inputs:
	Number of coins each boy starts with
	The fairness of the coin landing on heads
	Number of games the boys will play

END DESCRIPTION */


int main()
{
//BEFORE-GAME PARAMETERS.
	srand(time(NULL));

	cout << "How many coins should everyone start out with? " << endl;
	int iv_num_coins;
	cin >> iv_num_coins; //Input the number of coins each boy gets.

	cout << "What should be the probability of landing a Heads? (0.0 to 1.0) " << endl;
	double dv_weight;
	cin >> dv_weight; //Input the fairness of the coins.

	cout << "Lastly, how many games should the boys play? " << endl;
	int iv_games;
	cin >> iv_games; //Input number of games to be played.

	int ia_turns_per_game[iv_games]; 
	//keeps track of number of turns per game. 
	/*I used an array of number of turns for every game instead of 
	keeping a running sum of total number of turns over 
	all previous simulations.*/



//GAME SIMULATION & DATA GATHERING.
	for (int a = 0; a < iv_games; a++) //This is for ia_turns_per_game[iv_games] array.
	{
		int iv_turns = 0; //initialize turn number to 0 every game.
		int ia_p_coins[3] = {iv_num_coins, iv_num_coins, iv_num_coins}; 
		//resets # of coins per player.
		while (ia_p_coins[0] > 0 && ia_p_coins[1] > 0 && ia_p_coins[2] > 0) 
		// Playing the actual game.
		{
			//The actual coin flip
			bool ia_cflip[3] = {false, false, false}; //Tracks coin flip values.
			for (int b = 0; b < 3; b++)
			{
				int i_flip = rand();
				if (i_flip < dv_weight * RAND_MAX) //Fairness of coin.
				{
					ia_cflip[b] = true;
				}
			}
	
			//Update the player coins based upon the outcome.
			if (ia_cflip[0] != ia_cflip[1] && ia_cflip[1] == ia_cflip[2]) 
			//Player 0's flip is unique.
			{
				ia_p_coins[0] += 2;
				ia_p_coins[1] -= 1;
				ia_p_coins[2] -= 1;
			}
			else if (ia_cflip[1] != ia_cflip[2] && ia_cflip[2] == ia_cflip[0]) 
			//Player 1's flip is unique.
			{
				ia_p_coins[1] += 2;
				ia_p_coins[2] -= 1;
				ia_p_coins[0] -= 1;
			}
			else if (ia_cflip[2] != ia_cflip[0] && ia_cflip[0] == ia_cflip[1]) 
			//Player 2's flip is unique.
			{
				ia_p_coins[2] += 2;
				ia_p_coins[0] -= 1;
				ia_p_coins[1] -= 1;
			}
					
			iv_turns += 1; //increments the turn number
		}
		ia_turns_per_game[a] = iv_turns; //Updates number of turns per game.	
	}



//DATA ANALYSIS.
	double dv_avg_turns; //creating a variable for the final result.
	int iv_total_turns = 0;
	for (int c = 0; c < iv_games; c++) // computes total number of turns.
	{
		iv_total_turns += ia_turns_per_game[c];
	}
	double dv_total_turns = iv_total_turns; 
	//change to a double to compute final result.
	double dv_games = iv_games; 
	//change to a double to compute final result.
	dv_avg_turns = (dv_total_turns/dv_games); //computing final result.
	cout << "The average number of turns per game is: " << dv_avg_turns << endl;
}







					
