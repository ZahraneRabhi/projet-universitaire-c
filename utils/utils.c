#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include "utils.h" 
#include <sys/stat.h>

/*
BUF_SIZE is basically accumulating data up to 4096 bytes before 
operating any kind of I/O operation, think of it as an illegal
football streaming where buffering is basically loading a certain 
amount of data before streaming it in real time, adding a short delay
but roughly making the watch experience better
*/

#define BUF_SIZE 4096
#define RESET_STYLE "\033[0m"
#define PURPLE "\033[95m"
#define GREEN "\033[92m"

// Fancy header for the project output console
void header() {
    printf("%sZahrane@Zahry %sGit ~\n%s",GREEN, PURPLE, RESET_STYLE);
}

// Just some code formatting
void footer() {
    printf("\n\n");
    getchar();
}

//////////////////////////////
// Memory managment methods //
//////////////////////////////

int hashFile(char* source, char* dest) {
    FILE *sourceFile, *destFile;
    char command[1024];
    
    // Create a temporary file name
    char template[] = "/tmp/hashXXXXXX";
    char tmpFileName[strlen(template) + 1];
    strcpy(tmpFileName, template);
    int fd = mkstemp(tmpFileName);
    if (fd == -1) {
        perror("Error creating temporary file");
        return -1;
    }
    close(fd);

    // Construct the commands pipeline to calculate hash using sha256sum
    snprintf(command, sizeof(command), "sha256sum %s > %s", source, tmpFileName);

    // Execute the command
    if (system(command) != 0) {
        perror("Error executing sha256sum command");
        return -1;
    }

    // Open the temporary file containing the hash
    sourceFile = fopen(tmpFileName, "r");
    if (!sourceFile) {
        perror("Error opening temporary file");
        return -1;
    }

    // Open destination file for writing
    destFile = fopen(dest, "w");
    if (!destFile) {
        perror("Error opening destination file");
        fclose(sourceFile);
        return -1;
    }

    // Copy the hash from temporary file to destination file
    char buffer[BUF_SIZE];
    size_t bytes;
    while ((bytes = fread(buffer, 1, BUF_SIZE, sourceFile)) > 0) {
        fwrite(buffer, 1, bytes, destFile);
    }

    // Close files
    fclose(sourceFile);
    fclose(destFile);

    // Clean up: remove the temporary file
    remove(tmpFileName);

    return 0;
}

char* sha256file(char* file) {
    // Create a temporary file name
    char template[] = "/tmp/hashXXXXXX";
    char tmpFileName[strlen(template) + 1];
    strcpy(tmpFileName, template);
    int fd = mkstemp(tmpFileName);
    if (fd == -1) {
        perror("Error creating temporary file");
        return NULL;
    }
    close(fd);

    // Construct the command to calculate hash using sha256sum
    char command[1024];
    snprintf(command, sizeof(command), "sha256sum %s > %s", file, tmpFileName);

    // Execute the command
    if (system(command) != 0) {
        perror("Error executing sha256sum command");
        return NULL;
    }

    // Open the temporary file containing the hash
    FILE *tmpFile = fopen(tmpFileName, "r");
    if (!tmpFile) {
        perror("Error opening temporary file");
        return NULL;
    }

    // Read the hash from the temporary file
    char hash[65];
    if (fscanf(tmpFile, "%64s", hash) != 1) {
        perror("Error reading hash from temporary file");
        fclose(tmpFile);
        return NULL;
    }

    // Close and remove the temporary file
    fclose(tmpFile);
    remove(tmpFileName);

    // Allocate memory for the hash string and copy the hash into it
    char *hashString = malloc(strlen(hash) + 1);
    if (!hashString) {
        perror("Error allocating memory for hash string");
        return NULL;
    }
    strcpy(hashString, hash);

    return hashString;
}

List* initList() {
    List* newList = (List*)malloc(sizeof(List));
    *newList = NULL;
    return newList;
}

Cell* buildCell(char* ch) {
    Cell* newCell = (Cell*)malloc(sizeof(Cell));
    newCell->data = strdup(ch);
    newCell->next = NULL;
    return newCell;
}

void insertFirst(List* L, Cell* C) {
    if (*L == NULL) {
        *L = C;
    } else {
        C->next = *L;
        *L = C;
    }
}

char* ctos(Cell* c) {
    return c->data;
}

char* ltos(List* L) {
    if (*L == NULL) return "";
    
    int totalLength = 0;
    Cell* current = *L;
    while (current != NULL) {
        totalLength += strlen(current->data) + 1; // +1 for the delimiter
        current = current->next;
    }

    char* result = (char*)malloc(totalLength);
    result[0] = '\0'; // Initialize the result string

    current = *L;
    while (current != NULL) {
        strcat(result, current->data);
        strcat(result, "|");
        current = current->next;
    }
    result[totalLength - 1] = '\0'; // Remove the last delimiter
    return result;
}

Cell* listGet(List* L, int i) {
    Cell* current = *L;
    int count = 0;
    while (current != NULL) {
        if (count == i) {
            return current;
        }
        count++;
        current = current->next;
    }
    return NULL; // If i is out of bounds
}

Cell* searchList(List* L, char* str) {
    Cell* current = *L;
    while (current != NULL) {
        if (strcmp(current->data, str) == 0) {
            return current;
        }
        current = current->next;
    }
    return NULL; // If the string is not found
}

List* stol(char* s) {
    List* newList = initList();
    char* token = strtok(s, "|");
    while (token != NULL) {
        Cell* newCell = buildCell(token);
        insertFirst(newList, newCell);
        token = strtok(NULL, "|");
    }
    return newList;
}

void ltof(List* L, char* path) {
    FILE* file = fopen(path, "w");
    if (file == NULL) {
        fprintf(stderr, "Unable to open file %s for writing.\n", path);
        exit(EXIT_FAILURE);
    }

    Cell* current = *L;
    while (current != NULL) {
        fprintf(file, "%s|", current->data);
        current = current->next;
    }
    fclose(file);
}

List* ftol(char* path) {
    FILE* file = fopen(path, "r");
    if (file == NULL) {
        fprintf(stderr, "Unable to open file %s for reading.\n", path);
        exit(EXIT_FAILURE);
    }

    List* newList = initList();
    char buffer[100]; // Adjust buffer size as needed
    while (fgets(buffer, sizeof(buffer), file) != NULL) {
        char* token = strtok(buffer, "|");
        while (token != NULL) {
            Cell* newCell = buildCell(token);
            insertFirst(newList, newCell);
            token = strtok(NULL, "|");
        }
    }
    fclose(file);
    return newList;
}

/////////////////////
// Files management//
/////////////////////

List* listDir(char* root_dir) {
    List* fileList = initList();

    DIR * dp = opendir(root_dir);
    struct dirent *ep;
    if (dp != NULL) {
        while ((ep = readdir (dp)) != NULL)
        {
            insertFirst(fileList, buildCell(ep->d_name));
        }
    }
    return fileList;
}

int fileExists(char* file) {
    List* fileList = listDir("."); // List of files in the current directory
    Cell* fileCell = searchList(fileList, file);
    if (fileCell != NULL)
        return 1; 
    else
        return 0; 
}

void cp(char *to, char *from) {
    FILE *sourceFile = fopen(from, "r");
    FILE *destFile = fopen(to, "w");
    if (sourceFile == NULL || destFile == NULL) {
        fprintf(stderr, "Error opening files\n");
        return;
    }
    char buffer[BUF_SIZE];
    while (fgets(buffer, BUF_SIZE, sourceFile) != NULL) {
        fputs(buffer, destFile);
    }
    fclose(sourceFile);
    fclose(destFile);
}


char* hashToPath(char* hash) {
    char* path = (char*)malloc(strlen(hash) + 2); // Allocate memory for path
    if (path == NULL) {
        fprintf(stderr, "Memory allocation error\n");
        exit(EXIT_FAILURE);
    }
    strncpy(path, hash, 2); // Copy first two characters of hash
    path[2] = '/'; // Insert '/' character
    strcpy(path + 3, hash + 2); // Copy the rest of the hash
    return path;
}

void blobFile(char* file) {
    // Calculate the hash of the file
    char* hash = sha256file(file);
    if (hash == NULL) {
        fprintf(stderr, "Error calculating hash for file %s\n", file);
        return;
    }

    // Determine the directory path for the snapshot based on the hash
    char snapshotDir[128]; // Adjust the size as needed
    snprintf(snapshotDir, sizeof(snapshotDir), "./snapshots/%.2s/", hash);

    // Create the directory using the mkdir command
    char mkdirCommand[256]; // Adjust the size as needed
    snprintf(mkdirCommand, sizeof(mkdirCommand), "mkdir -p %s", snapshotDir);
    if (system(mkdirCommand) == -1) {
        perror("Error creating snapshot directory");
        free(hash);
        return;
    }

    // Determine the snapshot file path
    char snapshotFile[256]; // Adjust the size as needed
    snprintf(snapshotFile, sizeof(snapshotFile), "%s%s", snapshotDir, file);

    // Copy the file to the snapshot location
    cp(snapshotFile, file);

    printf("Snapshot created for file %s\n", file);

    // Free dynamically allocated memory
    free(hash);
}


//////////////////////////////////////////////
// Working Directory tree managment methods //
//////////////////////////////////////////////

int getChmod(const char *path){
    struct stat ret;
    if (stat(path, &ret) == -1) {
    return -1;
    }
return
    (ret.st_mode & S_IRUSR)|(ret.st_mode & S_IWUSR)|(ret.st_mode & S_IXUSR)|/*owner*/
    (ret.st_mode & S_IRGRP)|(ret.st_mode & S_IWGRP)|(ret.st_mode & S_IXGRP)|/*group*/
    (ret.st_mode & S_IROTH)|(ret.st_mode & S_IWOTH)|(ret.st_mode & S_IXOTH);/*other*/
}