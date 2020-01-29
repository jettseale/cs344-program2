#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <limits.h>

int main() {
	
	int dirPid = getpid(); //Generate pid to append to directory name
	char dir[NAME_MAX + 1];
	char *dirStart = "sealee.rooms.";

	snprintf(dir, NAME_MAX + 1, "%s%d", dirStart, dirPid); //Append directory pid to directory name

	struct stat st = {0}; //Make a struct to hold the directory stats

	if (stat(dir, &st) == -1) { //If the directory does not already exist
		(mkdir(dir, 0755)); //Make the directory
		printf("Directory created\n");

		FILE *file;
		strncat(dir, "/", NAME_MAX + 1); //Append slash to end of directory name
		//TODO: Append pid to filename
		strncat(dir, "filename.txt", NAME_MAX + 1); //Finish fullpath to file by appending filename
		printf(dir);
		printf("\n");
		file = fopen(dir, "w"); //Make the file
		fclose(file);

	} else {
		perror("Error making directory\n");
	}
	return 0;
}
