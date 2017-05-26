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

typedef struct Stack {
    Poly top_element;
    struct Stack *previous;
} Stack;

bool IsEmptyStack(Stack *stack);

Stack* SetupStack();

Poly Peek(Stack *stack);

Stack* Push(Stack *stack, Poly poly);

Stack* Pop(Stack *stack);

void ClearStack(Stack *stack);

int IncrementColumn(int increment);

int IncrementRow(int increment);

char* LineToString(bool *found_EOF);

poly_coeff_t ParseCoeff(char **string, bool *parse_error);

poly_exp_t ParseExp(char **string, bool *parse_error);

Mono ParseMono(char **string, bool *parse_error);

Poly ParsePoly(char **string, bool *parse_error);

unsigned ParseDegByVar(char *var, bool *parse_error);

poly_coeff_t ParseAtVar(char *var, bool *parse_error);

void PrintStackUnderflowError();

void PrintWrongCommandError();

void PrintWrongValueError();

void PrintWrongVariableError();

void PrintParseError();

Stack* ZeroCommand(Stack *stack);

void IsCoeffCommand(Stack *stack);

void IsZeroCommand(Stack *stack);

Stack* PolyCloneCommand(Stack *stack);

Stack* AddCommand(Stack *stack);

Stack* MulCommand(Stack *stack);

Stack* NegCommand(Stack *stack);

Stack* SubCommand(Stack *stack);

Stack* IsEqCommand(Stack *stack);

void DegCommand(Stack *stack);

void DegByCommand(Stack *stack, unsigned var);

Stack* AtCommand(Stack *stack, poly_coeff_t val);

void PrintCommand(Stack *stack);

Stack* PopCommand(Stack *stack);

bool IsEmptyStack(Stack *stack) {
    return stack->previous == NULL;
}

Stack* SetupStack() {
    Stack *stack;
    
    stack = (Stack*) malloc(sizeof(Stack));
    assert(stack != NULL);
    stack->previous = NULL;
    
    return stack;
}

Poly Peek(Stack *stack) {
    return stack->top_element;
}

Stack* Push(Stack *stack, Poly poly) {
    Stack *new_top;
    
    new_top = (Stack*) malloc(sizeof(Stack));
    assert(new_top != NULL);
    
    new_top->top_element = poly;
    new_top->previous = stack;
    
    return new_top;
}

Stack* Pop(Stack *stack) {
    Stack *previous;
    
    previous = stack->previous;
    
    free(stack);
    
    return previous;
}

void ClearStack(Stack *stack) {
    Poly element;
    
    while (!IsEmptyStack(stack)) {
        element = Peek(stack);
        PolyDestroy(&element);
        stack = Pop(stack);
    }
    
    free(stack);
    
    return;
}

int IncrementColumn(int increment) {
    static int column_number = 0;
    
    if (increment < 0) {
        column_number = 0;
    }
    else {
        column_number += increment;
    }
    
    return column_number;
}

int IncrementRow(int increment) {
    static int row_number = 0;
    
    if (increment < 0) {
        row_number = 0;
    }
    else {
        row_number += increment;
    }
    
    return row_number;
}

char* LineToString(bool *found_EOF) {
    int size, allocated_length;
    char next_char, *line;
    
    size = 0;
    line = (char*) malloc(sizeof(char));
    assert(line != NULL);
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
            assert(line != NULL);
        }
        line[size++] = next_char;
    }
    
    if (!(*found_EOF)){
        if (size == allocated_length) {
            allocated_length *= 2;
            line = (char*) realloc(line, allocated_length * sizeof(char));
            assert(line != NULL);
        }
        line[size++] = '\0';
    }
    
    
    return line;
}

poly_coeff_t ParseCoeff(char **string, bool *parse_error) {
    bool neg;
    poly_coeff_t value;
    int digit;
    
    value = 0;
    neg = false;
    if (**string == '-') {
        neg = true;
        (*string)++;
        IncrementColumn(1);
    }
    
    if (!isdigit(**string)) {
        *parse_error = true;
        PrintParseError();
        
        return value;
    }
    
    while (isdigit(**string)) {
        digit = ((int) **string) - ASCII_ZERO;
        
        if (((value * 10) / 10) != value) {
            *parse_error = true;
            PrintParseError();
            break;
        }
        value *= 10;
        
        if (!neg) {
            if (((value + digit) - digit) != value) {
                *parse_error = true;
                PrintParseError();
                break;
            }
            value += digit;
        }
        else {
            if (((value - digit) + digit) != value) {
                *parse_error = true;
                PrintParseError();
                break;
            }
            value -= digit;
        }
        
        (*string)++;
        IncrementColumn(1);
    }
    
    return value;
}

poly_exp_t ParseExp(char **string, bool *parse_error) {
    poly_exp_t value;
    int digit;
    
    value = 0;
    if (!isdigit(**string)) {
        *parse_error = true;
        PrintParseError();;
    }
    
    while (isdigit(**string)) {
        digit = ((int) **string) - ASCII_ZERO;
        
        if (((value * 10) / 10) != value) {
            *parse_error = true;
            PrintParseError();
            break;
        }
        value *= 10;
        
        if (((value + digit) - digit) != value) {
            *parse_error = true;
            PrintParseError();
            break;
        }
        value += digit;
        
        (*string)++;
        IncrementColumn(1);
    }
    
    return value;
}

Mono ParseMono(char **string, bool *parse_error) {
    Poly coeff;
    poly_exp_t exp;
    
    coeff = ParsePoly(string, parse_error);
    if (*parse_error == true) {
        return MonoFromPoly(&coeff, 0);
    }
    
    if (**string != ',') {
        *parse_error = true;
        PrintParseError();
        
        return MonoFromPoly(&coeff, 0);
    }
    (*string)++;
    IncrementColumn(1);
    
    exp = ParseExp(string, parse_error);
    /* ewentualny error zostanie przechwycony w następnej funkcji, tutaj
     * tylko zwracałby dokładnie to samo */
    
    return MonoFromPoly(&coeff, exp);
}

Poly ParsePoly(char **string, bool *parse_error) {
    Poly p;
    Mono mono, *monos;
    poly_coeff_t coeff_val;
    unsigned mono_count;
    unsigned long allocated_length;
    
    if (!(**string) || *parse_error == true) {
        if (*parse_error == false) {
            PrintParseError();
        }
        *parse_error = true;
        
        return PolyZero();
    }
    
    if (**string == '(') {
        mono_count = 0;
        monos = (Mono*) malloc(sizeof(Mono));
        assert(monos != NULL);
        allocated_length = 1;
        
        while (**string == '(') {
            (*string)++;
            IncrementColumn(1);
            mono = ParseMono(string, parse_error);
            if (*parse_error == true) {
                MonoDestroy(&mono);
                break;
            }
            
            if (mono_count == allocated_length) {
                allocated_length *= 2;
                monos = (Mono*) realloc(monos, allocated_length *sizeof(Mono));
                assert(monos != NULL);
            }
            monos[mono_count++] = mono;
            
            if (**string != ')') {
                *parse_error = true;
                PrintParseError();
                break;
            }
            (*string)++;
            IncrementColumn(1);
            
            if (**string && **string == '+') {
                (*string)++;
                IncrementColumn(1);
                
                if (!(**string) || **string != '(') {
                    if (*parse_error == false) {
                        PrintParseError();
                    }
                    *parse_error = true;
                    break;
                }
            }
        }
        p = PolyAddMonos(mono_count, monos);
        
        free(monos);
    }
    else /* string to pewna liczba */ {
        coeff_val = ParseCoeff(string, parse_error);
        /* ewentualny błąd parsowania został już wypisany */
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
        
        var++;
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

void PrintStackUnderflowError() {
    fprintf(stderr, "ERROR %d STACK UNDERFLOW\n", IncrementRow(0));
    
    return;
}

void PrintWrongCommandError() {
    fprintf(stderr, "ERROR %d WRONG COMMAND\n", IncrementRow(0));
    
    return;
}

void PrintWrongValueError() {
    fprintf(stderr, "ERROR %d WRONG VALUE\n", IncrementRow(0));
    
    return;
}

void PrintWrongVariableError() {
    fprintf(stderr, "ERROR %d WRONG VARIABLE\n", IncrementRow(0));
    
    return;
}

void PrintParseError() {
    fprintf(stderr, "ERROR %d %d\n", IncrementRow(0), IncrementColumn(1));
    
    return;
}

Stack* ZeroCommand(Stack *stack) {
    Poly poly_to_add;
    
    poly_to_add = PolyZero();
    
    return Push(stack, poly_to_add);
}

void IsCoeffCommand(Stack *stack) {
    Poly top;
    
    if (IsEmptyStack(stack)) {
        PrintStackUnderflowError();
        return;
    }
    top = Peek(stack);
    
    printf(PolyIsCoeff(&top) ? "1\n" : "0\n");
    
    return;
}

void IsZeroCommand(Stack *stack) {
    Poly top;
    
    if (IsEmptyStack(stack)) {
        PrintStackUnderflowError();
        return;
    }    
    top = Peek(stack);
    
    printf(PolyIsZero(&top) ? "1\n" : "0\n");
    
    return;
}

Stack* PolyCloneCommand(Stack *stack) {
    Poly top, cloned_top;
    
    if (IsEmptyStack(stack)) {
        PrintStackUnderflowError();
        return stack;
    }
    top = Peek(stack);
    cloned_top = PolyClone(&top);
    
    return Push(stack, cloned_top);
}

Stack* AddCommand(Stack *stack) {
    Poly first, second, sum;
    
    if (IsEmptyStack(stack)) {
        PrintStackUnderflowError();
        return stack;
    }
    first = Peek(stack);
    stack = Pop(stack);
    
    if (IsEmptyStack(stack)) {
        PrintStackUnderflowError();
        return Push(stack, first);
    }
    second = Peek(stack);
    stack = Pop(stack);
    
    sum = PolyAdd(&first, &second);
    PolyDestroy(&first);
    PolyDestroy(&second);
    
    return Push(stack, sum);
}

Stack* MulCommand(Stack *stack) {
    Poly first, second, mul;
    
    if (IsEmptyStack(stack)) {
        PrintStackUnderflowError();
        return stack;
    }
    first = Peek(stack);
    stack = Pop(stack);
    
    if (IsEmptyStack(stack)) {
        PrintStackUnderflowError();
        
        stack = Push(stack, first);
        return stack;
    }
    second = Peek(stack);
    stack = Pop(stack);
    
    mul = PolyAdd(&first, &second);
    PolyDestroy(&first);
    PolyDestroy(&second);
    
    return Push(stack, mul);
}

Stack* NegCommand(Stack *stack) {
    Poly top, neg;
    
    if (IsEmptyStack(stack)) {
        PrintStackUnderflowError();
        return stack;
    }
    top = Peek(stack);
    stack = Pop(stack);
    
    neg = PolyNeg(&top);
    PolyDestroy(&top);
    
    return Push(stack, neg);
}

Stack* SubCommand(Stack *stack) {
    Poly first, second, neg_second, sub;
    
    if (IsEmptyStack(stack)) {
        PrintStackUnderflowError();
        return stack;
    }
    first = Peek(stack);
    stack = Pop(stack);
    
    if (IsEmptyStack(stack)) {
        PrintStackUnderflowError();
        return Push(stack, first);
    }
    second = Peek(stack);
    stack = Pop(stack);
    
    neg_second = PolyNeg(&second);
    sub = PolySub(&first, &second);
    PolyDestroy(&first);
    PolyDestroy(&second);
    PolyDestroy(&neg_second);
    
    return Push(stack, sub);
}

Stack* IsEqCommand(Stack *stack) {
    Poly first, second;
    
    if (IsEmptyStack(stack)) {
        PrintStackUnderflowError();
        return stack;
    }
    first = Peek(stack);
    stack = Pop(stack);
    
    if (IsEmptyStack(stack)) {
        PrintStackUnderflowError();
        return Push(stack, first);
    }
    second = Peek(stack);
    stack = Push(stack, first);
    
    printf(PolyIsEq(&first, &second) ? "1\n" : "0\n");
    
    return stack;
}

void DegCommand(Stack *stack) {
    Poly top;
    
    if (IsEmptyStack(stack)) {
        PrintStackUnderflowError();
        return;
    }
    top = Peek(stack);
    
    printf("%d\n", PolyDeg(&top));
    
    return;
}

void DegByCommand(Stack *stack, unsigned var) {
    Poly top;
    
    if (IsEmptyStack(stack)) {
        PrintStackUnderflowError();
        return;
    }
    top = Peek(stack);
    
    printf("%d\n", PolyDegBy(&top, var));
    
    return;
}

Stack* AtCommand(Stack *stack, poly_coeff_t val) {
    Poly top, result;
    
    if (IsEmptyStack(stack)) {
        PrintStackUnderflowError();
        return stack;
    }
    top = Peek(stack);
    stack = Pop(stack);
    
    result = PolyAt(&top, val);
    PolyDestroy(&top);
    
    return Push(stack, result);
}

void PrintCommand(Stack *stack) {
    Poly top;
    
    if (IsEmptyStack(stack)) {
        PrintStackUnderflowError();
        return;
    }
    top = Peek(stack);
    
    PolyToString(&top);
    printf("\n");
    
    return;
}

Stack* PopCommand(Stack *stack) {
    Poly top;
    
    if (IsEmptyStack(stack)) {
        PrintStackUnderflowError();
        return stack;
    }
    top = Peek(stack);
    PolyDestroy(&top);
    
    return Pop(stack);
}

int main() {
    bool found_EOF, parse_error;
    int first_char_code;
    char *line, *first;
    poly_coeff_t at_val;
    unsigned deg_var;
    Poly poly;
    Stack *stack;
    
    found_EOF = false;
    stack = SetupStack();
    
    while (!found_EOF) {
        IncrementColumn(-1);
        IncrementRow(1);
        line = LineToString(&found_EOF);
        first = line;
        first_char_code = (int) line[0];
        parse_error = false;
        if (!found_EOF && 
            ((ASCII_A <= first_char_code && first_char_code <= ASCII_Z)
            || (ASCII_a <= first_char_code && first_char_code <= ASCII_z))) {
            
            if (strncmp(line, "ZERO", ZERO_LENGTH) == 0) {
                stack = ZeroCommand(stack);
            }
            else if (strncmp(line, "IS_COEFF", IS_COEFF_LENGTH) == 0) {
                IsCoeffCommand(stack);
            }
            else if (strncmp(line, "IS_ZERO", IS_ZERO_LENGTH) == 0) {
                IsZeroCommand(stack);
            }
            else if (strncmp(line, "CLONE", CLONE_LENGTH) == 0) {
                stack = PolyCloneCommand(stack);
            }
            else if (strncmp(line, "ADD", ADD_LENGTH) == 0) {
                stack = AddCommand(stack);
            }
            else if (strncmp(line, "MUL", MUL_LENGTH) == 0) {
                stack = MulCommand(stack);
            }
            else if (strncmp(line, "NEG", NEG_LENGTH) == 0) {
                stack = NegCommand(stack);
            }
            else if (strncmp(line, "SUB", SUB_LENGTH) == 0) {
                stack = SubCommand(stack);
            }
            else if (strncmp(line, "IS_EQ", IS_EQ_LENGTH) == 0) {
                stack = IsEqCommand(stack);
            }
            else if (strncmp(line, "DEG", DEG_LENGTH) == 0) {
                DegCommand(stack);
            }
            else if (strncmp(line, "DEG_BY", DEG_BY_LENGTH - 1) == 0) {
                if (line[DEG_BY_LENGTH - 1] == ' '
                    || line[DEG_BY_LENGTH - 1] == '\0') {
                    
                    deg_var = ParseDegByVar(
                        &line[DEG_BY_LENGTH], &parse_error);
                    
                    if (parse_error) {
                        PrintWrongVariableError();
                    }
                    else {
                        DegByCommand(stack, deg_var);
                    }
                }
                else {
                    PrintWrongCommandError();
                }
            }
            else if (strncmp(line, "AT", AT_LENGTH - 1) == 0) {
                if (line[AT_LENGTH - 1] == ' '
                    || line[AT_LENGTH - 1] == '\0') {
                    
                    at_val = ParseAtVar(&line[AT_LENGTH], &parse_error);
                    
                    if (parse_error) {
                        PrintWrongValueError();
                    }
                    else {
                        stack = AtCommand(stack, at_val);
                    }
                }
                else {
                    PrintWrongCommandError();
                }
            }
            else if (strncmp(line, "PRINT", PRINT_LENGTH) == 0) {
                PrintCommand(stack);
            }
            else if (strncmp(line, "POP", POP_LENGTH) == 0) {
                stack = PopCommand(stack);
            }
            else {
                PrintWrongCommandError();
            }
        }
        else if (!found_EOF) {
            poly = ParsePoly(&line, &parse_error);
            if (parse_error) {
                PolyDestroy(&poly);
            }
            else {
                stack = Push(stack, poly);
            }
        }
        else /* found_EOF */ {
            found_EOF = true;
        }
        
        free(first);
    }
    
    ClearStack(stack);
    
    return 0;
}