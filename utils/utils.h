#ifndef UTILS_H
#define UTILS_H

#define MAX_NAME_LENGTH 100
#define MAX_WORKFILES 100

typedef struct cell {
    char* data;
    struct cell* next;
} Cell;

typedef Cell* List;

/*
These WorkFile & WorkTree types are defined to handle projects 
with multiple files and sub-files
*/

typedef struct {
    char* name;
    char* hash;
    int mode;
} WorkFile;

typedef struct {
    WorkFile* tab;
    int size;
    int n;
} WorkTree;


// Console UI methods
void header();
void footer();

// SHA256 Hashing methods
int hashFile(char* source, char* dest);
char* sha256file(char* file);

// Memory managment methods
List* initList();
Cell* buildCell(char* ch);
void insertFirst(List* L, Cell* C);
char* ctos(Cell* c);
char* ltos(List* L);
Cell* listGet(List* L, int i);
Cell* searchList(List* L, char* str);
List* stol(char* s);
void ltof(List* L, char* path);
List* ftol(char* path);

// Files management
List* listDir(char* root_dir);
int fileExists(char* file);
void cp(char *to, char *from);
char* hashToPath(char* hash);
void blobFile(char* file);

// Working Directory tree managment methods
int getChmod(const char *path);
void setMode(int mode, char* path);
void createSnapshot(WorkFile* file);
void restoreSnapshot(WorkFile* file);
void createTreeSnapshot(WorkTree* tree);
void restoreTreeSnapshot(WorkTree* tree);

WorkFile* createWorkFile(char* name);
char* wfts(WorkFile* wf);
WorkFile* stwf(char* ch);
WorkTree* initWorkTree();
int inWorkTree(WorkTree* wt, char* name);
int appendWorkTree(WorkTree* wt, char* name, char* hash, int mode);
char* wtts(WorkTree* wt);
WorkTree* stowt(char* s);
int wttf(WorkTree* wt, char* file);
WorkTree* ftwt(char* file);

#endif