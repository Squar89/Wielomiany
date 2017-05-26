/** @file
   Implementacja kalkulatora wielomianów

   @author Jakub Wróblewski <jw386401@students.mimuw.edu.pl>
   @date 2017-05-22
*/
//TODO dodaj asercje przy allocu
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

typedef struct Stack {
    Poly top_element;
    struct Stack *previous;
} Stack;

bool IsEmptyStack(Stack *stack);

Stack SetupStack();

Poly Peek(Stack *stack);

Stack* Push(Stack *stack, Poly *poly);

Stack* Pop(Stack *stack);

char* AddCapacityString(char *string, unsigned long new_size);

char* LineToString(bool *found_EOF);

Mono* AddCapacityMonos(Mono *mono_array, unsigned long new_size);

poly_coeff_t ParseCoeff(char **string, bool *parse_error, int *column);

poly_exp_t ParseExp(char **string, bool *parse_error, int *column);

Mono ParseMono(char **string, bool *parse_error, int *column);

Poly ParsePoly(char **string, bool *parse_error, int *column);

unsigned ParseDegByVar(char *var, bool *parse_error);

poly_coeff_t ParseAtVar(char *var, bool *parse_error);

bool IsEmptyStack(Stack *stack) {
    return stack->previous == NULL;
}

Stack SetupStack() {
    Stack stack;
    
    stack.previous = NULL;
    
    return stack;
}

Poly Peek(Stack *stack) {
    return stack->top_element;
}

Stack* Push(Stack *stack, Poly *poly) {
    Stack *new_top;
    
    new_top = (Stack*) malloc(sizeof(Stack));
    
    new_top->top_element = *poly;
    new_top->previous = stack;
    
    return new_top;
}

Stack* Pop(Stack *stack) {
    Stack *previous;
    
    previous = stack->previous;
    
    free(stack);
    
    return previous;
}

char* AddCapacityString(char *string, unsigned long new_size) {
    return (char*) realloc(string, new_size * sizeof(char));
}

char* LineToString(bool *found_EOF) {
    int size, allocated_length;
    char next_char, *line;
    
    size = 0;
    line = (char*) malloc(sizeof(char));
    allocated_length = 1;
    
    while (!(*found_EOF)) {
        next_char = fgetc(stdin);
        if (next_char == EOF || next_char == '\n') {
            if (next_char == EOF) {
                *found_EOF = true;
            }
            break;
        }
        
        if (size == allocated_length) {
            allocated_length *= 2;
            line = (char*) realloc(line, allocated_length * sizeof(char));
        }
        line[size++] = next_char;
    }
    
    if (!(*found_EOF)){
        if (size == allocated_length) {
            allocated_length *= 2;
            line = (char*) realloc(line, allocated_length * sizeof(char));
        }
        line[size++] = '\0';
    }
    
    
    return line;
}

Mono* AddCapacityMonos(Mono *mono_array, unsigned long new_size) {
    return (Mono*) realloc(mono_array, new_size * sizeof(Mono));
}

poly_coeff_t ParseCoeff(char **string, bool *parse_error, int *column) {
    bool neg;
    poly_coeff_t value;
    int digit;
    
    value = 0;
    neg = false;
    if (**string == '-') {
        neg = true;
        (*string)++;
        *column += *column + 1;
    }
    
    if (!isdigit(**string)) {
        *parse_error = true;
        printf("ParseCoeff1");
        assert(false);
    }
    
    while (isdigit(**string)) {
        digit = ((int) **string) - ASCII_ZERO;
        
        if (((value * 10) / 10) != value) {
            *parse_error = true;
            printf("ParseCoeff2");
            assert(false);
            break;
        }
        value *= 10;
        
        if (!neg) {
            if (((value + digit) - digit) != value) {
                *parse_error = true;
                printf("ParseCoeff3");
                assert(false);
                break;
            }
            value += digit;
        }
        else {
            if (((value - digit) + digit) != value) {
                *parse_error = true;
                printf("ParseCoeff4");
                assert(false);
                break;
            }
            value -= digit;
        }
        
        (*string)++;
        *column = *column + 1;
    }
    
    return value;
}

poly_exp_t ParseExp(char **string, bool *parse_error, int *column) {
    poly_exp_t value;
    int digit;
    
    value = 0;
    if (!isdigit(**string)) {
        *parse_error = true;
        printf("ParseExp1");
        assert(false);
    }
    
    while (isdigit(**string)) {
        digit = ((int) **string) - ASCII_ZERO;
        
        if (((value * 10) / 10) != value) {
            *parse_error = true;
            printf("ParseExp2");
            assert(false);
            break;
        }
        value *= 10;
        
        if (((value + digit) - digit) != value) {
            *parse_error = true;
            printf("ParseExp3");
            assert(false);
            break;
        }
        value += digit;
        
        (*string)++;
        *column = *column + 1;
    }
    
    return value;
}

Mono ParseMono(char **string, bool *parse_error, int *column) {
    Poly coeff;
    poly_exp_t exp;
    Mono mono;
    
    coeff = ParsePoly(string, parse_error, column);
    if (*parse_error == true) {
        printf("ParseMono1");
        assert(false);
        //TODO co ma returnować?
    }
    
    if (**string != ',') {
        *parse_error = true;
        printf("ParseMono2");
        assert(false);
        //TODO co ma returnować?
    }
    (*string)++;
    *column = *column + 1;
    
    exp = ParseExp(string, parse_error, column);
    if (*parse_error == true) {
        printf("ParseMono3");
        assert(false);
        //TODO co ma returnować?
    }
    
    mono = MonoFromPoly(&coeff, exp);
    
    return mono;
}

Poly ParsePoly(char **string, bool *parse_error, int *column) {
    Poly p;
    Mono mono, *monos;
    poly_coeff_t coeff_val;
    unsigned mono_count;
    unsigned long allocated_length;
    
    if (!(**string) || *parse_error == true) {
        *parse_error = true;
        printf("ParsePoly1");
        assert(false);
        //TODO co ma returnować?
    }
    
    if (**string == '(') {
        mono_count = 0;
        monos = (Mono*) malloc(sizeof(Mono));
        allocated_length = 1;
        
        while (**string == '(') {
            (*string)++;
            *column = *column + 1;
            mono = ParseMono(string, parse_error, column);
            if (*parse_error == true) {
                printf("ParsePoly2");
                assert(false);
                //TODO usuwaj mono?
            }
            
            if (mono_count == allocated_length) {
                allocated_length *= 2;
                monos = AddCapacityMonos(monos, allocated_length);
            }
            monos[mono_count++] = mono;
            
            if (**string != ')') {
                *parse_error = true;
                printf("ParsePoly3");
                assert(false);
                //TODO co ma returnować?
            }
            (*string)++;
            *column = *column + 1;
            
            if (**string && **string == '+') {
                (*string)++;
                *column = *column + 1;
                
                if (!(**string) || **string != '(') {
                    *parse_error = true;
                    printf("ParsePoly4");
                    assert(false);
                    //TODO co ma returnować?
                }
                
                printf("%s\n", *string);
            }
        }
        p = PolyAddMonos(mono_count, monos);

        free(monos);
    }
    else /* string to pewna liczba */ {
        coeff_val = ParseCoeff(string, parse_error, column);
        p = PolyFromCoeff(coeff_val);
    }
    
    return p;
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
/*
void test() {
    Poly poly1, poly2, poly3, poly4, p;
    Stack *stack, s;
    
    s = SetupStack();
    stack = &s;
    
    poly1 = PolyFromCoeff(10);
    poly2 = PolyFromCoeff(20);
    poly3 = PolyFromCoeff(30);
    poly4 = PolyFromCoeff(40);
    
    printf(IsEmptyStack(stack)? "true\n" : "false\n");
    stack = Push(stack, &poly1);
    printf(IsEmptyStack(stack)? "true\n" : "false\n");
    p = Peek(stack);
    PolyToString(&p);printf("\n");
    printf(IsEmptyStack(stack)? "true\n" : "false\n");
    stack = Pop(stack);
    printf(IsEmptyStack(stack)? "true\n" : "false\n");
    stack = Push(stack, &poly2);
    stack = Push(stack, &poly3);
    printf(IsEmptyStack(stack)? "true\n" : "false\n");
    p = Peek(stack);
    PolyToString(&p);printf("\n");
    stack = Pop(stack);
    printf(IsEmptyStack(stack)? "true\n" : "false\n");
    stack = Push(stack, &poly4);
    p = Peek(stack);
    PolyToString(&p);printf("\n");
    printf(IsEmptyStack(stack)? "true\n" : "false\n");
    stack = Pop(stack);
    printf(IsEmptyStack(stack)? "true\n" : "false\n");
    p = Peek(stack);
    PolyToString(&p);printf("\n");
    printf(IsEmptyStack(stack)? "true\n" : "false\n");
    stack = Pop(stack);
    printf(IsEmptyStack(stack)? "true\n" : "false\n");
    
    PolyDestroy(&poly1);
    PolyDestroy(&poly2);
    PolyDestroy(&poly3);
    PolyDestroy(&poly4);
}
*/
int main() {
    //test();
    bool found_EOF, parse_error;
    int first_char_code, row, column;
    char *line, *first;
    poly_coeff_t at_val;
    unsigned deg_var;
    Poly poly;
    
    found_EOF = false;
    row = 0;
    
    while (!found_EOF) {
        column = 0;
        row++;
        line = LineToString(&found_EOF);
        first = line;
        first_char_code = (int) line[0];
        parse_error = false;
        if (!found_EOF && 
            ((ASCII_A <= first_char_code && first_char_code <= ASCII_Z)
            || (ASCII_a <= first_char_code && first_char_code <= ASCII_z))) {
            
            printf("command: %s\n", line);
            
            if (strncmp(line, "ZERO", ZERO_LENGTH) == 0) {
                printf("ZERO\n\n");
                //TODO
            }
            else if (strncmp(line, "IS_COEFF", IS_COEFF_LENGTH) == 0) {
                printf("IS_COEFF\n\n");
                //TODO
            }
            else if (strncmp(line, "IS_ZERO", IS_ZERO_LENGTH) == 0) {
                printf("IS_ZERO\n\n");
                //TODO
            }
            else if (strncmp(line, "CLONE", CLONE_LENGTH) == 0) {
                printf("CLONE\n\n");
                //TODO
            }
            else if (strncmp(line, "ADD", ADD_LENGTH) == 0) {
                printf("ADD\n\n");
                //TODO
            }
            else if (strncmp(line, "MUL", MUL_LENGTH) == 0) {
                printf("MUL\n\n");
                //TODO
            }
            else if (strncmp(line, "NEG", NEG_LENGTH) == 0) {
                printf("NEG\n\n");
                //TODO
            }
            else if (strncmp(line, "SUB", SUB_LENGTH) == 0) {
                printf("SUB\n\n");
                //TODO
            }
            else if (strncmp(line, "IS_EQ", IS_EQ_LENGTH) == 0) {
                printf("IS_EQ\n\n");
                //TODO
            }
            else if (strncmp(line, "DEG", DEG_LENGTH) == 0) {
                printf("DEG\n\n");
                //TODO
            }
            else if (strncmp(line, "DEG_BY", DEG_BY_LENGTH - 1) == 0) {
                if (line[DEG_BY_LENGTH - 1] == ' '
                    || line[DEG_BY_LENGTH - 1] == '\0') {
                    parse_error = false;
                    deg_var = ParseDegByVar(
                        &line[DEG_BY_LENGTH], &parse_error);
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
            else if (strncmp(line, "AT", AT_LENGTH - 1) == 0) {
                if (line[AT_LENGTH - 1] == ' '
                    || line[AT_LENGTH - 1] == '\0') {
                    parse_error = false;
                    at_val = ParseAtVar(&line[AT_LENGTH], &parse_error);
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
            else if (strncmp(line, "PRINT", PRINT_LENGTH) == 0) {
                printf("PRINT\n\n");
                //TODO
            }
            else if (strncmp(line, "POP", POP_LENGTH) == 0) {
                printf("POP\n\n");
                //TODO
            }
            else {
                fprintf(stderr, "ERROR %d WRONG COMMAND\n", row);
            }
        }
        else if (!found_EOF) {
            poly = ParsePoly(&line, &found_EOF, &column);
            PolyToString(&poly);
            PolyDestroy(&poly);
            printf(parse_error? " (true)\n" : " (false)\n");
            
            if (parse_error) {
                printf("line: %s\n", line);
                
                
            }
        }
        else /* found_EOF */ {
            found_EOF = true;
        }
        
        free(first);
    }
    
    return 0;
}