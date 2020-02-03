#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <limits.h>
#include <stdbool.h>

//Struct room type:
struct room {
	int id;
	char* name;
	int numConnections;
	struct room* connections[6];
	char* type;
};

//Hardcode array of possible room names
char* roomNames[10][10] = {
	"Saferoom\n",
	"Bossroom\n",
	"Acidroom\n",
	"Coalroom\n",
	"Coinroom\n",
	"Hyperoom\n",
	"Herbroom\n",
	"Lionroom\n",
	"Moldroom\n",
	"Traproom\n"
};

/*****************************************************************************************************************
 * NAME: TODO
 * SYNOPSIS:
 * DESCRIPTION:
 * AUTHOR:
 * **************************************************************************************************************/
bool isGraphFull(struct room* rooms) {
	int i;
	for (i = 0; i < 7; i++) {
		if (rooms[i].numConnections < 3) {
			return false;
		}
	}
	return true;
}

/*****************************************************************************************************************
 * NAME: TODO
 * SYNOPSIS:
 * DESCRIPTION:
 * AUTHOR:
 * **************************************************************************************************************/
struct room getRandomRoom(struct room* rooms) {
	int randNum = rand() % 7;
	return rooms[randNum];
}

/*****************************************************************************************************************
 * NAME: TODO
 * SYNOPSIS:
 * DESCRIPTION:
 * AUTHOR:
 * **************************************************************************************************************/
bool canAddConnectionFrom(struct room x) {
	if (x.numConnections < 6) {
		return true;
	} else {
		return false;
	}
}

/*****************************************************************************************************************
 * NAME: TODO
 * SYNOPSIS:
 * DESCRIPTION:
 * AUTHOR:
 * **************************************************************************************************************/
bool connectionAlreadyExists(struct room x, struct room y) {
	int i;
	for (i = 0; i < y.numConnections; i++) {
		if (x.id == y.connections[i]->id) {
			return true;
		}
	}
	return false;
}

/*****************************************************************************************************************
 * NAME: TODO
 * SYNOPSIS:
 * DESCRIPTION:
 * AUTHOR:
 * **************************************************************************************************************/
void connectRoom(struct room* x, struct room* y) {
	x->connections[x->numConnections] = y;
	x->numConnections++;
	y->connections[y->numConnections] = x;
	y->numConnections++;
	return;
}

/*****************************************************************************************************************
 * NAME: TODO
 * SYNOPSIS:
 * DESCRIPTION:
 * AUTHOR:
 * **************************************************************************************************************/
bool isSameRoom(struct room x, struct room y) {
	if (x.id == y.id) {
		return true;
	} else {
		return false;
	}
}

/*****************************************************************************************************************
 * NAME: TODO
 * SYNOPSIS:
 * DESCRIPTION:
 * AUTHOR:
 * **************************************************************************************************************/
void addRandomConnection(struct room* rooms) {
	struct room a, b;

	while (true) {
		a = getRandomRoom(rooms);
		if (canAddConnectionFrom(a)) {
			break;
		}
	}

	do {
		b = getRandomRoom(rooms);
	} while (!canAddConnectionFrom(b) || isSameRoom(a, b) || connectionAlreadyExists(a, b));

	connectRoom(&rooms[a.id], &rooms[b.id]);
	return;
}

/*****************************************************************************************************************
 * NAME: TODO
 * SYNOPSIS:
 * DESCRIPTION:
 * AUTHOR:
 * **************************************************************************************************************/
int main() {

	srand(time(0)); //Seed random number generator

	struct room* rooms = calloc(7, sizeof(struct room)); //Create array of rooms to be used in adventure program

	//Create an array of random numbers to be used to ranadomize room names:
	int randNums[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9}; //Initialize array

	//Shuffle array:
	int j;
	for (j = 0; j < 10; j++) {
		int temp = randNums[j];
		int randj = rand () % 10;
		randNums[j] = randNums[randj];
		randNums[randj] = temp;
	}	
	
	//Initialize the rooms:
	int i;
	for (i = 0; i < 7; i++) {
		rooms[i].id = i;
		rooms[i].numConnections = 0;
		rooms[i].name = roomNames[0][randNums[i]];
	}

	//Connect the rooms together:
	int k = 0;
	while (!isGraphFull(rooms)) {
		printf("Iteration %d\n", k);
		addRandomConnection(rooms);
		k++;	
	}



	free(rooms);	

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

	//for (i=0; i < 7; i++) {
	//	printf(rooms[i]->name);
	//}
	
	/*int dirPid = getpid(); //Generate pid to append to directory name
	char dir[NAME_MAX + 1];
	char *dirStart = "sealee.rooms.";

	snprintf(dir, NAME_MAX + 1, "%s%d", dirStart, dirPid); //Append directory pid to directory name

	struct stat st = {0}; //Make a struct to hold the directory stats

	if (stat(dir, &st) == -1) { //If the directory does not already exist

		(mkdir(dir, 0755)); //Make the directory
		printf("Directory created\n");

		FILE *file;
		int filePid = getpid(); //Generate pid to append to file name
		strncat(dir, "/", NAME_MAX + 1); //Append slash to end of directory name
		char fullPath[NAME_MAX + 1]; 
		char *fileStart = "roomfile"; //Set up the start of the filename
		snprintf(fullPath, NAME_MAX + 1, "%s%d", fileStart, filePid); //Put the file name start and pid together
		strncat(dir, fullPath, NAME_MAX + 1); //Append the filename to the directory path
		strncat(dir, ".txt", NAME_MAX + 1); //Finish fullpath to file by appending file type

		printf(dir);
		printf("\n");
		file = fopen(dir, "w"); //Make the file in the correct directory
		fclose(file); //Close file after finishing operations

	} else {
		perror("Error making directory\n");
	}*/

	return 0;
}
