#include <stdio.h>
#include <string.h>
#define BUFFERSIZE 1024
#define NUM_NUMBERS 9

typedef enum {false=0, true=1} bool;
const char* const FILENAME = "day1_input.txt";

bool isDigit(char c);
int findSubstring(const char* look_str, int look_len, const char* find_str, int find_len, bool leftmost);

int findNumericChar(const char* input_str, int len, bool leftmost);
int findNumericWord(const char* input_str, int len, int* found_number, bool leftmost);
int findNumericEither(const char* input_str, int len, bool leftmost);

int main(void) {
    char buffer[BUFFERSIZE] = "";   // buffer to hold string from file
    int total = 0;                  // cumulative total of all "calibration values"

    // open file
    FILE* infile = fopen(FILENAME, "r");
    if (!infile) {
        fprintf(stderr, "File \"%s\" failed to open!\n", FILENAME);
        return 1;
    }

    // read file
    while (fgets(buffer, BUFFERSIZE, infile)) {
        // skip lines of comments that I've added
        if (buffer[0] == '#')
            continue;

        int tens = findNumericEither(buffer, strnlen(buffer, BUFFERSIZE), true);
        int ones = findNumericEither(buffer, strnlen(buffer, BUFFERSIZE), false);
        // sum each "calibration value"
        total += 10*tens + ones;
    }
    // no longer need file
    fclose(infile);

    // final result
    printf("Total Sum: %i\n", total);
    return 0;
}

/* ----------------------------
    Returns TRUE if the given character is an ASCII digit.
---------------------------- */
bool isDigit(char c) {
    return (c >= '0' && c <= '9');
}

/* ----------------------------
    Returns the address of the first numeric character in the string `input_str` (of length `len`).
    Returns -1 if there is no numeric character.
---------------------------- */

int findNumericChar(const char* input_str, int len, bool leftmost) {
    int return_index = -1;
    
    if (leftmost) {
        for (int i=0; i<len; i++) {
            if (isDigit(input_str[i]))
                return i;
        }
    } else {
        for (int i=len-1; i>=0; i--) {
            if (isDigit(input_str[i]))
                return i;
        }
    }

    return -1;
}

/* ----------------------------
    If `lookstr` contains `find_str` as a substring, returns the index of the first character of the LEFTMOST/RIGHTMOST (boolean-controlled) instance of the substring in `lookstr`.
    Returns -1 if `find_str` is not a substring of `look_str`, or if `find_str` is empty.
---------------------------- */

int findSubstring(const char* look_str, int look_len, const char* find_str, int find_len, bool leftmost) {
    bool is_match = false;
    // can't search within empty strings
    if (look_len < 1) {
        fprintf(stderr, "Cannot look within a string a string of size %i", look_len);
        return -1;
    }

    // can't search for the empty string
    if (find_len < 1) {
        fprintf(stderr, "Cannot search for a string of size %i", find_len);
    }

    // no hope of finding a substring that's longer
    if (find_len > look_len) {
        return -1;
    }

    int most_recent_result = -1;

    // FROM LEFT
    for (int i=0; i<look_len; i++) {
        // if one matching character is found
        if (look_str[i] == find_str[0]) {
            // tentatively mark true match
            is_match = true;
            // check the subsequence characters
            for (int j=1; j<find_len; j++) {
                // if this would cause us to go beyond the look_str's end, there's no hope for a match
                if (i+j >= look_len) {
                    is_match = false;
                    break;
                }
                // otherwise, we need to match ALL subsequent characters in find_str
                is_match = is_match && (look_str[i+j] == find_str[j]);
            }
            // if we've reached here with 'is_match', then record the index of the first character that matched
            if (is_match) {
                    most_recent_result = i;
                if (leftmost) {
                    // if looking for leftmost, stop looking and go to return statement
                    break;
                } else {
                    // if looking for rightmost, keep looking for more
                    is_match = false;
                }
            }
        }
    }

    // if we've reached here, then we never found any positive matches
    return most_recent_result;

}

/* ----------------------------
    If `input_str` contains a numeric word ("one" thru "nine"), uses findSubstring() to return the INDEX of the numeric word that occurs LEFTMOST/RIGHTMOST (boolean-controlled) in `input_str`. If there is no such word, returns -1.
    Whenever a match is found, then `found_number` is updated by pointer to the VALUE of the found numeric word. If it does not, `found_number` is left unchanged.
    --> Potential speedup: keep a local variable and only update `found_number` at the end to avoid many memory writes. However, it can only happen up to 9 times, so I'm not concerned about performance here.
---------------------------- */

int findNumericWord(const char* input_str, int len, int* found_number, bool leftmost) {
    // for iterating over
    const struct {
        char* name;
        int length;
    } NUMBERS[NUM_NUMBERS] = {
        {"one",     3},
        {"two",     3},
        {"three",   5},
        {"four",    4},
        {"five",    4},
        {"six",     3},
        {"seven",   5},
        {"eight",   5},
        {"nine",    4},
    };

    int winning_index = leftmost ? len+1 : -1;  // index of leftMOST/rightMOST number word
    int current_test_index;                     // currently-found number word

    // for all numbers in the `number_names` list
    for (int i=0; i < NUM_NUMBERS; i++) {
        // look to see if that number word is in the string, and if so, grab its leftmost/rightmost position
        current_test_index = findSubstring(input_str, len, NUMBERS[i].name, NUMBERS[i].length, leftmost);

        if (current_test_index > -1 &&                                  // if an instance of the number word was found...
                (   leftmost && current_test_index < winning_index      // ...LEFT:  and it's further left than the current-best one
                || !leftmost && current_test_index > winning_index)) {  // ...RIGHT: and it's further right that the current-best one
            // then update the winning index and the number
            winning_index = current_test_index;
            *found_number = i+1;
        }
    }

    // (I could make this a nested ternary expression, but it would only serve to make this less readable)
    if (leftmost)   // LEFT: if one was found (since it's set to `len` by default otherwise), return it, otherwise -1
        return (winning_index < len ? winning_index : -1);
    else            // RIGHT: if one was found, return it, otherwise -1... but by design, it's set to -1 by default, so no test needed
        return winning_index;
}

/* ----------------------------
    Returns the VALUE of the LEFTMOST/RIGHTMOST (boolean-controlled) decimal character *or* numeric word that appears in `input_str`.
---------------------------- */
int findNumericEither(const char* input_str, int len, bool leftmost) {
    // check for numeric character
    int found_c_val = -1;
    int found_c_index = findNumericChar(input_str, len, leftmost);
    if (found_c_index != -1)
        found_c_val = input_str[found_c_index] - '0';
    
    // check for numeric word
    int found_w_val = -1;
    int found_w_index = findNumericWord(input_str, len, &found_w_val, leftmost);    // found_w_val updated by pointer

    // if only one type is found, then obviously the other one is the one to go with (we know the data MUST have one)
    if (found_c_index == -1) return found_w_val;
    if (found_w_index == -1) return found_c_val;

    // else, if both types are found, then choose the LEFTMOST/RIGHTMOST one
    if (leftmost)
        return (found_c_index < found_w_index ? found_c_val : found_w_val);
    else
        return (found_c_index > found_w_index ? found_c_val : found_w_val);
}
