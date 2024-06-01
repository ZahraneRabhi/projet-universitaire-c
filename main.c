#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include "utils/utils.h" 

#define ROOT "/home/zahrane-rabhi/projet-scv"

int main() {
    char* fileName = "test.txt"; // Name of the file to be snapshot

    // Check if the file exists
    if (!fileExists(fileName)) {
        fprintf(stderr, "The file %s does not exist.\n", fileName);
        return 1;
    }

    // Create an "instantané" of the file
    blobFile(fileName);

    printf("Instantané created for the file %s.\n", fileName);

    return 0;
}