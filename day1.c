#include <stdio.h>
#include <string.h>
#define BUFFERSIZE 1024

typedef enum {false=0, true=1} bool;
const char* const FILENAME = "day1_input.txt";

bool isDigit(char c);
int findFirstNumberChar(const char* str, int len);
int findLastNumberChar(const char* str, int len);
int findSubstringL(const char* lookstr, int looklen, const char* findstr, int findlen);
int findSubstringR(const char* lookstr, int looklen, const char* findstr, int findlen);
int findFirstNumberWord(const char* str, int len, int* firstnum);
int findLastNumberWord(const char* str, int len, int* lastnum);
int findFirstNumber(const char* str, int len);
int findLastNumber(const char* str, int len);

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

        int tens = findFirstNumber(buffer, strnlen(buffer, BUFFERSIZE));
        int ones = findLastNumber(buffer, strnlen(buffer, BUFFERSIZE));
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
    if (c >= '0' && c <= '9')
        return true;
    else
        return false;
}

/* ----------------------------
    Returns the address of the first numeric character in the string `str` (of length `len`).
    Returns -1 if there is no numeric character.
---------------------------- */
int findFirstNumberChar(const char* str, int len) {
    for (int i=0; i < len; i++) {
        if (isDigit(str[i]))
            return i;
    }
    return -1;
}

/* ----------------------------
    Returns the address of the last numeric character in the string `str` (of length `len`).
    Returns -1 if there is no numeric character.
---------------------------- */
int findLastNumberChar(const char* str, int len) {
    int lastindex = -1;
    for (int i=0; i < len; i++) {
        if (isDigit(str[i]))
            lastindex = i;
    }
    return lastindex;
}

/* ----------------------------
    If `lookstr` contains `findstr` as a substring, returns the index of the first character of the LAST instance of the substring in `lookstr`.
    Returns -1 if `findstr` is not a substring of `lookstr`, or if `findstr` is empty.
---------------------------- */
int findSubstringL(const char* lookstr, int looklen, const char* findstr, int findlen) {
    bool is_match = false;

    // can't search within empty strings
    if (looklen < 1) {
        fprintf(stderr, "Cannot look within a string a string of size %i", looklen);
        return -1;
    }

    // can't search for the empty string
    if (findlen < 1) {
        fprintf(stderr, "Cannot search for a string of size %i", findlen);
    }

    // no hope of finding a substring that's longer
    if (findlen > looklen) {
        return -1;
    }

    // loop through entire LookIn string
    for (int i=0; i<looklen; i++) {
        // if one matching character is found
        if (lookstr[i] == findstr[0]) {
            // tentatively mark true match
            is_match = true;
            // check the subsequence characters
            for (int j=1; j<findlen; j++) {
                // if this would cause us to go beyond the lookstr's end, there's no hope for a match
                if (i+j >= looklen) {
                    is_match = false;
                    break;
                }
                // otherwise, we need to match ALL subsequent characters in findstr
                is_match = is_match && (lookstr[i+j] == findstr[j]);
            }
            // if we've reached here with is_match true, then we did find a match: return the index of the first character that matched
            if (is_match)
                return i;
        }
    }
    // if we've reached here, then we never found any positive matches
    return -1;
}

/* ----------------------------
    If `lookstr` contains `findstr` as a substring, returns the index of the first character of the LAST instance of the substring in `lookstr`.
    Returns -1 if `findstr` is not a substring of `lookstr`, or if `findstr` is empty.
---------------------------- */
int findSubstringR(const char* lookstr, int looklen, const char* findstr, int findlen) {
    bool is_match = false;

    // can't search within empty strings
    if (looklen < 1) {
        fprintf(stderr, "Cannot look within a string a string of size %i", looklen);
        return -1;
    }

    // can't search for the empty string
    if (findlen < 1) {
        fprintf(stderr, "Cannot search for a string of size %i", findlen);
    }

    // no hope of finding a substring that's longer
    if (findlen > looklen) {
        return -1;
    }

    int last_result = -1;

    // loop through entire LookIn string
    for (int i=0; i<looklen; i++) {
        // if one matching character is found
        if (lookstr[i] == findstr[0]) {
            // tentatively mark true match
            is_match = true;
            // check the subsequence characters
            for (int j=1; j<findlen; j++) {
                // if this would cause us to go beyond the lookstr's end, there's no hope for a match
                if (i+j >= looklen) {
                    is_match = false;
                    break;
                }
                // otherwise, we need to match ALL subsequent characters in findstr
                is_match = is_match && (lookstr[i+j] == findstr[j]);
            }
            // if we've reached here with is_match true, then we did find a match: mark the index of the first character that matched
            if (is_match) {
                last_result = i;
                is_match = false;
            }
        }
    }
    return last_result;
}

/* ----------------------------
    If `str` contains a numeric word ("one" thru "nine"), uses findSubstringL() to return the INDEX of the numeric word that occurs earliest in `str`.
    If there is no such word, returns -1.
    If such a word does exist, then `firstnum` is updated by pointer to the VALUE of the found numeric word.
---------------------------- */
int findFirstNumberWord(const char* str, int len, int* firstnum) {
    int lowest_index = len+1;
    int current_test;

    current_test = findSubstringL(str, len, "one", 3);
    if (current_test > -1 && current_test < lowest_index) {
        lowest_index = current_test;
        *firstnum = 1;
    }
    current_test = findSubstringL(str, len, "two", 3);
    if (current_test > -1 && current_test < lowest_index) {
        lowest_index = current_test;
        *firstnum = 2;
    }
    current_test = findSubstringL(str, len, "three", 5);
    if (current_test > -1 && current_test < lowest_index) {
        lowest_index = current_test;
        *firstnum = 3;
    }
    current_test = findSubstringL(str, len, "four", 4);
    if (current_test > -1 && current_test < lowest_index) {
        lowest_index = current_test;
        *firstnum = 4;
    }
    current_test = findSubstringL(str, len, "five", 4);
    if (current_test > -1 && current_test < lowest_index) {
        lowest_index = current_test;
        *firstnum = 5;
    }
    current_test = findSubstringL(str, len, "six", 3);
    if (current_test > -1 && current_test < lowest_index) {
        lowest_index = current_test;
        *firstnum = 6;
    }
    current_test = findSubstringL(str, len, "seven", 5);
    if (current_test > -1 && current_test < lowest_index) {
        lowest_index = current_test;
        *firstnum = 7;
    }
    current_test = findSubstringL(str, len, "eight", 5);
    if (current_test > -1 && current_test < lowest_index) {
        lowest_index = current_test;
        *firstnum = 8;
    }
    current_test = findSubstringL(str, len, "nine", 4);
    if (current_test > -1 && current_test < lowest_index) {
        lowest_index = current_test;
        *firstnum = 9;
    }
    // current_test = findSubstringL(str, len, "zero", 4);
    // if (current_test > -1 && current_test < lowest_index) {
    //     lowest_index = current_test;
    //     *firstnum = 0;
    // }
    
    if (lowest_index > len) return -1;
    else return lowest_index;
}

/* ----------------------------
    If `str` contains a numeric word ("one" thru "nine"), uses findSubstringR() to return the INDEX of the numeric word that occurs latest in `str`.
    If there is no such word, returns -1.
    If such a word does exist, then `lastnum` is updated by pointer to the VALUE of the found numeric word.
---------------------------- */
int findLastNumberWord(const char* str, int len, int* lastnum) {
    int highest_index = -1;
    int current_test;

    current_test = findSubstringR(str, len, "one", 3);
    if (current_test > -1 && current_test > highest_index) {
        highest_index = current_test;
        *lastnum = 1;
    }
    current_test = findSubstringR(str, len, "two", 3);
    if (current_test > -1 && current_test > highest_index) {
        highest_index = current_test;
        *lastnum = 2;
    }
    current_test = findSubstringR(str, len, "three", 5);
    if (current_test > -1 && current_test > highest_index) {
        highest_index = current_test;
        *lastnum = 3;
    }
    current_test = findSubstringR(str, len, "four", 4);
    if (current_test > -1 && current_test > highest_index) {
        highest_index = current_test;
        *lastnum = 4;
    }
    current_test = findSubstringR(str, len, "five", 4);
    if (current_test > -1 && current_test > highest_index) {
        highest_index = current_test;
        *lastnum = 5;
    }
    current_test = findSubstringR(str, len, "six", 3);
    if (current_test > -1 && current_test > highest_index) {
        highest_index = current_test;
        *lastnum = 6;
    }
    current_test = findSubstringR(str, len, "seven", 5);
    if (current_test > -1 && current_test > highest_index) {
        highest_index = current_test;
        *lastnum = 7;
    }
    current_test = findSubstringR(str, len, "eight", 5);
    if (current_test > -1 && current_test > highest_index) {
        highest_index = current_test;
        *lastnum = 8;
    }
    current_test = findSubstringR(str, len, "nine", 4);
    if (current_test > -1 && current_test > highest_index) {
        highest_index = current_test;
        *lastnum = 9;
    }
    // current_test = findSubstringR(str, len, "zero", 4);
    // if (current_test > -1 && current_test > highest_index) {
    //     highest_index = current_test;
    //     *lastnum = 0;
    //}
    
    return highest_index;
}

/* ----------------------------
    Returns the VALUE of the first decimal digit *or* numeric word that appears in `str`.
---------------------------- */
int findFirstNumber(const char* str, int len) {
    int firstn_c_val = -1;
    int firstn_c_idx = findFirstNumberChar(str, len);
    if (firstn_c_idx != -1)
        firstn_c_val = str[firstn_c_idx] - '0';
    
    //printf("first digit search: found %c at index %i\n", firstn_c_val+'0', firstn_c_idx);

    int firstn_w_val = -1;
    int firstn_w_idx = findFirstNumberWord(str, len, &firstn_w_val);

    //printf("first word search: found %i at index %i\n", firstn_w_val, firstn_w_idx);

    if (firstn_c_idx == -1)
        return firstn_w_val;
    if (firstn_w_idx == -1)
        return firstn_c_val;
    return (firstn_c_idx < firstn_w_idx ? firstn_c_val : firstn_w_val);
}

/* ----------------------------
    Returns the VALUE of the last decimal digit *or* numeric word that appears in `str`.
---------------------------- */
int findLastNumber(const char* str, int len) {
    int lastn_c_val = -1;
    int lastn_c_idx = findLastNumberChar(str, len);
    if (lastn_c_idx != -1)
        lastn_c_val = str[lastn_c_idx] - '0';
    
    //printf("last digit search: found %c at index %i\n", lastn_c_val+'0', lastn_c_idx);

    int lastn_w_val = -1;
    int lastn_w_idx = findLastNumberWord(str, len, &lastn_w_val);

    //printf("last word search: found %i at index %i\n", lastn_w_val, lastn_w_idx);
        
    if (lastn_c_idx == -1)
        return lastn_w_val;
    if (lastn_w_idx == -1)
        return lastn_c_val;
    return (lastn_c_idx > lastn_w_idx ? lastn_c_val : lastn_w_val);
}
