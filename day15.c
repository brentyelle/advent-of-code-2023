#include <stdio.h>
#include <string.h>
#define BUFFERSIZE 20
#define COMMA ','

typedef enum {false=0, true=1} bool;
typedef unsigned long long ulong;

ulong hash_char(const ulong current_value, const char character);
ulong hash_str(ulong current_value, const char buffer[BUFFERSIZE]);
bool extract_str(FILE* readfile, char buffer[BUFFERSIZE], const char delimiter);
void zero_buffer(char buffer[BUFFERSIZE]);

int main(int argc, char* argv[]) {

    if (argc != 2) {
        fprintf(stderr, "Usage: $ ./a.out <filename>\n");
        return 1;
    }

    ulong sum_hashes = 0;
    char buffer[BUFFERSIZE];
    bool more_to_read;
    
    // open file & check for success
    FILE* infile = fopen(argv[1], "r");
    if (!infile) {
        fprintf(stderr, "File \"%s\" failed to open!\n", argv[1]);
        return 1;
    }

    // initial read, to check for EOF
    more_to_read = extract_str(infile, buffer, COMMA);

    while (more_to_read) {
        // update hash value
        sum_hashes += hash_str(0, buffer);
        // then grab the next string, if there is one
        more_to_read = extract_str(infile, buffer, COMMA);
    }

    printf("Part 1: The final hash is %llu\n", sum_hashes);

    fclose(infile);
}




// Expects a `char` array `buffer` of maximum size `BUFFERSIZE`. Unconditionally overwrites all bytes of `buffer` with the zero-delimiter character `\0`.
void zero_buffer(char buffer[BUFFERSIZE]) {
    for (size_t i=0; i < BUFFERSIZE; i++)
        buffer[i] = '\0';
    return;
}

// Given the current running value in the hash (`current_value`) and the next character (`next_char`), calculates the next value in the hash.
// Returns the new value in the hash after `next_char` is processed.
ulong hash_char(const ulong current_value, const char next_char) {
    return ((current_value + (ulong)next_char) * 17) % 256;
}

// Given the current running value in the hash (`current_value`) and a `char` array (`buffer`) of maximum size `BUFFERSIZE`, runs `hash_char()` successively on all characters in the string-valid portion of `buffer`.
// Returns the new value in the hash after all characters in `buffer`'s string are processed.
ulong hash_str(ulong current_value, const char buffer[BUFFERSIZE]) {
    size_t strlen = strnlen(buffer, BUFFERSIZE);
    for (size_t position=0; position < strlen; position++)
        current_value = hash_char(current_value, buffer[position]);

    fprintf(stderr, "The string \"%s\" hashes to %llu\n", buffer, current_value);
    return current_value;
}

// Given a reading file `readfile`, a writabe buffer `buffer`, and a delimiting character `delimiter`:
// (1) Initializes `buffer` using `zero_buffer()`.
// (2) Grabs characters from `readfile` and writes them into `buffer` starting from position `0`, stopping once `EOF` or `delimiter` is found. If a `delimiter` is found, it is consumed from the filestream.
// (3) Returns `true` if readable characters (characters besides `EOF` and the `delimiter`) are found.
bool extract_str(FILE* readfile, char buffer[BUFFERSIZE], const char delimiter) {
    zero_buffer(buffer);
    bool string_found = false;

    size_t curr_pos = 0;
    int curr_char = fgetc(readfile);
    
    while (curr_char != delimiter && curr_char != EOF) {
        string_found = true;
        buffer[curr_pos] = (char)curr_char;
        curr_pos++;
        curr_char = fgetc(readfile);
    }

    return string_found;
}