/** @file
   Implementacja kalkulatora wielomianów

   @author Jakub Wróblewski <jw386401@students.mimuw.edu.pl>
   @date 2017-05-22
*/

#include <string.h>
#include <stdio.h>
//#include "poly.h"
#include <stdbool.h>
#include <stdlib.h>

#define ASCII_A 65
#define ASCII_Z 90
#define ASCII_a 97
#define ASCII_z 122

char* AddCapacityString(char* string) {
    return (char*) realloc(string, 2 * strlen(string) * sizeof(char));
}

char* ParseCommand(char first_char, bool *found_EOF, int *x) {
    int size, allocated_length;
    char next_char;
    char *command;
    
    size = 0;
    command = (char*) malloc(sizeof(char));
    allocated_length = 1;
    command[size++] = first_char;
    
    printf(*found_EOF ? "true" : "false");
    
    while (!(*found_EOF)) {
        printf("DEBUG2");
        next_char = fgetc(stdin);
        if (first_char == EOF) {
            *found_EOF = true;
            break;
        }
        else {
            if (size == allocated_length) {
                command = AddCapacityString(command);
                allocated_length *= 2;
            }
            command[size++] = next_char;
        }
    }
    
    return command;
}

int main() {
    bool found_EOF;
    int first_char_code, x;
    char first_char, *command;
    
    found_EOF = false;
    
    while (!found_EOF) {
        first_char = fgetc(stdin);
        first_char_code = (int) first_char;
        
        if ((ASCII_A <= first_char_code && first_char_code <= ASCII_Z)
            || (ASCII_a <= first_char_code && first_char_code <= ASCII_z)) {
                /* "*"? */command = ParseCommand(first_char, &found_EOF, &x);
                printf("command: %s\n", command);
                printf(found_EOF ? "1true" : "1false");
            }
        else if (first_char != EOF) {
            printf("poly: TODO\n");
        }
        else /* first_char == EOF */ {
            found_EOF = true;
        }
    }
    
    return 0;
}