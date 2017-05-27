/** @file
   Implementacja kalkulatora wielomianów

   @author Jakub Wróblewski <jw386401@students.mimuw.edu.pl>
   @date 2017-05-22
*/
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <limits.h>
#include "poly.h"

#define ASCII_A 65 /* wartości ascii dla danych liter alfabetu */
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

/**
 * Struktura przechowująca stos.
 * Jest zbudowana z najwyższego elementu oraz
 * wskaźnika na stos elementu poniżej
 */
typedef struct Stack {
    Poly top_element; ///< element na szczycie stosu
    struct Stack *previous;///< wskaźnik na stos elementu poniżej
} Stack;

/**
 * Sprawdza czy stos jest pusty.
 * @param[in] stack : stos dla którego chcemy sprawdzić czy jest pusty
 * @return Czy stos jest pusty?
 */
bool IsEmptyStack(Stack *stack);

/**
 * Funkcja przygotowująca stos.
 * @return wskaźnik na przygotowany stos
 */
Stack* SetupStack();

/**
 * Zwraca najwyższy element stosu.
 * @param[in] stack : stos którego najwyższy element chcemy zobaczyć
 * @return wielomian położony najwyżej na stosie
 */
Poly Peek(Stack *stack);

/**
 * Funkcja dokładająca podany wielomian na szczyt stosu.
 * @param[in] stack : stos do którego chcemy dołożyć wielomian
 * @param[in] poly : wielomian który chcemy dołożyć na stos
 * @return zaaktualizowany stos z danym wielomianem na szczycie
 */
Stack* Push(Stack *stack, Poly poly);

/**
 * Usuwa najwyższy element stosu.
 * @param[in] stack : stos dla którego chcemy usunąc najwyższy element
 * @return zaaktualizowany stos z usuniętym najwyższym elementem
 */
Stack* Pop(Stack *stack);

/**
 * Usuwa stos z pamięci razem z jego zawartością
 * @param[in] stack : stos który chcemy całkowicie usunąć
 */
void ClearStack(Stack *stack);

/**
 * Zmienia wartość zmiennej statycznej column.
 * Jeśli podana liczba jest <0, to zeruje wartość column.
 * W p.p. zwiększa wartość column o daną liczbę;
 * @param[in] increment : liczba o którą chcemy zwiększyć wartość column
 * @return wartość column
 */
int IncrementColumn(int increment);

/**
 * Zmienia wartość zmiennej statycznej row.
 * Jeśli podana liczba jest <0, to zeruje wartość row.
 * W p.p. zwiększa wartość row o daną liczbę;
 * @param[in] increment : liczba o którą chcemy zwiększyć wartość row
 * @return wartość row
 */
int IncrementRow(int increment);

/**
 * Wczytuje dane wejściowe aż do napotkania nowej linii/EOFa.
 * @param[in] found_EOF : wartość logiczna ustawiana na true w przypadku
 *                        napotkania EOFa
 * @return tablica charów reprezentująca dane wejściowe
 */
char* LineToString(bool *found_EOF);

/**
 * Przetwarza podany string wczytując liczbę reprezentującą współczynnik
 * wielomianu. W przypadku błędnych danych wejściowych zmienia wartość
 * pod adresem parse_error na true i wypisuje komunikat o błędzie.
 * @param[in] string : podana tablica charów do przetworzenia
 * @param[in] parse_error : wskaźnik na wartość logiczną - czy wystąpił błąd?
 * @return przetworzona liczba typu poly_coeff_t
 */
poly_coeff_t ParseCoeff(char **string, bool *parse_error);

/**
 * Przetwarza podany string wczytując liczbę reprezentującą wykładnik
 * wielomianu. W przypadku błędnych danych wejściowych zmienia wartość pod
 * adresem parse_error na true i wypisuje komunikat o błędzie.
 * @param[in] string : podana tablica charów do przetworzenia
 * @param[in] parse_error : wskaźnik na wartość logiczną - czy wystąpił błąd?
 * @return przetworzona liczba typu poly_exp_t
 */
poly_exp_t ParseExp(char **string, bool *parse_error);

/**
 * Przetwarza podany string wczytując jednomian. W przypadku błędnych
 * danych wejściowych zmienia wartość pod adresem parse_error na true
 * i wypisuje komunikat o błędzie.
 * @param[in] string : podana tablica charów do przetworzenia
 * @param[in] parse_error : wskaźnik na wartość logiczną - czy wystąpił błąd?
 * @return przetworzony jednomian
 */
Mono ParseMono(char **string, bool *parse_error);

/**
 * Przetwarza podany string wczytując wielomian. W przypadku błędnych danych
 * wejściowych zmienia wartość pod adresem parse_error na true i wypisuje
 * komunikat o błędzie.
 * @param[in] string : podana tablica charów do przetworzenia
 * @param[in] parse_error : wskaźnik na wartość logiczną - czy wystąpił błąd?
 * @return przetworzony wielomian
 */
Poly ParsePoly(char **string, bool *parse_error);

/**
 * Przetwarza podany string wczytując liczbę reprezentującą parametr do
 * funkcji DegBy. W przypadku błędnych danych wejściowych zmienia wartość pod
 * adresem parse_error na true.
 * @param[in] var : podana tablica charów do przetworzenia
 * @param[in] parse_error : wskaźnik na wartość logiczną - czy wystąpił błąd?
 * @return przetworzona liczba typu unsigned
 */
unsigned ParseDegByVar(char *var, bool *parse_error);

/**
 * Przetwarza podany string wczytując liczbę reprezentującą parametr do
 * funkcji At. W przypadku błędnych danych wejściowych zmienia wartośc pod
 * adresem parse_error na true.
 * @param[in] var : podana tablica charów do przetworzenia
 * @param[in] parse_error : wskaźnik na wartość logiczną - czy wystąpił błąd?
 * @return przetworzona liczba typu poly_coeff_t
 */
poly_coeff_t ParseAtVar(char *var, bool *parse_error);

/**
 * Wypisuje komunikat o błędzie zbyt małej liczby wielomianów na stosie.
 */
void PrintStackUnderflowError();

/**
 * Wypisuje komunikat o błędzie złej komendy.
 */
void PrintWrongCommandError();

/**
 * Wypisuje komunikat o błędzie złej wartości przy komendzie AT.
 */
void PrintWrongValueError();

/**
 * Wypisuje komunikat o błędzie złej wartości przy komendzie DEG_BY.
 */
void PrintWrongVariableError();

/**
 * Wypisuje komunikat o błędzie przy próbie wczytania wielomianu.
 */
void PrintParseError();

/**
 * Dodaje na stos zerowy wielomian.
 * @param[in] stack : stan stosu przed dodaniem zerowego wielomianu
 * @return stos po dodaniu zerowego wielomianu
 */
Stack* ZeroCommand(Stack *stack);

/**
 * Sprawdza czy wielomian na szczycie stosu jest współczynnikiem i wypisuje
 * wynik. W przypadku pustego stosu wypisuje komunikat o błędzie.
 * @param[in] stack : stos dla którego chcemy sprawdzić najwyższy element
 */
void IsCoeffCommand(Stack *stack);

/**
 * Sprawdza czy wielomian na szczycie stosu jest wielomianem zerowym i 
 * wypisuje wynik. W przypadku pustego stosu wypisuje komunikat o błędzie.
 * @param[in] stack : stos dla którego chcemy sprawdzić najwyższy element
 */
void IsZeroCommand(Stack *stack);

/**
 * Klonuje wierzchołek stosu i dodaje go do tego samego stosu. W przypadku 
 * pustego stosu wypisuje komunikat o błędzie.
 * @param[in] stack : stos którego wierzchołek chcemy sklonować
 * @return stos po dodaniu duplikatu wierzchołka na szczyt
 */
Stack* PolyCloneCommand(Stack *stack);

/**
 * Dodaje dwa najwyżej położone wielomiany ze stosu, usuwa je i dodaje wynik
 * dodawania. W przypadku niewystarczającej liczby wielomianów na stosie
 * wypisuje komunikat o błędzie.
 * @param[in] stack : stos z którego wielomiany chcemy dodać
 * @return stos po wykonaniu operacji na wielomianach i dodaniu wyniku
 */
Stack* AddCommand(Stack *stack);

/**
 * Mnoży dwa najwyżej położone wielomiany ze stosu, usuwa je i dodaje wynik 
 * mnożenia. W przypadku niewystarczającej liczby wielomianów na stosie 
 * wypisuje komunikat o błędzie.
 * @param[in] stack : stos z którego wielomiany chcemy pomożyć
 * @return stos po wykonaniu operacji na wielomianach i dodaniu ich wyniku
 */
Stack* MulCommand(Stack *stack);

/**
 * Neguje najwyżej położony wielomian ze stosu. W przypadku pustego stosu
 * wypisuje komunikat o błędzie.
 * @param[in] stack : stos którego wierzchołek chcemy zanegować
 * @return stos po wykonaniu operacji negacji na jego wierzchołku
 */
Stack* NegCommand(Stack *stack);

/**
 * Odejmuje dwa najwyżej położone wielomiany ze stosu, usuwa je i dodaje
 * wynik odejmowania. W przypadku niewystarczającej liczby wielomianów na
 * stosie wypisuje komunikat o błędzie.
 * @param[in] stack : stos którego wielomiany chcemy odjąć
 * @return stos po wykonaniu operacji na wielomianach i dodaniu ich wyniku
 */
Stack* SubCommand(Stack *stack);

/**
 * Sprawdza czy dwa najwyżej położone wielomiany ze stosą są sobie równe
 * i wypisuje wynik. W przypadku niewystarczającej liczby wielomianów na 
 * stosie wypisuje komunikat o błędzie.
 * @param[in] stack : stos którego wielomiany chcemy porównać
 * @return stan stosu po wykonaniu porównania
 */
Stack* IsEqCommand(Stack *stack);

/**
 * Oblicza i wypisuje stopień wielomianu z wierzchołka stosu. W przypadku
 * pustego stosu wypisuje komunikat o błędzie.
 * @param[in] stack : stos dla którego wierzchołka chcemy wyliczyć stopień
 */
void DegCommand(Stack *stack);

/**
 * Oblicza i wypisuje stopień wielomianu z wierzchołka stosu ze względu na 
 * daną zmienną. W przypadku pustego stosu wypisuje komunikat o błędzie.
 * @param[in] stack : stos dla którego wierzchołka chcemy wyliczyć stopień
 * @param[in] var : zmienna dla której chcemy obliczyć stopień wielomianu
 */
void DegByCommand(Stack *stack, unsigned var);

/**
 * Wylicza wartość wielomianu z wierzchołka stosu w podanym punkcie, usuwa
 * ten wielomian ze stosu i wynik działania wrzuca na stos. W przypadku
 * pustego stosu wypisuje komunikat o błędzie.
 * @param[in] stack : stos dla którego wierzchołka chcemy wyliczyć wartość
 *                    w punkcie
 * @param[in] val : punkt w którym chcemy obliczyć wartość wielomianu
 * @return stos po dodaniu wielomianu z wykonanej operacji.
 */
Stack* AtCommand(Stack *stack, poly_coeff_t val);

/**
 * Wypisuje wielomian z wierzchołka stosu. W przypadku pustego stosu wypisuje 
 * komunikat o błędzie.
 * @param[in] stack : stos którego wierzchołek chcemy wypisać.
 */
void PrintCommand(Stack *stack);

/**
 * Usuwa wielomian z wierzchołka stosu. W przypadku pustego stosu wypisuje 
 * komunikat o błędzie.
 * @param[in] stack : stos którego wierzchołek chcemy usunąć.
 */
Stack* PopCommand(Stack *stack);

int main() {
    bool found_EOF, parse_error;
    int first_char_code;
    char *line, *first_char;
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
        first_char = line;
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
            if (*line != '\0' && !parse_error) {
                parse_error = true;
                PrintParseError();
            }
            
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
        
        /* w czasie wczytywania linii przesuneliśmy wskaźnik line na koniec 
         * wejścia, dlatego wywołujemy free first_char */
        free(first_char);
    }
    
    ClearStack(stack);
    
    return 0;
}

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
            if (value > LONG_MAX - digit) {
                *parse_error = true;
                PrintParseError();
                break;
            }
            value += digit;
        }
        else {
            if (value < LONG_MIN + digit) {
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
        PrintParseError();
    }
    
    while (isdigit(**string)) {
        digit = ((int) **string) - ASCII_ZERO;
        
        
        if (value > INT_MAX / 10) {
            *parse_error = true;
            PrintParseError();
            break;
        }
        value *= 10;
        
        if (value > INT_MAX - digit) {
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
            
            if (**string) {
                if (**string == ',') {
                    break;
                }
                if (**string != '+') {
                    *parse_error = true;
                    PrintParseError();
                    break;
                }
                
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
        
        if (value > UINT_MAX - digit) {
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
            if (value > LONG_MAX - digit) {
                *parse_error = true;
                break;
            }
            value += digit;
        }
        else {
            if (value < LONG_MIN + digit) {
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
    
    mul = PolyMul(&first, &second);
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