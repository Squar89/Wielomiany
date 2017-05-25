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
    Poly *top_element;
    unsigned long size;
} Stack;

Stack SetupStack() {
    Stack stack;
    
    stack.top_element = NULL;
    stack.size = 0;
    
    return stack;
}

bool IsEmptyStack(Stack *stack) {
    return stack->top_element == NULL;
}

int Size(Stack *stack) {
    return stack->size;
}

Poly* Peek(Stack *stack) {
    return stack->top_element;
}

void Push(Stack *stack, Poly *poly) {
    stack->size++;
    
    stack->top_element = (Poly*) realloc(stack->top_element, sizeof(Poly));
    (stack->top_element)++;
    stack->top_element = poly;
    
    return;
}

Poly* Pop(Stack *stack) {
    Poly *top;
    
    top = Peek(stack);
    (stack->top_element)--;
    stack->size--;
    
    return top;
}

char* AddCapacityString(char *string);

char* LineToString(bool *found_EOF);

Mono* AddCapacityMonos(Mono *mono_array, unsigned long new_size);

poly_coeff_t ParseCoeff(char **string, bool *parse_error, int *column);

poly_exp_t ParseExp(char **string, bool *parse_error, int *column);

Mono ParseMono(char **string, bool *parse_error, int *column);

Poly ParsePoly(char **string, bool *parse_error, int *column);

unsigned ParseDegByVar(char *var, bool *parse_error);

poly_coeff_t ParseAtVar(char *var, bool *parse_error);

char* AddCapacityString(char *string) {
    return (char*) realloc(string, 2 * strlen(string) * sizeof(char));
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
            line = AddCapacityString(line);
            allocated_length *= 2;
        }
        line[size++] = next_char;
    }
    
    return line;
}

//BUG NIE FREEUJE STAREGO?
Mono* AddCapacityMonos(Mono *mono_array, unsigned long new_size) {
    return (Mono*) realloc(mono_array, new_size * sizeof(char));
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
    Mono mono, *monos, mono1;
    poly_coeff_t coeff_val;
    unsigned mono_count;
    unsigned long allocated_length;
    bool flag = false;
    
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
            if (!flag) {
                flag = true;
                mono1 = mono;
            }
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
                IsEmpty(mono1.p.mono_list);
                assert(false);
                monos[0].p.mono_list = SetupList();//Czyli z jakiegoś powodu, moje p traci dostep do swojej listy gdy tylko nie jest w ostatnim mono z petli
                //IsEmpty(monos[0].p.mono_list);
                PolyToString(&monos[0].p);
                PolyToString(&monos[1].p);
                assert(false);
                printf("%lu %d\n", allocated_length, mono_count);
                //MonoToString(&monos[1]);printf("\n");
                //printf("%ld\n", monos[0].p.constant_value);
                //assert(false);
                //MonoToString(&monos[1]);printf("\n");
                //z jakiegos powodu wszystkie mono rozne od tego ostatniego maja problemy z poly, chyba nie maja one list
        p = PolyAddMonos(mono_count, monos);
        
        free(monos);//BUG
    }
    else /* string to pewna liczba */ {
        coeff_val = ParseCoeff(string, parse_error, column);
        p = PolyFromCoeff(coeff_val);
        printf("Tworzę Poly: %ld\n", coeff_val);
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

void test() {
    Poly *p;
    Poly poly1, poly2, poly3, poly4;
    Stack stack;
    
    stack = SetupStack();
    poly1 = PolyFromCoeff(1);
    poly2 = PolyFromCoeff(2);
    poly3 = PolyFromCoeff(3);
    poly4 = PolyFromCoeff(4);
    
    printf(IsEmptyStack(&stack)? "true\n" : "false\n");
    Push(&stack, &poly1);
    printf(IsEmptyStack(&stack)? "true\n" : "false\n");
    p = Pop(&stack);
    printf(IsEmptyStack(&stack)? "true\n" : "false\n");
    printf("%d", stack.size);//size sie zmniejsza, ale prawdopodobne problemy z wskaznikami na top_element(najprawdopodobnie zepsulem arytmetyke wskaznikow)
}

int main() {
    test();
    bool found_EOF, parse_error;
    int first_char_code, row, column;
    char *line;
    poly_coeff_t at_val;
    unsigned deg_var;
    Poly poly;
    
    found_EOF = false;
    row = 0;
    
    while (!found_EOF) {
        column = 0;
        row++;
        line = LineToString(&found_EOF);
        first_char_code = (int) line[0];
        parse_error = false;
        
        if ((ASCII_A <= first_char_code && first_char_code <= ASCII_Z)
            || (ASCII_a <= first_char_code && first_char_code <= ASCII_z)) {
            
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
        else if (line[0] != EOF) {
            poly = ParsePoly(&line, &found_EOF, &column);
            //PolyToString(&poly);
            printf(parse_error? " (true)\n" : " (false)\n");
            
            if (parse_error) {
                printf("line: %s\n", line);
                
                
            }
        }
        else /* first_char == EOF */ {
            found_EOF = true;
        }
    }
    
    return 0;
}