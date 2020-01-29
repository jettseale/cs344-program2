#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <limits.h>

int main() {
	
	int pid = getpid(); //Generate pid to append to directory name
	char dir[NAME_MAX + 1];
	char *start = "sealee.rooms.";

	snprintf(dir, NAME_MAX + 1, "%s%d", start, pid); //Append pid to directory name
	//printf("%s\n", dir);

	struct stat st = {0}; //Make a struct to hold the directory stats

	if (stat(dir, &st) == -1) { //If the directory does not already exist
		if (mkdir(dir, 0755)) { //Make the directory
			printf(" Directory created\n");
			fopen("filename", "w");
		}
	} else {
		perror("Error making directory\n");
	}
	return 0;
}
