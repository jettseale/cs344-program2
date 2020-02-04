#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <dirent.h>

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

	char* dir = findNewestDir();	
	printf("Newest dir: %s", dir);
	free(dir);

	return 0;
}
