#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define BUFFERSIZE 150
#define CYCLECOUNT 50000
#define CYCLEPRINT 1000

typedef enum {
    false=0, true=1
} bool;

typedef enum {
    INVALID=0, CUBIC=1, ROUND=2, EMPTY=3
} TileEnum;

typedef enum {
    NORTH=10, SOUTH=11, WEST=12, EAST=13
} DirectionEnum;

//============================================

typedef struct {
    TileEnum* array;
    int rowCt;
    int colCt;
} Platform;

//============================================

// conversion for TileEnum type
char tile2char(const TileEnum tile);
TileEnum char2tile(const char ch);

// allocation and deallocation for Platform type
Platform* p_read_alloc(FILE* infile);
void p_free(Platform* platPtr);

// functions for calculating features of a Platform
void p_print(       const Platform* const platPtr);
int  p_linearCoord( const Platform* const platPtr, const int i, const int j);
bool p_isValidCord( const Platform* const platPtr, const int i, const int j);
TileEnum p_get(     const Platform* const platPtr, const int i, const int j);
bool p_has(         const Platform* const platPtr, const int i, const int j, const TileEnum val);
bool p_isOpen(      const Platform* const platPtr, const int i, const int j);
int  p_calcLoad(    const Platform* const platPtr);

// functions for altering a Platform
void p_set(         Platform* const platPtr, const int i, const int j, const TileEnum val);
bool p_slideOnce(   Platform* const platPtr, const DirectionEnum dir);
void p_slide(       Platform* const platPtr, const DirectionEnum dir);
void p_cycle(       Platform* const platPtr, const int numCycles);

//============================================

char tile2char(const TileEnum tile) {
    switch (tile) {
        case CUBIC:
            return '#';
        case ROUND:
            return 'O';
        case EMPTY:
            return '.';
        default:
            return '?';
    }
}

TileEnum char2tile(const char ch) {
    switch (ch) {
        case '#':
            return CUBIC;
        case 'O':
            return ROUND;
        case '.':
            return EMPTY;
        default:
            return INVALID;
    }
}

Platform* p_read_alloc(FILE* infile) {
    char smallbuffer[BUFFERSIZE];
    int countRows, countCols;

    // count number of rows
    countRows = 0;
    rewind(infile);
    while (fgets(smallbuffer, BUFFERSIZE, infile)) {
        countRows++;
    }

    // count number of columns
    countCols = 0;
    rewind(infile);
    fgets(smallbuffer, BUFFERSIZE, infile);
    for (int c=0; c < (int)strnlen(smallbuffer, BUFFERSIZE); c++) {
        if (smallbuffer[c] == '\n' || smallbuffer[c] == '\r') {
            smallbuffer[c] = '\0';
        }
    }
    countCols = strnlen(smallbuffer, BUFFERSIZE);

    // allocate memory needed
    Platform* newPlat = malloc(sizeof(Platform));
    newPlat->array    = malloc(countRows * countCols * sizeof(TileEnum));
    newPlat->rowCt    = countRows;
    newPlat->colCt    = countCols;

    // actually read from file
    rewind(infile);
    countRows = 0;
    while (fgets(smallbuffer, BUFFERSIZE, infile)) {
        for (int c=0; c < (int)strnlen(smallbuffer, BUFFERSIZE); c++) {
            if (smallbuffer[c] == '\n' || smallbuffer[c] == '\r') {
                smallbuffer[c] = '\0';
            }
        }
        size_t linelen = strnlen(smallbuffer, BUFFERSIZE);
        //printf("The buffer is [%s]\n", smallbuffer);
        //printf("On line %d, the line length is %d.\n", countRows, linelen);
        for (size_t j=0; j < linelen; j++) {
            if (smallbuffer[j] != '\n' && smallbuffer[j] != '\r') {
                //printf("Writing into (%d,%d)...\n", countRows, j);
                p_set(newPlat, countRows, j, char2tile(smallbuffer[j]));
            }
        }
        countRows++;
    }

    return newPlat;
}

void p_free(Platform* platPtr) {
    free(platPtr->array);
    free(platPtr);
    return;
}

// Converts user's 2D coordinates `(i,j)` into the proper 1D coordinate for the underlying `platPtr->array`.
int p_linearCoord(const Platform* const platPtr, const int i, const int j) {
    return (i * (platPtr->colCt)) + j;
}

// Grabs the value of `platPtr->array` corresponding to the 2D coordinates `(i,j)`.
TileEnum p_get(const Platform* const platPtr, const int i, const int j) {
    return (platPtr->array)[p_linearCoord(platPtr, i, j)];
}

// Sets the value of `platPtr->array` corresponding to the 2D coordinates `(i,j)` to the given `val`.
void p_set(Platform* const platPtr, const int i, const int j, const TileEnum val) {
    (platPtr->array)[p_linearCoord(platPtr, i, j)] = val;
    return;
}

// Determines whether the given 2D coordinates `(i,j)` are valid for the `platPtr->array`.
bool p_isValidCord(const Platform* const platPtr, const int i, const int j) {
    return (i >= 0) && (j >= 0) && (i < platPtr->rowCt) && (j < platPtr->colCt);
}

// Determines whether the `platPtr->array` has the given `val` at the location corresponding to the 2D coordinates `(i,j)`.
bool p_has(const Platform* const platPtr, const int i, const int j, const TileEnum val) {
    return p_isValidCord(platPtr, i, j) && (p_get(platPtr, i, j) == val);
}

// Determines whether the `platPtr->array` has the `EMPTY` at the location corresponding to the 2D coordinates `(i,j)`.
bool p_isOpen(const Platform* const platPtr, const int i, const int j) {
    return p_isValidCord(platPtr, i, j) && (p_get(platPtr, i, j) == EMPTY);
}

// Prints out the contents of the Platform to `stdout`.
void p_print(const Platform* const platPtr) {
    printf("This platform has dimensions of %dx%d:\n", platPtr->rowCt, platPtr->colCt);
    for (int i=0; i < platPtr->rowCt; i++) {
        for (int j=0; j < platPtr->colCt; j++) {
            printf("%c", tile2char(p_get(platPtr, i, j)));
        }
        printf("\n");
    }
    return;
}

// Does one step of sliding of the platform in the given direction `dir`.
bool p_slideOnce(Platform* const platPtr, const DirectionEnum dir) {
    bool anyChange = false;
    int next_i;
    int next_j;

    switch (dir) {
        case NORTH: //done
            for (int i=0; i < platPtr->rowCt; i++) {
                for (int j=0; j < platPtr->rowCt; j++) {
                    next_i = i - 1;
                    next_j = j;
                    if (p_has(platPtr, i, j, ROUND) && p_isOpen(platPtr, next_i, next_j)) {
                        p_set(platPtr, next_i, next_j, ROUND);
                        p_set(platPtr, i, j, EMPTY);
                        anyChange = true;
                    }
                }
            }
            break;
        case SOUTH: //done
            for (int i=platPtr->rowCt - 1; i >= 0; i--) {
                for (int j=0; j < platPtr->rowCt; j++) {
                    next_i = i + 1;
                    next_j = j;
                    if (p_has(platPtr, i, j, ROUND) && p_isOpen(platPtr, next_i, next_j)) {
                        p_set(platPtr, next_i, next_j, ROUND);
                        p_set(platPtr, i, j, EMPTY);
                        anyChange = true;
                    }
                }
            }
            break;
        case WEST: //done
            for (int i=0; i < platPtr->rowCt; i++) {
                for (int j=0; j < platPtr->rowCt; j++) {
                    next_i = i;
                    next_j = j - 1;
                    if (p_has(platPtr, i, j, ROUND) && p_isOpen(platPtr, next_i, next_j)) {
                        p_set(platPtr, next_i, next_j, ROUND);
                        p_set(platPtr, i, j, EMPTY);
                        anyChange = true;
                    }
                }
            }
            break;
        case EAST:
            for (int i=0; i < platPtr->rowCt; i++) {
                for (int j=platPtr->rowCt - 1; j >= 0; j--) {
                    next_i = i;
                    next_j = j + 1;
                    if (p_has(platPtr, i, j, ROUND) && p_isOpen(platPtr, next_i, next_j)) {
                        p_set(platPtr, next_i, next_j, ROUND);
                        p_set(platPtr, i, j, EMPTY);
                        anyChange = true;
                    }
                }
            }
            break;
        default:
            fprintf(stderr, "p_slideOnce : Bad `dir` value of %d\n", (int)dir);
    }
    return anyChange;
}

// Does one complete slide of the given Platform in the given `dir`.
void p_slide(Platform* const platPtr, const DirectionEnum dir) {
    bool anyChange = true;
    while (anyChange) {
        anyChange = p_slideOnce(platPtr, dir);
    }
    return;
}

// Causes the Platform to go through `numCycles` number of cycles, where one "cycle" means sliding North, then West, then South, then East.
void p_cycle(Platform* const platPtr, const int numCycles) {
    int load0=0;
    //int load1=-1, load2=-2, load3=-3;
    for (int cyc=0; cyc < numCycles; cyc++) {
        // print our progress every once in a while
        if ((cyc % CYCLEPRINT) == 0) {
            load0 = p_calcLoad(platPtr);
            /*
            // if the load hasn't changed after CYCLEPRINT cycles, then we should be done
            if (load0 == load1 && load1 == load2 && load2 == load3) {
                break; // leave the for-loop
            } else {
                load3 = load2;
                load2 = load1;
                load1 = load0;
            }
            */
            printf("Calculated cycle #%d, current load is %d\n", cyc, load0);
        }
        p_slide(platPtr, NORTH);
        p_slide(platPtr, WEST);
        p_slide(platPtr, SOUTH);
        p_slide(platPtr, EAST);
    }
    return;
}

// Calculates the load on the platform.
int  p_calcLoad(const Platform* const platPtr) {
    int load = 0;
    for (int i=0; i < platPtr->rowCt; i++) {
        for (int j=0; j < platPtr->rowCt; j++) {
            if (p_has(platPtr, i, j, ROUND)) {
                load += (platPtr->rowCt - i);
            }
        }
    }
    return load;
}

//============================================

int main(const int argc, const char* argv[]) {
    // ensure correct usage
    if (argc != 2) {
        fprintf(stderr, "Usage: $ ./a.out <filename>\n");
        return 1;
    }

    // open file and check for error
    FILE* infile = fopen(argv[1], "r");
    if (!infile) {
        fprintf(stderr, "File \"%s\" failed to open!\n", argv[1]);
        return 1;
    }
    printf("File \"%s\" opened successfully!\n", argv[1]);

    // Part 1
    Platform* platform1 = p_read_alloc(infile);
    printf("\nPlatform 1 before sliding north:\n");
    p_print(platform1);
    p_slide(platform1, NORTH);
    printf("\nPlatform 1 after sliding north:\n");
    p_print(platform1);
    printf("\n--------------------------\nPlatform 1's current load is %d.\n", p_calcLoad(platform1));
    p_free(platform1);

    
    // Part 2
    Platform* platform2 = p_read_alloc(infile);
    printf("Platform 2 before doing %d cycles:\n", CYCLECOUNT);
    p_print(platform2);
    p_cycle(platform2, CYCLECOUNT);
    printf("Platform 2 after doing all the cycles:\n");
    p_print(platform2);
    printf("--------------------------\nPlatform 2's final load is %d.\n", p_calcLoad(platform2));
    p_free(platform2);
    

    fclose(infile);
    return 0;
}