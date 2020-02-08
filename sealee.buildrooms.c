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
	"Saferoom",
	"Bossroom",
	"Acidroom",
	"Coalroom",
	"Coinroom",
	"Hyperoom",
	"Herbroom",
	"Lionroom",
	"Moldroom",
	"Traproom"
};

/*****************************************************************************************************************
 * NAME: IsGraphFull
 * SYNOPSIS: Checks to see if a given array of rooms is full
 * DESCRIPTION: If every room in the array has more than 3 connections, returns true. Otherwise, returns false.
 * AUTHOR: Jett Seale (sealee@oregonstate.edu)
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
 * NAME: GetRandomRoom
 * SYNOPSIS: Grabs a random room from the array
 * DESCRIPTION: Randomly generates a number and uses that as an index to get a random room from the array.
 * AUTHOR: Jett Seale (sealee@oregonstate.edu)
 * **************************************************************************************************************/
struct room getRandomRoom(struct room* rooms) {
	int randNum = rand() % 7;
	return rooms[randNum];
}

/*****************************************************************************************************************
 * NAME: CanAddConnectionFrom
 * SYNOPSIS: Checks to see if a connection can be added to a given room
 * DESCRIPTION: Returns true if the room has less than 6 connections, false otherwise
 * AUTHOR: Jett Seale (sealee@oregonstate.edu)
 * **************************************************************************************************************/
bool canAddConnectionFrom(struct room x) {
	if (x.numConnections < 6) {
		return true;
	} else {
		return false;
	}
}

/*****************************************************************************************************************
 * NAME: ConnectionAlreadyExists
 * SYNOPSIS: Checks to see if two rooms are already connected
 * DESCRIPTION: If one of the rooms already has the other's id in its connections array, returns true
 * AUTHOR: Jett Seale (sealee@oregonstate.edu)
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
 * NAME: ConnectRoom
 * SYNOPSIS: Connects two rooms
 * DESCRIPTION: Increments the number of connections for both rooms and adds them to each other's connetion arrays
 * AUTHOR: Jett Seale (sealee@oregonstate.edu)
 * **************************************************************************************************************/
void connectRoom(struct room* x, struct room* y) {
	x->connections[x->numConnections] = y;
	x->numConnections++;
	y->connections[y->numConnections] = x;
	y->numConnections++;
	return;
}

/*****************************************************************************************************************
 * NAME: IsSameRoom
 * SYNOPSIS: Checks to see if two rooms are the same room
 * DESCRIPTION: If their ID's are the same, returns true. Otherwise, false
 * AUTHOR: Jett Seale (sealee@oregonstate.edu)
 * **************************************************************************************************************/
bool isSameRoom(struct room x, struct room y) {
	if (x.id == y.id) {
		return true;
	} else {
		return false;
	}
}

/*****************************************************************************************************************
 * NAME: AddRandomConnection
 * SYNOPSIS: Loops until the room's connections are full, adding random connections along the way
 * DESCRIPTION: Checks to see if connections are valid before connecting
 * AUTHOR: Jett Seale (sealee@oregonstate.edu)
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
 * NAME: Main
 * SYNOPSIS: Creates an array of room structs, connects them, reads data into files
 * DESCRIPTION: Is the main function called at the start of the program
 * AUTHOR: Jett Seale (sealee@oregonstate.edu)
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
		if (i == 0) {
			rooms[i].type = "START_ROOM";
		} else if (i == 6) {
			rooms[i].type = "END_ROOM";
		} else {
			rooms[i].type = "MID_ROOM";
		}
	}

	//Connect the rooms together:
	while (!isGraphFull(rooms)) {
		addRandomConnection(rooms);	
	}

	//Create and populate directory and files:
	int dirPid = getpid(); //Generate pid to append to directory name
	char dir[NAME_MAX + 1];
	char* dirStart = "sealee.rooms.";
	
	snprintf(dir, NAME_MAX + 1, "%s%d", dirStart, dirPid); //Append directory pid to directory name
	
	struct stat st = {0}; //Make a struct to hold the dir stats
	
	if (stat(dir, &st) == -1) { //If the dir does not already exist
		mkdir(dir, 0755); //Make the directory
		strncat(dir, "/", NAME_MAX + 1); //Append slash to end of dir name

		int l;
		for (l = 0; l < 7; l++) {
			char temp[NAME_MAX + 1]; //Temporary string to hold the path to the lth file
			FILE* file;

			strcpy(temp, dir);
			strncat(temp, rooms[l].name, NAME_MAX + 1); //Append filename to end of dir path
			file = fopen(temp, "w"); //Make file in the correct dir

			fputs("ROOM NAME: ", file); //Insert room names
			fputs(rooms[l].name, file);
			fputs("\n", file);

			int k;
			for (k = 0; k < rooms[l].numConnections; k++) {
				fputs("CONNECTION ", file); //Insert connections
				char conn[NAME_MAX + 1];
				snprintf(conn, NAME_MAX + 1, "%d", k + 1);
				fputs(conn, file);
				fputs(": ", file);
				fputs(rooms[l].connections[k]->name, file);
				fputs("\n", file);	
			}

			fputs("ROOM TYPE: ", file); //Insert room types
			fputs(rooms[l].type, file);
			fputs("\n", file);

			fclose(file); //Close file after finishing operations
		}
	} else {
		perror("Error making the directory\n");
	}

	free(rooms);	

	return 0;
}
