#include <stdio.h>
#include <string.h>
#define MAX_HISTORY_SIZE 25
#define BUFFERSIZE 1024
#define MAX_STACK_SIZE 40
// this line must be changed depending on the input file
#define INTS_PER_LINE 21

const char* const FILENAME = "day9_input.txt";

typedef long long int64;
typedef enum { false = 0, true = 1 } bool;

typedef struct {
    int64 array[MAX_HISTORY_SIZE];
    int64 size;
} History;

typedef struct {
    History histories[MAX_STACK_SIZE];
    int64 layercount;
} Stack;

// FUNCTION DECLARATIONS
void wipeHistory(History* history);
void wipeStack(Stack* stack);
bool isAllZeroes(const History *h);
History readHistory(const char* line);
History calcDifferences(const History *h);
void printHistory(const History* hist);
int64 extrapolateForward(const Stack* stack);
int64 extrapolateBack(const Stack* stack);

// ---------------------------------------------------
int main() {
    FILE* infile = fopen(FILENAME, "r");
    if (!infile) {
        fprintf(stderr, "File \"%s\" failed to open!\n", FILENAME);
        return 1;
    }
    printf("File \"%s\" opened successfully!\n", FILENAME);

    char buffer[BUFFERSIZE] = "";
    int64 total1=0, total2=0;
    Stack curr_stack;
    History curr_hist;

    // read file
    while (fgets(buffer, BUFFERSIZE, infile)) {
        // set up for storing new details
        wipeHistory(&curr_hist);
        wipeStack(&curr_stack);
        // interpret the line and store it in a `History` on the `curr_stack`
        curr_hist = readHistory(buffer);
        curr_stack.histories[0] = curr_hist;
        curr_stack.layercount++;
        int64 i = 1;
        // calculate all further layers of histories
        while (! isAllZeroes(&curr_hist)) {
            curr_hist = calcDifferences(&curr_hist);
            curr_stack.histories[i] = curr_hist;
            curr_stack.layercount++;
            i++;
        }
        total1 += extrapolateForward(&curr_stack);
        total2 += extrapolateBack(&curr_stack);
    }
    // no longer need file
    fclose(infile);

    printf("Answer to Part 1: %lld\n", total1);
    printf("Answer to Part 2: %lld\n", total2);

    return 0;
}

// ---------------------------------------------------

// memory erasure, for loop safety
void wipeHistory(History* history) {
    for (int i=0; i < MAX_HISTORY_SIZE; i++) {
        history->array[i] = 0;
    }
    history->size = 0;
    return;
}

// memory erasure, for loop safety
void wipeStack(Stack* stack) {
    stack->layercount = 0;
    for (int i=0; i < MAX_STACK_SIZE; i++) {
        wipeHistory(stack->histories + i);
    }
    return;
}

bool isAllZeroes(const History *h) {
    int64 len = h->size;
    for (int i=0; i < len; i++) {
        if (h->array[i] != 0) return false;
    }
    return true;
}

History readHistory(const char* line) {
    History newhist;
    int64* h = newhist.array;
    const char* TWENTYONE_INTEGERS = "%lld %lld %lld %lld %lld %lld %lld %lld %lld %lld %lld %lld %lld %lld %lld %lld %lld %lld %lld %lld %lld";
    sscanf(line, TWENTYONE_INTEGERS, h, h+1, h+2, h+3, h+4, h+5, h+6, h+7, h+8, h+9, h+10, h+11, h+12, h+13, h+14, h+15, h+16, h+17, h+18, h+19, h+20);
    newhist.size = INTS_PER_LINE;
    return newhist;
}

History calcDifferences(const History *h) {
    int64 len = h->size;
    History layerbelow;

    // each layer below is the forward difference of the layer above
    for (int i=0; i < len-1; i++) {
        layerbelow.array[i] = h->array[i+1] - h->array[i];
    }
    // since it's in pairs, we get one less result
    layerbelow.size = len - 1;
    return layerbelow;
}

void printHistory(const History* hist) {
    for (int i=0; i < hist->size; i++) {
        printf("%lld\t", hist->array[i]);
    }
    printf("\n");
    return;
}

/*
1   3   6  10  15  21 | 28
2   3   4   5   6 | 7
1   1   1   1 | 1
0   0   0 | 0

21 + 7 = 28         arr[0][6] = arr[0][5] + arr[1][5]
7  + 1 = 7          arr[1][5] = arr[1][4] + arr[2][4]
1  + 0 = 1          arr[2][4] = arr[2][3] + arr[3][3]
                    arr[3][3] = 0

PART 1:             arr[i][j] = arr[i][j-1] + arr[i+1][j-1]     */
int64 extrapolateForward(const Stack* stack) {
    for (int layer=stack->layercount - 1; layer >= 0; layer--) {
        const History *this_layer  = &(stack->histories[layer]);   // for ease of reading
        int64 new_next_val; // holds the imaginary this_layer->array[n] that we're calculating

        //printf("Checking layer %d:  ", layer);
        //printHistory(this_layer);

        // for the bottom-most layer, always add a 0 (since it should just be a row of 0s)
        if (layer == stack->layercount - 1) {
            new_next_val = 0;
        }
        // otherwise, calculate based on the values on `this_layer` and `below_layer`
        else {
            // this_layer->array[n] = this_layer->array[n-1] + below_layer->array[n-1]
            new_next_val = this_layer->array[this_layer->size - 1] + new_next_val;
        }
        // the 0th (top) layer's new value is what we want to return
        if (layer == 0) {
            //printf("Next value in this history is %lld\n-------------\n", new_next_val);
            return new_next_val;
        }
    }
    // will never run
    return -1;
}

/*
 5  | 10  13  16  21  30  45
 5  | 3   3   5   9  15
-2  | 0   2   4   6
 2  | 2   2   2
 0  | 0   0

2  - 0  =  2        arr[0][-1] = arr[0][0] - arr[1][-1]
0  - 2  = -2        arr[1][-1] = arr[1][0] - arr[2][-1]
3  - -2 =  5        arr[2][-1] = arr[2][0] - arr[3][-1]
10 - 5  =  5        arr[3][-1] = arr[3][0] - arr[4][-1]
                    arr[4][-1] = 0

PART2:              arr[i][-1] = arr[i][0] - arr[i+1][-1]       */
int64 extrapolateBack(const Stack* stack) {
    for (int layer=stack->layercount - 1; layer >= 0; layer--) {
        const History *this_layer  = &(stack->histories[layer]);   // for ease of reading
        int64 new_prev_val; // holds the imaginary this_layer->array[-1] that we're calculating

        //printf("Checking layer %d:  ", layer);
        //printHistory(this_layer);

        // for the bottom-most layer, always add a 0 (since it should just be a row of 0s)
        if (layer == stack->layercount - 1) {
            new_prev_val = 0;
        }
        else {
            // this_layer->array[-1] = this_layer->array[0] - below_layer->array[-1]
            new_prev_val = this_layer->array[0] - new_prev_val;
        }
        // the 0th (top) layer's new value is what we want to return
        if (layer == 0) {
            //printf("Next value in this history is %lld\n-------------\n", new_prev_val);
            return new_prev_val;
        }
    }
    // will never run
    return -1;
}
