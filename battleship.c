#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <ncurses.h>
#include <time.h>
#include <unistd.h>

#define ROWS 8
#define COLUMNS 8
#define NUMSTRINGS 5
#define SLEEPTIME 1250000

/*	Prototypes	*/
void initializeMaps(int computerBoatMap[ROWS][COLUMNS],int userBoatMap[ROWS][COLUMNS], int userFiredLocs[ROWS][COLUMNS], int compFiredLocs[ROWS][COLUMNS]);

void drawUserBoard(WINDOW *localwin, int map[ROWS][COLUMNS]);

void drawCompBoard(WINDOW *localwin, int map[ROWS][COLUMNS]);

void printAxis(int winRows);

void generateComputerShip(int map[ROWS][COLUMNS], int num);

void showShips(int map[ROWS][COLUMNS]);

void generateUserShip(WINDOW * localwin, WINDOW * inputwin, int map[COLUMNS][ROWS], int num, int printRow, int printCol);

void getFireCoordinates(WINDOW *localwin, int *x, int *y, char *affResponses[NUMSTRINGS], char *negResponses[NUMSTRINGS], int firedLocs[ROWS][COLUMNS], int printRow, int printCol);

void updateCompBoard(WINDOW *localwin, int map[ROWS][COLUMNS], int x, int y, int printRow, int printCol);

void computerTurn(WINDOW *localwin, int map[ROWS][COLUMNS], int firedLocs[ROWS][COLUMNS], int *lastX, int *lastY, int printRow, int printCol);

int checkWinStatus(int computerBoatMap[ROWS][COLUMNS],int userBoatMap[ROWS][COLUMNS], int compFiredLocs[ROWS][COLUMNS], int userFiredLocs[ROWS][COLUMNS]);

void finalResults(int winner, int printRow, int printCol);

/* 	Struct declaration 	*/
struct Location
{
	int x,y;
};

/*	Start of Program	*/
int main()
{
	//randomize
	srand(time(NULL));
	
/*	Variable Declarations	*/
/*----------------------------------------------------------------------------------*/
	//The 2d arrays for maps
	int computerBoatMap[ROWS][COLUMNS];
	int userBoatMap[ROWS][COLUMNS];
	//Game status flag
	int endGameFlag = 0;
	//Coordinate of most recent computer coordinate
	int recX = -1;
	int recY = -1;
	//User fire coordinates
	int x, y;
	//Variable to hold the winner
	int winner = 0;
	//Arrays to hold locations already fired at
	int userFiredLocs[ROWS][COLUMNS];
	int compFiredLocs[ROWS][COLUMNS];
	//Set window size
	int winRows = ROWS + 2;
	int winCols = COLUMNS + 2;
	//Set input window size
	int inputRows = 3;
	int inputCols = 7;
	//variables to hold where print to screen
	int printRow = winRows * 2 + inputRows + 3;
	int printCol = 0;
	//Arrays to hold options of responses to actions
	char *affirmatives[NUMSTRINGS] = {"FIRING!!!", 
					"KOBE!",
					"On it!",
					"Doing it!", 
					"Their gift has been sent"};
	char *negatives[NUMSTRINGS] = {"That won't work, pick another", 
					"How about choose a new one", 	
					"No can do, try again", 
					"Are you being funny? Retry", 
					"You can't do that, do it again"};
/*----------------------------------------------------------------------------------*/
	//Initialize maps
	initializeMaps(computerBoatMap, userBoatMap, userFiredLocs, compFiredLocs);
	
	//Start main window
	initscr();
	noecho();
	cbreak();
	keypad(stdscr, TRUE);
	
/*	Internal Windows 	*/
/*----------------------------------------------------------------------------------*/
	//Create computer window
	WINDOW * compwin = newwin(winRows,winCols,2,20);
	box(compwin, 0,0);
	
	//Create user window
	WINDOW * userwin = newwin(winRows,winCols,winRows + 3,20);
	box(userwin, 0,0);
	
	//Create input window
	WINDOW * inputwin = newwin(inputRows, inputCols, winRows * 2 + 3, 20);
	box(inputwin,0,0);
/*----------------------------------------------------------------------------------*/
	//print the numbers and letters
	printAxis(winRows);

	//update screen
	refresh();
	wrefresh(compwin);
	wrefresh(userwin);
	wrefresh(inputwin);
	
	//Generate computer boats
	generateComputerShip(computerBoatMap, 3);	
	generateComputerShip(computerBoatMap, 2);	
	generateComputerShip(computerBoatMap, 1);
	
	//Show computer ships
	showShips(computerBoatMap);
	
	//Draw the two boards
	drawCompBoard(compwin, computerBoatMap);
	drawUserBoard(userwin, userBoatMap);
	
	//Generate user ships
	generateUserShip(userwin, inputwin, userBoatMap, 3, printRow, printCol);
	generateUserShip(userwin, inputwin, userBoatMap, 2, printRow, printCol);
	generateUserShip(userwin, inputwin, userBoatMap, 1, printRow, printCol);
	
	
	
/*	Main Loop	*/
/*----------------------------------------------------------------------------------*/
	do
	{
		//Prompt user for fire coordinates
		getFireCoordinates(inputwin, &x, &y, affirmatives, negatives, userFiredLocs, printRow, printCol);
		
		//Update computer boat map
		updateCompBoard(compwin, computerBoatMap, x, y, printRow, printCol);
		
		//Generate computer fire location
		computerTurn(userwin, userBoatMap, compFiredLocs, &recX, &recY, printRow, printCol);
		
		//Check if game is over
		winner = checkWinStatus(computerBoatMap, userBoatMap, compFiredLocs, userFiredLocs);
		
		refresh();
	}while(winner == 0);
/*----------------------------------------------------------------------------------*/
	//Show the win or lose message
	finalResults(winner, printRow, printCol);
	
	endwin();
	return 0;
}

//Function to initialize 2 map arrays
void initializeMaps(int computerBoatMap[ROWS][COLUMNS], int userBoatMap[ROWS][COLUMNS], int userFiredLocs[ROWS][COLUMNS], int compFiredLocs[ROWS][COLUMNS])
{
	for(int i = 0; i < ROWS; i++)
	{
		for(int j = 0; j < COLUMNS; j++)
		{
			computerBoatMap[i][j] = 0;
			userBoatMap[i][j] = 0;
			userFiredLocs[i][j] = 0;
			compFiredLocs[i][j] = 0;
		}
	}
}

//Function to print out user board
void drawUserBoard(WINDOW *localwin, int map[ROWS][COLUMNS])
{
	//loop through each element in map and display the correct item
	for(int i = 0; i < ROWS; i++)
	{
		for(int j = 0; j < COLUMNS; j++)
		{
			if(map[i][j] == 1)
			{
				mvwprintw(localwin, i + 1, j + 1, "O");
			}
			else
			{
				mvwprintw(localwin, i + 1, j + 1, ".");
			}
		}
	}
	wrefresh(localwin);
}

//Function to print out computer board
void drawCompBoard(WINDOW *localwin, int map[ROWS][COLUMNS])
{
	//print a period in all computer map spots
	for(int i = 0; i < ROWS; i++)
	{
		for(int j = 0; j < COLUMNS; j++)
		{
			mvwprintw(localwin, i + 1, j + 1, ".");
		}
	}
	wrefresh(localwin);
}

//Function to generate computer ship locations
void generateComputerShip(int map[ROWS][COLUMNS], int num)
{
	//variable to determine orientation of the ship
	int horizontalFlag = rand() % 2;
	//variable to store number of open spaces in row or column
	int sum = 0;
	//variables to hold the x and y coordinates generated by random number
	int x = 0;
	int y = 0;
	
	do
	{
		//set random coordinates
		x = rand() % COLUMNS;
		y = rand() % ROWS;
		sum = 0;

		if(horizontalFlag)
		{
			//go through x values with same y coordinate
			for(int i = x; i < COLUMNS; i++)
			{
				//if slot is open add to sum
				if(map[y][i] == 0)
				{
					sum++;
				}
				//otherwise exit loop because cant fit ship
				else
				{
					break;
				}
			}
			//if there is enough room for the ship to be placed
			if(sum >= num)
			{
				//place the ship 
				for(int j = x; j < x + num; j++)
				{
					map[y][j] = 1;
				}
			}
		}
		else
		{
			//go through y values with the same x coordinate
			for(int i = y; i < ROWS; i++)
			{
				//if slot is open add to sum
				if(map[i][x] == 0)
				{
					sum++;
				}
				//otherwise exit loop because cant fit ship
				else
				{
					break;
				}
			}
			//if there is enough room for the ship to be placed
			if(sum >= num)
			{
				//place the ship 
				for(int j = y; j < y + num; j++)
				{
					map[j][x] = 1;
				}
			}
		}
	}while(sum < num);
}

//Function to print the axis of both boards
void printAxis(int winRows)
{
	char place = 'A';
	for(int i = 0; i < ROWS; i++)
	{
		mvprintw(1, i + 21, "%d", i + 1);
		mvprintw(winRows + 2, i + 21, "%d", i + 1);
	}
	for(int j = 0; j < COLUMNS; j++)
	{
		mvprintw(j + 3, 19, "%c", place);
		mvprintw(j + winRows + 4, 19, "%c", place);
		place++;
	}
}

//Function to print computer ships for testing
void showShips(int map[ROWS][COLUMNS])
{
	for(int i = 0; i < ROWS; i++)
	{
		for(int j = 0; j < COLUMNS; j++)
		{
			mvprintw(i,j, "%d", map[i][j]);
		}
	}
	refresh();
}

//Function to place a user ship of num size
void generateUserShip(WINDOW * localwin, WINDOW * inputwin, int map[COLUMNS][ROWS], int num, int printRow, int printCol)
{
	//variable to hold the location of the ship slots
	struct Location locs[num];
	//variable to switch location x and y
	int temp[num];
	//flag to tell if move is legal
	int invalidMoveFlag;
	//flag to tell if done with placing
	int finishFlag = 0;
	
	//set the spawn location of the ship
	for(int i = 0; i < num; i++)
	{
		locs[i].x = 2;
		locs[i].y = i + 1;
	}
	
	//print the spawned ship
	for(int i = 0; i < num; i++)
	{
		mvwprintw(localwin, locs[i].y + 1, locs[i].x + 1, "O");
	}
	wrefresh(localwin);
	
	//while the user is still placing
	while(!finishFlag)
	{
		mvprintw(printRow,printCol, "Use the arrow keys to move the boat, press x to rotate, press z to place");
		
		invalidMoveFlag = 0;
		int ch = getch();
		
		//case system for each arrow key and the two control keys
		switch(ch)
		{
			case KEY_UP:
				//go through each location and check if it can move
				for(int i = 0; i < num; i++)
				{
					if(locs[i].y - 1 < 0)
					{
						invalidMoveFlag = 1;
						break;
					}
				}
				//If it can move set the new location values and draw them
				if(!invalidMoveFlag)
				{
					drawUserBoard(localwin, map);
					for(int i = 0; i < num; i++)
					{
						locs[i].y = locs[i].y - 1;
						mvwprintw(localwin, locs[i].y + 1, locs[i].x + 1, "O");
					}
				}
				wrefresh(localwin);
				break;
			case KEY_DOWN:
				//go through each location and check if it can move
				for(int i = 0; i < num; i++)
				{
					if(locs[i].y + 1 > ROWS - 1)
					{
						invalidMoveFlag = 1;
						break;
					}
				}
				//If it can move set the new location values and draw them
				if(!invalidMoveFlag)
				{
					drawUserBoard(localwin, map);
					for(int i = 0; i < num; i++)
					{
						locs[i].y = locs[i].y + 1;
						mvwprintw(localwin, locs[i].y + 1, locs[i].x + 1, "O");
					}
				}
				wrefresh(localwin);
				break;
			case KEY_LEFT:
				//go through each location and check if it can move
				for(int i = 0; i < num; i++)
				{
					if(locs[i].x - 1 < 0)
					{
						invalidMoveFlag = 1;
						break;
					}
				}
				//If it can move set the new location values and draw them
				if(!invalidMoveFlag)
				{
					drawUserBoard(localwin, map);
					for(int i = 0; i < num; i++)
					{
						locs[i].x = locs[i].x - 1;
						mvwprintw(localwin, locs[i].y + 1, locs[i].x + 1, "O");
					}
				}
				wrefresh(localwin);
				break;
			case KEY_RIGHT:
				//go through each location and check if it can move
				for(int i = 0; i < num; i++)
				{
					if(locs[i].x + 1 > COLUMNS - 1)
					{
						invalidMoveFlag = 1;
						break;
					}
				}
				//If it can move set the new location values and draw them
				if(!invalidMoveFlag)
				{
					drawUserBoard(localwin, map);
					for(int i = 0; i < num; i++)
					{
						locs[i].x = locs[i].x + 1;
						mvwprintw(localwin, locs[i].y + 1, locs[i].x + 1, "O");
					}
				}
				wrefresh(localwin);
				break;
			case 120:			//x key (rotate)
				drawUserBoard(localwin, map);
				//switch the x and y of each location
				for(int i = 0; i < num; i++)
				{
					temp[i] = locs[i].x;
					locs[i].x = locs[i].y;
					locs[i].y = temp[i];
				}
				//print the rotated boat
				for(int i = 0; i < num; i++)
				{
					mvwprintw(localwin, locs[i].y + 1, locs[i].x + 1, "O");
				}
				wrefresh(localwin);
				break;
			case 122:			//z key (set)
				//check if attempted spot is taken
				for(int i = 0; i < num; i++)
				{
					if(map[locs[i].y][locs[i].x] == 1)
					{
						invalidMoveFlag = 1;
						break;
					}
				}
				//If legal position set the map and indicate that it is finished
				if(!invalidMoveFlag)
				{
					for(int i = 0; i < num; i++)
					{
						map[locs[i].y][locs[i].x] = 1;
					}
					finishFlag = 1;
				} 
				//Else print occupied message
				else
				{
					//clear output line
					move(printRow, printCol);
					clrtoeol();
					mvprintw(printRow,printCol, "That spot is occupied");
					refresh();
					usleep(SLEEPTIME);
				}
				break;
		}
		//clear output line
		move(printRow, printCol);
		clrtoeol();
	}
	//clear output line
	move(printRow, printCol);
	clrtoeol();
	refresh();	
}

//Function to get fire coordinates from user
void getFireCoordinates(WINDOW *localwin, int *x, int *y, char *affResponses[NUMSTRINGS], char *negResponses[NUMSTRINGS], int firedLocs[ROWS][COLUMNS], int printRow, int printCol)
{
	//Set user fire coordinates
	*x = -1;
	*y = -1;
	//The character the user inputs
	char inputX, inputY;
	
	//loop until coordinate is legal
	do{
		mvprintw(printRow, printCol, "Choose a coordinate");
		refresh();
		//Delete previous input
		mvwprintw(localwin, 1, 2, "   ");
		wrefresh(localwin);
		
		//loop until the user gives a letter first
		do
		{
			inputY = wgetch(localwin);
			inputY = toupper(inputY);
		}while(!isalpha(inputY));
		//set *y to integer value of the char inputY
		*y = inputY - 'A';
		mvwprintw(localwin, 1, 2, "%c", inputY);
		
		//loop until the second input is a digit
		do{
			inputX = wgetch(localwin);
		}while(!isdigit(inputX));
		//set *x to integer value of the char inputX
		*x = inputX - '1';
		mvwprintw(localwin, 1, 4, "%c", inputX);
		wrefresh(localwin);
		
		//clear ouput line
		move(printRow, printCol);
		clrtoeol();
		
		//If move is not legal print the negative message
		if(((*x < 0 || *x > COLUMNS - 1) || (*y < 0 || *y > ROWS - 1)) || firedLocs[*y][*x] == 1)
		{
			mvprintw(printRow, printCol, "%s", negResponses[rand() % NUMSTRINGS]);
		}
		//Else print affirmative message
		else
		{
			mvprintw(printRow, printCol, "%s", affResponses[rand() % NUMSTRINGS]);
		}
		refresh();
		usleep(SLEEPTIME);
		
		//reset ouput lines
		move(printRow, printCol);
		clrtoeol();
		refresh();
	}while(((*x < 0 || *x > COLUMNS - 1) || (*y < 0 || *y > ROWS - 1)) || firedLocs[*y][*x] == 1);
	
	//set the most recent fired location
	firedLocs[*y][*x] = 1;
	
}

//Function to update the board with newly fired coordinates
void updateCompBoard(WINDOW *localwin, int map[ROWS][COLUMNS], int x, int y, int printRow, int printCol)
{
	//Display if the user missed or hit. Set the x y coordinate to correct item
	if(map[y][x] == 1)
	{
		mvwprintw(localwin, y + 1, x + 1, "X");
		mvprintw(printRow, printCol, "They're hit!");
	}
	else
	{
		mvwprintw(localwin, y + 1, x + 1, "O");
		mvprintw(printRow, printCol, "We missed!");
	}
	wrefresh(localwin);
	refresh();
	usleep(SLEEPTIME);
	
	//reset ouput lines
	move(printRow, printCol);
	clrtoeol();
	refresh();
} 

//Function to generate a location for the computer to fire
void computerTurn(WINDOW *localwin, int map[ROWS][COLUMNS], int firedLocs[ROWS][COLUMNS], int *lastX, int *lastY, int printRow, int printCol)
{
	int x;
	int y;
	int dir = -1;
	int dirs[4] = {0, 1, 2, 3};
	
	//Loop until found new random coordinate to use
	do
	{
		x = rand() % ROWS;
		y = rand() % COLUMNS;
	}while(firedLocs[y][x] == 1);
	
	//Set new fired location
	firedLocs[y][x] = 1;
	*lastX = x;
	*lastY = y;
	
	//Display if the computer missed or hit and set the x y coordinate to correct item
	if(map[y][x] == 1)
	{
		mvwprintw(localwin, y + 1, x + 1, "X");
		mvprintw(printRow, printCol, "We got hit!");
	}
	else
	{
		mvwprintw(localwin, y + 1, x + 1, "!");
		mvprintw(printRow, printCol, "They missed!");
	}
	wrefresh(localwin);
	refresh();
	usleep(SLEEPTIME);
	
	//reset ouput lines
	move(printRow, printCol);
	clrtoeol();
	refresh();
}

//Function to determine whether or not to continue the game loop, returns 1 if player won, returns 2 if computer won
int checkWinStatus(int computerBoatMap[ROWS][COLUMNS],int userBoatMap[ROWS][COLUMNS], int compFiredLocs[ROWS][COLUMNS], int userFiredLocs[ROWS][COLUMNS])
{
	int userCounter = 0;
	int compCounter = 0;

	//for each boat the user and computer hit add one to counter
	for(int i = 0; i < ROWS; i++)
	{
		for(int j = 0; j < COLUMNS; j++)
		{
			if(computerBoatMap[i][j] == 1 && userFiredLocs[i][j] == 1)
			{
				userCounter++;
			}
			if(userBoatMap[i][j] == 1 && compFiredLocs[i][j] == 1)
			{
				compCounter++;
			}
		}
	}
	//if counters = 6 return the winner 
	if(userCounter == 6)
	{
		return 1;
	}
	else if(compCounter == 6)
	{
		return 2;
	}
	//else game keeps going
	else 
	{
		return 0;
	}
}

//function to handle final screen
void finalResults(int winner, int printRow, int printCol)
{
	//display the correct message for who won
	if(winner == 1)
	{
		mvprintw(printRow, printCol, "We won!");
	}
	else
	{
		mvprintw(printRow, printCol, "We lost!");
	}
	refresh();
	usleep(SLEEPTIME * 3);
}
