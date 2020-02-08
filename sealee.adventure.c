#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <dirent.h>
#include <pthread.h>
#include <time.h>

//Threads:
pthread_mutex_t myMutex = PTHREAD_MUTEX_INITIALIZER;
void* get_time(void* argument) {
	pthread_mutex_lock(&myMutex); //Lock thread

	time_t rawTime;
	struct tm* timeInfo;
	time(&rawTime);
	timeInfo = localtime(&rawTime);
	char final[256];
	char temp[256];
	strftime(temp, 256, "%I", timeInfo);
	strncat(final, temp, 256);
	strncat(final, ":", 1);
	strftime(temp, 256, "%M", timeInfo);
	strncat(final, temp, 256);
	strftime(temp, 256, "%P", timeInfo);
	strncat(final, temp, 256);
	strncat(final, ", ", 2);
	strftime(temp, 256, "%A", timeInfo);
	strncat(final, temp, 256);
	strncat(final, ", ", 2);
	strftime(temp, 256, "%B", timeInfo);
	strncat(final, temp, 256);
	strncat(final, " ", 1);
	strftime(temp, 256, "%d", timeInfo);
	strncat(final, temp, 256);
	strncat(final, ", ", 2);
	strftime(temp, 256, "%Y", timeInfo);
	strncat(final, temp, 256); //Format time and date correctly

	printf("\n%s\n", final);
	FILE* timeFile = fopen("./currentTime.txt", "w");
	fputs(final, timeFile);
	fclose(timeFile); //Write to file

	pthread_mutex_unlock(&myMutex); //Unlock thread
	return NULL;
}

/*****************************************************************************************************************
 * NAME: FindNewestDir
 * SYNOPSIS: Finds the newest directory in a given directory
 * DESCRIPTION: Uses readdir to loop through dirs in a given dir and checks the stats for the newest one
 * AUTHOR: Taken from CS344 Canvas page
 * **************************************************************************************************************/
char* findNewestDir () {
	int newestDirTime = -1;
        char targetDirPrefix[32] = "sealee.rooms.";
        char* newestDirName = calloc(256, sizeof(char));
        memset(newestDirName, '\0', sizeof(newestDirName));

        DIR* dirToCheck;
        struct dirent* fileInDir;
        struct stat dirAttributes;

        dirToCheck = opendir(".");

        if (dirToCheck > 0) {
                while ((fileInDir = readdir(dirToCheck)) != NULL) {
                        if (strstr(fileInDir->d_name, targetDirPrefix) != NULL) {
                                stat(fileInDir->d_name, &dirAttributes);

                                if((int)dirAttributes.st_mtime > newestDirTime) {
                                        newestDirTime = (int)dirAttributes.st_mtime;
                                        memset(newestDirName, '\0', sizeof(newestDirName));
                                        strcpy(newestDirName, fileInDir->d_name);
                                }
                        }
                }
        }

        closedir(dirToCheck);

        return newestDirName;
}

/*****************************************************************************************************************
 * NAME: Main
 * SYNOPSIS: Provides UI for game
 * DESCRIPTION: Loops while end game condition is found to be false
 * AUTHOR: Jett Seale (sealee@oregonstate.edu)
 * **************************************************************************************************************/
int main() {

	pthread_mutex_lock(&myMutex); //Lock main thread
	pthread_t timeThread; //Create time thread

	char* dirPath;
	dirPath = findNewestDir();
	strncat(dirPath, "/", 256);	
	
	struct dirent* de;
	DIR* dr = opendir(dirPath);

	if (dr == NULL) {
		perror("Error opening directory");
		exit(1);
	}

	char currentRoom[256];
	char endRoom[256];
	char currentFilePath[256];

	//Find the start room and end rooms:
	FILE* file;
	char fileName[256];
	char line[256];
	memset(fileName, '\0', 256);

	while ((de = readdir(dr)) != NULL) { //Go through the most current dir
		if (strncmp(de->d_name, ".", 256) && strncmp(de->d_name, "..", 256)) { //Ignore . and .. dirs
			char fullPath[256];
			memset(fullPath, '\0', 256);
			sprintf(fullPath, "%s", dirPath);
			sprintf(fileName, "%s", de->d_name);
			strncat(fullPath, fileName, 256);
			file = fopen(fullPath, "r"); //Open correct file
			if (file == NULL) { 
				perror("Error opening file");
				exit(1);
			}
			while (!feof(file)) {
				fgets(line, 256, file);
				if (!strncmp(line, "ROOM TYPE: START_ROOM\n", 256)) {
					strcpy(currentRoom, de->d_name);
					strcpy(currentFilePath, fullPath); //Find the start room
				} else if (!strncmp(line, "ROOM TYPE: END_ROOM\n", 256)) {
					strcpy(endRoom, de->d_name); //Find the end room
				}
			}
			fclose(file);
		}
	}

	char possibleConnections[6][256];
	FILE* currentFile;
	char playerPath[256][256];
	int stepCount = 0;

	while (true) { //Loops until the player finds the end room
		//Find all current room's outbound connections:
		currentFile = fopen(currentFilePath, "r");
		if (currentFile == NULL) {
			perror("Error opening file");
			exit(1);
		}

		char sentence[256];
		char firstLetter;
		memset(sentence, '\0', 256);
		int inc = 0;
		while (!feof(currentFile)) {
			fgets(sentence, 256, currentFile);
			firstLetter = sentence[0];
			if (firstLetter == 'C') {
				strcpy(possibleConnections[inc], sentence + 14); //Find the possible connections in a file by counting the number of lines that start with C
				++inc;		
			}
		}

		fclose(currentFile);

		//Trim the \n character from each possible connection name:
		int j;
		for (j = 0; j < inc; j++) {
			possibleConnections[j][strlen(possibleConnections[j]) - 1] = '\0';	
		}

		//Print UI:
		bool invalidInput = true;
		char newRoomName[256];
		while (invalidInput) {
			char input[256];
			printf("\nCURRENT LOCATION: %s\n", currentRoom);
			printf("\nPOSSIBLE CONNECTIONS: ");
			int i;
			for (i = 0; i < inc; i++) {
				printf("%s", possibleConnections[i]); //Print possible locations
				if (i == inc - 1) {
					printf(".");
				} else {
					printf(", ");
				}
			}
			printf("\n");
			printf("\nWHERE TO? > ");
		
			scanf("%s", input);
			
			int k;
			for (k = 0; k < inc; k++) {
				if (!strncmp(possibleConnections[k], input, 8)) {
					invalidInput = false; //If the input equals one of the possible connections, set to false
				}
			}
			
			if (invalidInput) {
				if (!strncmp(input, "time", 4)) { //Do time thread call here
					pthread_create(&timeThread, NULL, get_time, NULL);
					pthread_mutex_unlock(&myMutex);
					pthread_join(timeThread, NULL);
					pthread_mutex_lock(&myMutex); 
					
				} else {
					printf("\nHUH? I DON'T UNDERSTAND THAT ROOM. TRY AGAIN.\n"); //If not time and not a valid connection, reprompt player
				}
			} else {

				//This is where I set the new values for the new room the player entered:
				strncpy(newRoomName, input, 8);
				strncpy(playerPath[stepCount], input, 8);
				stepCount++;

				memset(currentFilePath, '\0', 256);
				sprintf(currentFilePath, dirPath, 256);
				strncat(currentFilePath, newRoomName, 8);

				memset(currentRoom, '\0', 256);
				strncat(currentRoom, newRoomName, 8);
			}
		}
		
		if (!strncmp(endRoom, newRoomName, 8)) {
			break; //If the current room equals the final room, break out of the loop
		}
	}

	//Congratulatory message for winning the game:
	printf("\nYOU HAVE FOUND THE END ROOM. CONGRATULATIONS!\n");
	printf("YOU TOOK %d STEPS. YOUR PATH TO VICTORY WAS:\n", stepCount);
	int n;
	for(n = 0; n < stepCount; n++) {
		printf("%s\n", playerPath[n]);
	}
	closedir(dr);
	free(dirPath);
	pthread_mutex_destroy(&myMutex);
	return 0;
}
