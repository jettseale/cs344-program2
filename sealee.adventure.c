#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <dirent.h>
#include <pthread.h>

//Struct room type:
struct room {
	int id;
	char* name;
	int numConnections;
	char* connectionNames[6];
	char* type;
};

/*****************************************************************************************************************
 * NAME: TODO
 * SYNOPSIS:
 * DESCRIPTION:
 * AUTHOR:
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
 * NAME: TODO
 * SYNOPSIS:
 * DESCRIPTION:
 * AUTHOR:
 * **************************************************************************************************************/
int main() {
	char* dirPath = findNewestDir();
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

	while ((de = readdir(dr)) != NULL) {
		if (strncmp(de->d_name, ".", 256) && strncmp(de->d_name, "..", 256)) {
			char fullPath[256];
			memset(fullPath, '\0', 256);
			sprintf(fullPath, "%s", dirPath);
			sprintf(fileName, "%s", de->d_name);
			strncat(fullPath, fileName, 256);
			file = fopen(fullPath, "r");
			if (file == NULL) { 
				perror("Error opening file");
				exit(1);
			}
			while (!feof(file)) {
				fgets(line, 256, file);
				if (!strncmp(line, "ROOM TYPE: START_ROOM\n", 256)) {
					strcpy(currentRoom, de->d_name);
					strcpy(currentFilePath, fullPath);
				} else if (!strncmp(line, "ROOM TYPE: END_ROOM\n", 256)) {
					strcpy(endRoom, de->d_name);
				}
			}
			fclose(file);
		}
	}

	char possibleConnections[6][256];
	FILE* currentFile;
	char playerPath[256][256];
	int stepCount = 0;

	while (true) { 
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
				strcpy(possibleConnections[inc], sentence + 14);
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
				printf("%s", possibleConnections[i]);
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
					invalidInput = false;
				}
			}
			
			if (invalidInput) {
				printf("\nHUH? I DON'T UNDERSTAND THAT ROOM. TRY AGAIN.\n");
			} else {
				strncpy(newRoomName, input, 8);
				strncpy(playerPath[stepCount], input, 8);
				stepCount++;
			}
		}

		memset(currentFilePath, '\0', 256);
		sprintf(currentFilePath, dirPath, 256);
		strncat(currentFilePath, newRoomName, 8);

		memset(currentRoom, '\0', 256);
		strncat(currentRoom, newRoomName, 8);
		
		if (!strncmp(endRoom, newRoomName, 8)) {
			break;
		}
	}
	printf("\nYOU HAVE FOUND THE END ROOM. CONGRATULATIONS!\n");
	printf("YOU TOOK %d STEPS. YOUR PATH TO VICTORY WAS:\n", stepCount);
	int n;
	for(n = 0; n < stepCount; n++) {
		printf("%s\n", playerPath[n]);
	}
	closedir(dr);
	free(dirPath);
	return 0;
}
