#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define DIJKSTRA_INFINITY 0xFFFFFF
#define BUFFERSIZE 4096
#define BUFFERSCANF "%4095s"
#define ERRVAL -1

typedef unsigned int HeatValue;
typedef enum { false, true } bool;
typedef enum { NOWHERE, UP, DOWN, LEFT, RIGHT } Direction;

// Holds all of the values associated with a specific point.
typedef struct {
    HeatValue heat;         // heat value of this tile
    int prior_z;            // z-index of the prior tile along the path
    Direction past1;        // direction moved 1 movement ago
    Direction past2;        // direction moved 2 movements ago
    Direction past3;        // direciton moved 3 movements ago
    int distance;           // Dijkstra's-algorithm-calculated distance from z=0
    bool searchable;        // whether the tile can still be searched for in Dijkstra's algorithm
    Direction printed_as;   // directional arrow to be printed out
} Point;

// Holds all of the points of a 2D array, but in a 1D array shape.
// In this file, I use `z` to refer to a 1D index, and `(i,j)` to refer to a 2D index.
// Must be dynamically allocated using `allocate_heatmap`.
typedef struct {
    Point *points;
    int max_row;
    int max_col;
    int total_vertices;
} HeatMap;

// convert a `Direction` to an arrow-like character
char dir_to_arrow_char(const Direction dir);
// dynamically allocate space for a `HeatMap` struct, giving ownership to the caller
HeatMap *allocate_heatmap(const int width, const int height);
// deallocates the space used for a `HeatMap` struct
void free_heatmap(HeatMap *const heatmap);
// PRIVATE: reset all things in a `HeatMap` to pre-Dijskstra's algorithm 
void reset_paths(HeatMap *const heatmap);
// PRIVATE: return the index of the first tile that has `.searchable` == true
int first_in_searchables(const HeatMap *const heatmap);
// PRIVATE: return the index of the tile with `.searchable` == true and the smallest `.distance`.
int z_of_minimum_distance_in_searchables(const HeatMap* const heatmap);
// PRIVATE: determines whether moving from `(i,j)` in the direction `dir` would stay in-bounds, and whether it would obey the no-3-same-movements-in-a-row restriction.
bool safe_to_move(int i, int j, const HeatMap* const heatmap, const Direction dir);
// PRIVATE: from the point `(iMin,jMin)`, tries to link to the point in direction `dir`, if there is a shorter path to it
// SIDE-EFFECT: updates nearby points' `prior_z`, `.past#`, and `.distance`
bool try_to_branch(const int iMin, const int jMin, HeatMap *const heatmap, const Direction dir);
// run Dijkstra's algorithm
// RETURN: length of the shortest path
// SIDE-EFFECT: updates all points' `prior_z`, `.past#`, `.distance`, and `.searchable` 
int dijkstras_algorithm_calc(HeatMap *const heatmap);
// double-check the calculation for the shortest path, and mark the path with arrows
// RETURN: length of the shortest path (calculated differently)
// SIDE-EFFECT: updates all path points' `.printed_as`
int dijkstras_algorithm_result(HeatMap *const heatmap);
// open the file `filename` and check to see how many rows & columns it has
// UPDATE BY POINTER: `rows` and `cols` are set to the counted rows & columns
void count_height_width(const char* const filename, int *rows, int *cols);
// fills the `HeatMap` with the data from the file `filename`
int fill_heatmap(HeatMap *const heatmap, const char* const filename);
// prints the `HeatMap`
// if any tile has `.printed_as` set as something other than `NOWHERE`, it will be printed as an arrow
int print_heatmap(const HeatMap *const heatmap);

char dir_to_arrow_char(const Direction dir) {
    //return ' ';
    switch (dir) {
        case UP:
            return '^';
        case DOWN:
            return 'v';
        case LEFT:
            return '<';
        case RIGHT:
            return '>';
        default:
            return '*';
    }
}

HeatMap *allocate_heatmap(const int width, const int height) {
    // allocate new object
    HeatMap *new_heatmap        = calloc(1, sizeof(HeatMap));
    new_heatmap->max_col        = width;
    new_heatmap->max_row        = height;
    new_heatmap->total_vertices = width * height;
    new_heatmap->points         = calloc(new_heatmap->total_vertices, sizeof(Point));
    return new_heatmap;
}

void free_heatmap(HeatMap *const heatmap) {
    free(heatmap->points);
    free(heatmap);
    return;
}

void reset_paths(HeatMap *const heatmap) {
    const int total_vertices = heatmap->total_vertices;
    for (int z=0; z < total_vertices; z++) {
        heatmap->points[z].searchable   = true;
        heatmap->points[z].distance     = DIJKSTRA_INFINITY;
        heatmap->points[z].prior_z      = ERRVAL;
        heatmap->points[z].past1        = NOWHERE;
        heatmap->points[z].past2        = NOWHERE;
        heatmap->points[z].past3        = NOWHERE;
        heatmap->points[z].printed_as   = NOWHERE;
    }
    // z=0 (top left) is the starting position, so it gets distance 0
    heatmap->points[0].distance = 0;
    return;
}

int first_in_searchables(const HeatMap *const heatmap) {
    for (int z=0; z < heatmap->total_vertices; z++) {
        if (heatmap->points[z].searchable) {
            return z;
        }
    }
    return ERRVAL;
}

int z_of_minimum_distance_in_searchables(const HeatMap* const heatmap) {
    // first, grab *a* searchable `z`
    int zMin = 0;
    while (!heatmap->points[zMin].searchable) {
        zMin++;
    }
    // if we reached the end of the list, then return an error
    if (zMin == heatmap->total_vertices) {
        return ERRVAL;
    }
    // otherwise, using the found `z` as a temporary comparison, find the `z` with the minimum distance
    int zMinDist = heatmap->points[zMin].distance;
    printf("initial zMin=%d, with distance=%d\n", zMin, zMinDist);
    for (int z=zMin+1; z < heatmap->total_vertices; z++) {
        int zDist = heatmap->points[z].distance;
        if (zDist == zMinDist) printf("--equal-distance point found at z=%d\n", z);
        if (heatmap->points[z].searchable && zDist < zMinDist) {
            zMin     = z;
            zMinDist = zDist;
            printf("  better zMin=%d, with distance=%d\n", zMin, zMinDist);
        }
    }
    return zMin;
}

bool safe_to_move(int i, int j, const HeatMap* const heatmap, const Direction dir) {
    const int z = i * heatmap->max_col + j;
    int i2      = i;
    int j2      = j;
    // this function shouldn't be called with dir=NOWHERE
    if (dir == NOWHERE) {
        fprintf(stderr, "warning: `safe_to_move` called with dir=NOWHERE\n");
        return false;
    }
    // if it would be the 3rd in the same direction, not safe
    if (dir == heatmap->points[z].past1 && dir == heatmap->points[z].past2 && dir == heatmap->points[z].past3) {
        return false;
    }
    // if it would reverse direction, not safe
    if ((dir == UP    && heatmap->points[z].past1 == DOWN) || (dir == DOWN  && heatmap->points[z].past1 == UP)  ||
        (dir == RIGHT && heatmap->points[z].past1 == LEFT) || (dir == LEFT  && heatmap->points[z].past1 == RIGHT)) {
        return false;
    }
    // if the current location is out-of-bounds, error
    if (i < 0 || i >= heatmap->max_row || j < 0 || j >= heatmap->max_col) {
        fprintf(stderr, "warning: `safe_to_move` called with out-of-bounds position\n");
        return false;
    }
    // if the new location would be out-of-bounds, not safe
    switch (dir) {
        case UP:
            i2--;
            break;
        case DOWN:
            i2++;
            break;
        case LEFT:
            j2--;
            break;
        case RIGHT:
            j2++;
            break;
        default:
            fprintf(stderr, "warning: `safe_to_move` called with dir=NOWHERE\n");
            return false;
    }
    if (i2 < 0 || i2 >= heatmap->max_row || j2 < 0 || j2 >= heatmap->max_col) {
        return false;
    }

    // otherwise, it's safe
    return true;
}


bool try_to_branch(const int iMin, const int jMin, HeatMap *const heatmap, const Direction dir) {
    const int width = heatmap->max_col;
    const int zMin  = iMin * width + jMin;

    // calculate the index for the neighbor we're investigating
    int zNeighbor;
    switch (dir) {
        case UP:
            zNeighbor = (iMin-1)*width + (jMin  );
            break;
        case DOWN:
            zNeighbor = (iMin+1)*width + (jMin  );
            break;
        case LEFT:
            zNeighbor = (iMin  )*width + (jMin-1);
            break;
        case RIGHT:
            zNeighbor = (iMin  )*width + (jMin+1);
            break;
        default:
            fprintf(stderr, "function `try_to_branch` received direction NOWHERE\n");
            return false;
    }

    // if we can move along the path from `z` to `v`, and `v` hasn't been searched before
    if (safe_to_move(iMin, jMin, heatmap, dir) && heatmap->points[zNeighbor].searchable) {
        // calculate length of alternate path of going through `z` and then to `v`
        int alternate_path = heatmap->points[zMin].distance + heatmap->points[zNeighbor].heat;
        // if that length is less than the current distance to `v`, update the path
        if (alternate_path < heatmap->points[zNeighbor].distance) {
            heatmap->points[zNeighbor].distance = alternate_path;
            heatmap->points[zNeighbor].prior_z  = zMin;
            heatmap->points[zNeighbor].past3    = heatmap->points[zMin].past2;
            heatmap->points[zNeighbor].past2    = heatmap->points[zMin].past1;
            heatmap->points[zNeighbor].past1    = dir;
            return true;
        }
    }

    return false;
}

int dijkstras_algorithm_calc(HeatMap *const heatmap) {
    reset_paths(heatmap);
    const int zGoal = heatmap->total_vertices - 1;

    // `z` = vertex in searchables with minimum distance from source
    int zMin = z_of_minimum_distance_in_searchables(heatmap);

    // and if `z` is the target, we can stop
    while (zMin != zGoal) {
        // otherwise, first remove `z` from future searchables...
        heatmap->points[zMin].searchable = false;
        
        // Then, for each neighbor `v` of `z`:
        //      Calculate length of alternate path of going through `z` and then to `v`.
        //      If that length is less than the current distance to `v`, update the distance to `v` to the new, shorter one.
        const int iMin = zMin / heatmap->max_col;
        const int jMin = zMin % heatmap->max_col;
        try_to_branch(iMin, jMin, heatmap, UP);
        try_to_branch(iMin, jMin, heatmap, DOWN);
        try_to_branch(iMin, jMin, heatmap, LEFT);
        try_to_branch(iMin, jMin, heatmap, RIGHT);
        zMin = z_of_minimum_distance_in_searchables(heatmap);
    }

    return heatmap->points[zGoal].distance;
}


int dijkstras_algorithm_result(HeatMap *const heatmap) {
    const int total_vertices = heatmap->total_vertices;
    int zCurr = total_vertices - 1;
    int zPrev = heatmap->points[zCurr].prior_z;
    int cumulative_cost = 0;

    while (zCurr != 0) {
        cumulative_cost += heatmap->points[zCurr].heat;
        heatmap->points[zCurr].printed_as = heatmap->points[zCurr].past1;
        zCurr = zPrev;
        zPrev = heatmap->points[zCurr].prior_z;
    }

    // we don't count the cost at z=0
    // cumulative_cost += heatmap->points[zCurr].heat;
    return cumulative_cost;
}

void count_height_width(const char* const filename, int *rows, int *cols) {
    FILE* infile = fopen(filename, "r");
    char buffer[BUFFERSIZE] = "";
    *rows = 0;
    *cols = 0;

    // count columns and first row
    if (fscanf(infile, BUFFERSCANF, buffer) > 0) {
        *cols = strnlen(buffer, BUFFERSIZE);
        *rows += 1;
    }
    // count remaining rows
    while (fscanf(infile, BUFFERSCANF, buffer) > 0) {
        *rows += 1;
    }

    fclose(infile);
    return;
}

int fill_heatmap(HeatMap *const heatmap, const char* const filename) {
    if (!heatmap || !heatmap->points) {
        fprintf(stderr, "error in `fill_heatmap`: null pointer for heatmap or its points array");
        return ERRVAL;
    }

    FILE* infile = fopen(filename, "r");
    const int total_vertices = heatmap->total_vertices;

    int z = 0;
    while (z < total_vertices) {
        char c = fgetc(infile);
        if (c >= '0' && c <= '9') {
            heatmap->points[z].heat = c - '0';
            z++;
        }
        else {
            continue;
        }
    }

    fclose(infile);
    return 0;
}

int print_heatmap(const HeatMap *const heatmap) {
    if (!heatmap || !heatmap->points) {
        fprintf(stderr, "error in `fill_heatmap`: null pointer for heatmap or its points array");
        return ERRVAL;
    }

    for (int i=0; i < heatmap->max_row; i++) {
        for (int j=0; j < heatmap->max_col; j++) {
            int z = i*heatmap->max_col+j;
            if (heatmap->points[z].printed_as == NOWHERE) {
                printf("%c", (char)(heatmap->points[z].heat) + '0');
            } else {
                printf("%c", dir_to_arrow_char(heatmap->points[z].printed_as));
            }
        }
        printf("\n");
    }
    return 0;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: $ ./a.out <filename>\n");
        return 1;
    }

    FILE* infile = fopen(argv[1], "r");
    if (!infile) {
        fprintf(stderr, "File \"%s\" failed to open!\n", argv[1]);
        return 1;
    }
    
    // calculate how big the map should be
    int iMAX, jMAX;
    count_height_width(argv[1], &iMAX, &jMAX);
    
    // create the heatmap
    HeatMap *map = allocate_heatmap(jMAX, iMAX);
    fill_heatmap(map, argv[1]);
    printf("The Heat Map before running the thing:\n");
    print_heatmap(map);

    // run Dijkstra's Algorithm
    dijkstras_algorithm_calc(map);
    int part1answer = dijkstras_algorithm_result(map);
    printf("Answer for Part 1 is %d or %d\n", part1answer, map->points[map->total_vertices-1].distance);
    print_heatmap(map);

    free_heatmap(map);
    return 0;
}