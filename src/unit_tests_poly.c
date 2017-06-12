/*
 * Copyright 2008 Google Inc.
 * Copyright 2015 Tomasz Kociumaka
 * Copyright 2016, 2017 IPP team
 * Copyright 2017 Jakub Wróblewski
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <setjmp.h>
#include <limits.h>
#include "cmocka.h"
#include "poly.h"

/* Ustalam maksymalną długość wejścia na 256. Mogę założyć maksymalną długość,
 * ponieważ to będzie maksymalna długość tylko i wyłącznie testów, które sam
 * stworzę i jeśli wszystko będzie działać tak jak należy, to zmieści się w 
 * tym limicie. */
#define MAX_INPUT_LENGTH 256

static jmp_buf jmp_at_exit;
static int exit_status;

extern int calc_poly_main();

/**
 * Struktura stworzona do ułatwienia kontroli pamięci, zawiera Poly 
 * wykorzystywane w testach.
 */
typedef struct Tools {
    Poly poly_zero; ///< Wielomian zerowy
    Poly poly_coeff_1; ///< Wielomian stały równy 1
    Poly poly_coeff_2; ///< Wielomian stały równy 2
    Poly poly_simple; ///< Wielomian (1,1)
} Tools;
static Tools tools;

/**
 * Atrapa funkcji main.
 */
int mock_main() {
    if (!setjmp(jmp_at_exit)) {
        return calc_poly_main();
    }
    
    return exit_status;
}

/**
 * Atrapa funkcji exit.
 */
void mock_exit(int status) {
    exit_status = status;
    longjmp(jmp_at_exit, 1);
}

/*
//TODO potrzebne?
int mock_fprintf(FILE* const file, const char *format, ...)
    CMOCKA_PRINTF_ATTRIBUTE(2, 3);
int mock_printf(const char *format, ...) CMOCKA_PRINTF_ATTRIBUTE(1, 2);
*/

/**
 * Pomocnicze bufory, do których piszą atrapy funkcji printf i fprintf oraz
 * pozycje zapisu w tych buforach. Pozycja zapisu wskazuje bajt o wartości 0.
 */
static char fprintf_buffer[MAX_INPUT_LENGTH];
static char printf_buffer[MAX_INPUT_LENGTH];
static int fprintf_position = 0;
static int printf_position = 0;

/**
 * Atrapa funkcji fprintf sprawdzająca poprawność wypisywania na stderr.
 */
int mock_fprintf(FILE* const file, const char *format, ...) {
    int return_value;
    va_list args;

    assert_true(file == stderr);
    /* Poniższa asercja sprawdza też, czy fprintf_position jest nieujemne.
    W buforze musi zmieścić się kończący bajt o wartości 0. */
    assert_true((size_t)fprintf_position < sizeof(fprintf_buffer));

    va_start(args, format);
    return_value = vsnprintf(fprintf_buffer + fprintf_position,
                             sizeof(fprintf_buffer) - fprintf_position,
                             format,
                             args);
    va_end(args);

    fprintf_position += return_value;
    assert_true((size_t)fprintf_position < sizeof(fprintf_buffer));
    
    return return_value;
}

/**
 * Atrapa funkcji fprintf sprawdzająca poprawność wypisywania na stdout.
 */
int mock_printf(const char *format, ...) {
    int return_value;
    va_list args;

    /* Poniższa asercja sprawdza też, czy printf_position jest nieujemne.
    W buforze musi zmieścić się kończący bajt o wartości 0. */
    assert_true((size_t)printf_position < sizeof(printf_buffer));

    va_start(args, format);
    return_value = vsnprintf(printf_buffer + printf_position,
                             sizeof(printf_buffer) - printf_position,
                             format,
                             args);
    va_end(args);

    printf_position += return_value;
    assert_true((size_t)printf_position < sizeof(printf_buffer));
    
    return return_value;
}

/**
 * Pomocniczy bufor, z którego korzystają atrapy funkcji operujących na stdin.
 */
static char input_stream_buffer[MAX_INPUT_LENGTH];
static int input_stream_position = 0;
static int input_stream_end = 0;
int read_char_count;

/**
 * Atrapa funkcji scanf używana do przechwycenia czytania z stdin.
 */
int mock_scanf(const char *format, ...) {
    va_list fmt_args;
    int ret;

    va_start(fmt_args, format);
    ret = vsscanf
        (input_stream_buffer + input_stream_position, format, fmt_args);
    va_end(fmt_args);

    if (ret < 0) { /* ret == EOF */
        input_stream_position = input_stream_end;
    }
    else {
        assert_true(read_char_count >= 0);
        input_stream_position += read_char_count;
        if (input_stream_position > input_stream_end) {
            input_stream_position = input_stream_end;
        }
    }
    
    return ret;
}

/**
 * Atrapa funkcji getchar używana do przechwycenia czytania z stdin.
 */
int mock_getchar() {
    if (input_stream_position < input_stream_end)
        return input_stream_buffer[input_stream_position++];
    else
        return EOF;
}

/**
 * Atrapa funkcji fgetc używana do przechwycenia czytania z stdin.
 */
int mock_fgetc(FILE * const stream) {
    assert_true(stream == stdin);
    
    if (input_stream_position < input_stream_end) {
        return input_stream_buffer[input_stream_position++];
    }
    else
        return EOF;
}

/**
 * Funkcja przygotowująca otoczenie przed pierwszą grupą testów.
 */
static int test_group_1_setup(void **state) {
    (void)state;
    
    tools.poly_zero = PolyZero();
    tools.poly_coeff_1 = PolyFromCoeff(1);
    tools.poly_coeff_2 = PolyFromCoeff(2);
    
    Mono monos[1];
    Poly temp = PolyFromCoeff(1);
    monos[0] = MonoFromPoly(&temp, 1);
    tools.poly_simple = PolyAddMonos(1, monos);
    
    return 0;
}

/**
 * Funkcja modyfikująca otoczenie po wykonaniu pierwszej grupy testów.
 */
static int test_group_1_teardown(void **state) {
    (void)state;
    
    PolyDestroy(&tools.poly_zero);
    PolyDestroy(&tools.poly_coeff_1);
    PolyDestroy(&tools.poly_coeff_2);
    PolyDestroy(&tools.poly_simple);
    
    return 0;
}

/**
 * Funkcja wołana przed każdym testem grupy nr 2.
 */
static int test_setup(void **state) {
    (void)state;

    memset(fprintf_buffer, 0, sizeof(fprintf_buffer));
    memset(printf_buffer, 0, sizeof(printf_buffer));
    printf_position = 0;
    fprintf_position = 0;

    /* Zwrócenie zera oznacza sukces. */
    return 0;
}

/**
 * Funkcja wołana po każdym teście grupy nr 2.
 */
static int test_teardown(void **state) {
    (void)state;

    assert_string_equal(printf_buffer, "");
    assert_string_equal(fprintf_buffer, "");

    /* Zwrócenie zera oznacza sukces. */
    return 0;
}

/**
 * Funkcja inicjująca dane wejściowe dla programu korzystającego ze stdin.
 */
static void init_input_stream(const char *str) {
    memset(input_stream_buffer, 0, sizeof(input_stream_buffer));
    input_stream_position = 0;
    input_stream_end = strlen(str);
    assert_true((size_t)input_stream_end < sizeof(input_stream_buffer));
    strcpy(input_stream_buffer, str);
}

/**
 * p wielomian zerowy, count równe 0.
 */
static void test_poly_compose_1(void **state) {
    (void)state;
    
    Poly result, expected_result;
    
    expected_result = tools.poly_zero;
    result = PolyCompose(&tools.poly_zero, 0, NULL);
    
    assert_true(PolyIsEq(&result, &expected_result));
    
    PolyDestroy(&result);
}

/**
 * p wielomian zerowy, count równe 1, x[0] wielomian stały.
 */
static void test_poly_compose_2(void **state) {
    (void)state;
    
    Poly result, expected_result;
    
    expected_result = tools.poly_zero;
    result = PolyCompose(&tools.poly_zero, 1, &tools.poly_coeff_2);
    
    assert_true(PolyIsEq(&result, &expected_result));
    
    PolyDestroy(&result);
}

/**
 * p wielomian stały, count równe 0.
 */
static void test_poly_compose_3(void **state) {
    (void)state;
    
    Poly result, expected_result;
    
    expected_result = tools.poly_coeff_1;
    result = PolyCompose(&tools.poly_coeff_1, 0, NULL);
    
    assert_true(PolyIsEq(&result, &expected_result));
    
    PolyDestroy(&result);
}

/**
 * p wielomian stały, count równe 1, x[0] wielomian stały różny od p.
 */
static void test_poly_compose_4(void **state) {
    (void)state;
    
    Poly result, expected_result;
    
    expected_result = tools.poly_coeff_1;
    result = PolyCompose(&tools.poly_coeff_1, 1, &tools.poly_coeff_2);
    
    assert_true(PolyIsEq(&result, &expected_result));
    
    PolyDestroy(&result);
}

/**
 * p wielomian x0, count równe 0.
 */
static void test_poly_compose_5(void **state) {
    (void)state;
    
    Poly result, expected_result;
    
    expected_result = tools.poly_zero;
    result = PolyCompose(&tools.poly_simple, 0, NULL);
    
    assert_true(PolyIsEq(&result, &expected_result));
    
    PolyDestroy(&result);
}

/**
 * p wielomian x0, count równe 1, x[0] wielomian stały.
 */
static void test_poly_compose_6(void **state) {
    (void)state;
    
    Poly result, expected_result;
    
    expected_result = tools.poly_coeff_2;
    result = PolyCompose(&tools.poly_simple, 1, &tools.poly_coeff_2);
    
    assert_true(PolyIsEq(&result, &expected_result));
    
    PolyDestroy(&result);
}

/**
 * p wielomian x0, count równe 1, x[0] wielomian x0.
 */
static void test_poly_compose_7(void **state) {
    (void)state;
    
    Poly result, expected_result;
    
    expected_result = tools.poly_simple;
    result = PolyCompose(&tools.poly_simple, 1, &tools.poly_simple);
    
    assert_true(PolyIsEq(&result, &expected_result));
    
    PolyDestroy(&result);
}

static void test_compose_command(void **state, char *input,
                                 char *expected_stdout_result,
                                 char *expected_stderr_result) {
    (void)state;
    
    init_input_stream(input);
    
    calc_poly_main();
    //assert_int_equal(calc_poly_main(), 0);
    assert_string_equal(printf_buffer, expected_stdout_result);
    assert_string_equal(fprintf_buffer, expected_stderr_result);
}

/**
 * brak parametru count.
 */
static void test_compose_command_1(void **state) {
    (void)state;
    
    test_compose_command(state, "1\n1\nPRINT\n", "", "");
}

/**
 * minimalna wartość count = 0.
 */
static void test_compose_command_2(void **state) {
    (void)state;
    
    test_compose_command(state, "COMPOSE 0", "", "");
}

/**
 * count = maksymalna wartość reprezentowana w typie unsigned.
 */
static void test_compose_command_3(void **state) {
    (void)state;
    
//TODO ogarnij dodawanie liczby do string
    
    test_compose_command(state,"COMPOSE 4294967295", "",
                         "ERROR 1 STACK UNDERFLOW\n");
}

/**
 * wartość o jeden mniejsza od minimalnej, czyli count = −1.
 */
static void test_compose_command_4(void **state) {
    (void)state;
    
    test_compose_command(state, "COMPOSE -1", "", "ERROR 1 WRONG COUNT\n");
}

/**
 * count = wartość o jeden większa od maksymalnej reprezentowanej w typie 
 * unsigned.
 */
static void test_compose_command_5(void **state) {
    (void)state;
    
//TODO
//    long count = UINT_MAX;
//    count += 1;
    
    test_compose_command(state, "COMPOSE 4294967296", "",
                         "ERROR 1 WRONG COUNT\n");
}

/**
 * count = duża dodatnia wartość, znacznie przekraczająca zakres typu unsigned.
 */
static void test_compose_command_6(void **state) {
    (void)state;
    
    /* żeby liczba ta była na pewno większa od maksymalnego unsigned,
     * bierzemy maksymalny unsigned i mnożymy go razy losową liczbę, np 3 */
//TODO
//    long count = UINT_MAX;
//    count *= 3;
    
    test_compose_command(state, "COMPOSE 54325414236", "",
                         "ERROR 1 WRONG COUNT\n");
}

/**
 * count = kombinacja liter.
 */
static void test_compose_command_7(void **state) {
    (void)state;
    
    test_compose_command(state, "COMPOSE XYZ", "", "ERROR 1 WRONG COUNT\n");
}

/**
 * count = kombinacja cyfr i liter, rozpoczynająca się cyfrą.
 */
static void test_compose_command_8(void **state) {
    (void)state;
    
    test_compose_command(state, "COMPOSE 123XYZ", "", "ERROR 1 WRONG COUNT\n");
}

int main() {
    const struct CMUnitTest group1[] = {
        //cmocka_unit_test(test_poly_compose_1),
        //cmocka_unit_test(test_poly_compose_2),
        //cmocka_unit_test(test_poly_compose_3),
        //cmocka_unit_test(test_poly_compose_4),
        //cmocka_unit_test(test_poly_compose_5),
        //cmocka_unit_test(test_poly_compose_6),
        //cmocka_unit_test(test_poly_compose_7),
    };
    const struct CMUnitTest group2[] = {
        cmocka_unit_test_setup_teardown(test_compose_command_1, test_setup, 
                                        test_teardown),
        cmocka_unit_test_setup_teardown(test_compose_command_2, test_setup, 
                                        test_teardown),
        cmocka_unit_test_setup_teardown(test_compose_command_3, test_setup, 
                                        test_teardown),
        cmocka_unit_test_setup_teardown(test_compose_command_4, test_setup, 
                                        test_teardown),
        cmocka_unit_test_setup_teardown(test_compose_command_5, test_setup, 
                                        test_teardown),
        cmocka_unit_test_setup_teardown(test_compose_command_6, test_setup, 
                                        test_teardown),
        cmocka_unit_test_setup_teardown(test_compose_command_7, test_setup, 
                                        test_teardown),
        cmocka_unit_test_setup_teardown(test_compose_command_8, test_setup, 
                                        test_teardown),
    };
    
    return /*cmocka_run_group_tests(group1, test_group_1_setup,
                                  test_group_1_teardown);
         + */cmocka_run_group_tests(group2, NULL, NULL);
}