#include <stdio.h>
#include <string.h>
#define MAX_HISTORY_SIZE 25
#define BUFFERSIZE 1024
#define MAX_STACK_SIZE 40
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

// PART 1 FUNCTIONS
void wipeStack(Stack* stack);
void wipeHistory(History* history);
History readHistory(const char* line, const int len);
bool isAllZeroes(const History *h);
History calcDifferences(const History *h);
int64 extrapolate(Stack* stack);
void printHistory(const History* hist);

int main() {
    FILE* infile = fopen(FILENAME, "r");
    if (!infile) {
        fprintf(stderr, "File \"%s\" failed to open!\n", FILENAME);
        return 1;
    }
    printf("File \"%s\"opened successfully!\n", FILENAME);

    char buffer[BUFFERSIZE] = "";
    int64 total = 0;
    Stack curr_stack;
    History curr_hist;

    int linecount = 1;

    // read file
    while (fgets(buffer, BUFFERSIZE, infile)) {
        // set up for storing new details
        wipeHistory(&curr_hist);
        wipeStack(&curr_stack);
        // interpret the line and store it in a `History` on the `curr_stack`
        curr_hist = readHistory(buffer, strnlen(buffer, BUFFERSIZE));
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
        total += extrapolate(&curr_stack);
        linecount++;
    }
    // no longer need file
    fclose(infile);

    //show answers
    printf("Answer to Part 1: %lld\n", total);

    return 0;
}

void wipeStack(Stack* stack) {
    stack->layercount = 0;
    for (int i=0; i < MAX_STACK_SIZE; i++) {
        wipeHistory(stack->histories + i);
    }
    return;
}
void wipeHistory(History* history) {
    for (int i=0; i < MAX_HISTORY_SIZE; i++) {
        history->array[i] = 0;
    }
    history->size = 0;
    return;
}

History readHistory(const char* line, const int len) {
    History newhist;
    int64* h = newhist.array;
    const char* TWENTYONE_INTEGERS = "%lld %lld %lld %lld %lld %lld %lld %lld %lld %lld %lld %lld %lld %lld %lld %lld %lld %lld %lld %lld %lld";
    sscanf(line, TWENTYONE_INTEGERS, h, h+1, h+2, h+3, h+4, h+5, h+6, h+7, h+8, h+9, h+10, h+11, h+12, h+13, h+14, h+15, h+16, h+17, h+18, h+19, h+20);
    newhist.size = INTS_PER_LINE;
    return newhist;
}

bool isAllZeroes(const History *h) {
    int64 len = h->size;
    for (int i=0; i < len; i++) {
        if (h->array[i] != 0) return false;
    }
    return true;
}

History calcDifferences(const History *h) {
    int64 len = h->size;
    History nextlayer;

    // each next layer is the forward difference of the previous layer
    for (int i=0; i < len-1; i++) {
        nextlayer.array[i] = h->array[i+1] - h->array[i];
    }
    // since it's in pairs, we get one less result
    nextlayer.size = len - 1;
    return nextlayer;
}

int64 extrapolate(Stack* stack) {
    for (int layer=stack->layercount - 1; layer >= 0; layer--) {
        // for ease of reading, get pointers to this layer and the layer below
        History * const this_layer  = &(stack->histories[layer]);
        History * const below_layer = &(stack->histories[layer + 1]);
        // and the position that we'll be adding a value at in `this_layer`
        const int new_position  = this_layer->size;
        const int prev_position = new_position - 1;

        printf("Checking layer %d:  ", layer);
        printHistory(this_layer);

        // for the bottom-most layer, always add a 0 (since it should just be a row of 0s)
        if (layer == stack->layercount - 1) {
            this_layer->array[new_position] = 0;
        }
        // otherwise, calculate based on the values on `this_layer` and `below_layer` in the previous position
        else {
            this_layer->array[new_position] = this_layer->array[prev_position] + below_layer->array[prev_position];
        }
        // the 0th (top) layer's new value is what we want to return
        if (layer == 0) {
            printf("Next value in this history is %lld\n-------------\n", this_layer->array[new_position]);
            return this_layer->array[new_position];
        }
    }
    // will never run
    return -1;
}

void printHistory(const History* hist) {
    for (int i=0; i < hist->size; i++) {
        printf("%lld\t", hist->array[i]);
    }
    printf("\n");
    return;
}
