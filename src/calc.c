/** @file
   Implementacja kalkulatora wielomianów

   @author Jakub Wróblewski <jw386401@students.mimuw.edu.pl>
   @date 2017-05-22
*/

#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include "poly.h"

#define ASCII_A 65
#define ASCII_Z 90
#define ASCII_a 97
#define ASCII_z 122
#define ASCII_ZERO 48
#define ZERO_LENGTH 5 //4 + 1
#define IS_COEFF_LENGTH 9 //8 + 1
#define IS_ZERO_LENGTH 8 //7 + 1
#define CLONE_LENGTH 6 //5 + 1
#define ADD_LENGTH 4 //3 + 1
#define MUL_LENGTH 4 //3 + 1
#define NEG_LENGTH 4 //3 + 1
#define SUB_LENGTH 4 //3 + 1
#define IS_EQ_LENGTH 6 //5 + 1
#define DEG_LENGTH 4 //3 + 1
#define DEG_BY_LENGTH 7 //6 + 1
#define AT_LENGTH 3 //2 + 1
#define PRINT_LENGTH 6 //5 + 1
#define POP_LENGTH 4 //3 + 1

char* AddCapacityString(char* string) {
    return (char*) realloc(string, 2 * strlen(string) * sizeof(char));
}

char* ParseCommand(char first_char, bool *found_EOF) {
    int size, allocated_length;
    char next_char;
    char *command;
    
    size = 0;
    command = (char*) malloc(sizeof(char));
    allocated_length = 1;
    command[size++] = first_char;
    
    
    while (!(*found_EOF)) {
        next_char = fgetc(stdin);
        if (next_char == EOF || next_char == '\n') {
            if (next_char == EOF) {
                *found_EOF = true;
            }
            break;
        }
        
        if (size == allocated_length) {
            command = AddCapacityString(command);
            allocated_length *= 2;
        }
        command[size++] = next_char;
    }
    
    return command;
}

unsigned ParseDegByVar(char *var, bool *parse_error) {
    unsigned value;
    int digit;
    
    value = 0;
    if (!(*var)) {
        *parse_error = true;
    }
    
    while (*var) {
        if (!isdigit(*var)) {
            *parse_error = true;
            break;
        }
        
        digit = ((int) *var) - ASCII_ZERO;
        
        if (((value * 10) / 10) != value) {
            *parse_error = true;
            break;
        }
        value *= 10;
        
        if (((value + digit) - digit) != value) {
            *parse_error = true;
            break;
        }
        value += digit;
    }
    
    return value;
}

poly_coeff_t ParseAtVar(char *var, bool *parse_error) {
    bool neg;
    poly_coeff_t value;
    int digit;
    
    value = 0;
    neg = false;
    if (!(*var)) {
        *parse_error = true;
    }
    
    while (*var) {
        if (value == 0 && *var == '-') {
            neg = true;
            var++;
            if (!(*var)) {
                *parse_error = true;
                break;
            }
        }
        if (!isdigit(*var)) {
            *parse_error = true;
            break;
        }
        
        digit = ((int) *var) - ASCII_ZERO;
        
        if (((value * 10) / 10) != value) {
            *parse_error = true;
            break;
        }
        value *= 10;
        
        if (!neg) {
            if (((value + digit) - digit) != value) {
                *parse_error = true;
                break;
            }
            value += digit;
        }
        else {
            if (((value - digit) + digit) != value) {
                *parse_error = true;
                break;
            }
            value -= digit;
        }
        
        var++;
    }
    
    return value;
}

int main() {
    bool found_EOF, parse_error;
    int first_char_code, row;
    char first_char, *command;
    poly_coeff_t at_val;
    unsigned deg_var;
    
    found_EOF = false;
    row = 0;
    
    while (!found_EOF) {
        row++;
        first_char = fgetc(stdin);
        first_char_code = (int) first_char;
        
        if ((ASCII_A <= first_char_code && first_char_code <= ASCII_Z)
            || (ASCII_a <= first_char_code && first_char_code <= ASCII_z)) {
            
            command = ParseCommand(first_char, &found_EOF);
            printf("command: %s\n", command);
            
            if (strncmp(command, "ZERO", ZERO_LENGTH) == 0) {
                printf("ZERO\n\n");
                //TODO
            }
            else if (strncmp(command, "IS_COEFF", IS_COEFF_LENGTH) == 0) {
                printf("IS_COEFF\n\n");
                //TODO
            }
            else if (strncmp(command, "IS_ZERO", IS_ZERO_LENGTH) == 0) {
                printf("IS_ZERO\n\n");
                //TODO
            }
            else if (strncmp(command, "CLONE", CLONE_LENGTH) == 0) {
                printf("CLONE\n\n");
                //TODO
            }
            else if (strncmp(command, "ADD", ADD_LENGTH) == 0) {
                printf("ADD\n\n");
                //TODO
            }
            else if (strncmp(command, "MUL", MUL_LENGTH) == 0) {
                printf("MUL\n\n");
                //TODO
            }
            else if (strncmp(command, "NEG", NEG_LENGTH) == 0) {
                printf("NEG\n\n");
                //TODO
            }
            else if (strncmp(command, "SUB", SUB_LENGTH) == 0) {
                printf("SUB\n\n");
                //TODO
            }
            else if (strncmp(command, "IS_EQ", IS_EQ_LENGTH) == 0) {
                printf("IS_EQ\n\n");
                //TODO
            }
            else if (strncmp(command, "DEG", DEG_LENGTH) == 0) {
                printf("DEG\n\n");
                //TODO
            }
            else if (strncmp(command, "DEG_BY", DEG_BY_LENGTH - 1) == 0) {
                if (command[DEG_BY_LENGTH - 1] == ' '
                    || command[DEG_BY_LENGTH - 1] == '\0') {
                    parse_error = false;
                    deg_var = ParseDegByVar(
                        &command[DEG_BY_LENGTH], &parse_error);
                    //TODO
                    
                    if (parse_error) {
                        fprintf(stderr, "ERROR %d WRONG VARIABLE\n", row);
                        parse_error = false;
                    }
                    printf("DEG_BY %u\n\n", deg_var);
                }
                else {
                    fprintf(stderr, "ERROR %d WRONG COMMAND\n", row);
                }
            }
            else if (strncmp(command, "AT", AT_LENGTH - 1) == 0) {
                if (command[AT_LENGTH - 1] == ' '
                    || command[AT_LENGTH - 1] == '\0') {
                    parse_error = false;
                    at_val = ParseAtVar(&command[AT_LENGTH], &parse_error);
                    //TODO
                    
                    if (parse_error) {
                        fprintf(stderr, "ERROR %d WRONG VALUE\n", row);
                        parse_error = false;
                    }
                    printf("AT %ld\n\n", at_val);
                }
                else {
                    fprintf(stderr, "ERROR %d WRONG COMMAND\n", row);
                }
            }
            else if (strncmp(command, "PRINT", PRINT_LENGTH) == 0) {
                printf("PRINT\n\n");
                //TODO
            }
            else if (strncmp(command, "POP", POP_LENGTH) == 0) {
                printf("POP\n\n");
                //TODO
            }
            else {
                fprintf(stderr, "ERROR %d WRONG COMMAND\n", row);
            }
        }
        else if (first_char != EOF) {
            printf("poly: TODO\n");//TODO
        }
        else /* first_char == EOF */ {
            found_EOF = true;
        }
    }
    
    return 0;
}