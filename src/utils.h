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

#ifndef UTILS_H
#define UTILS_H

#ifdef UNIT_TESTING

/* Przekierowuje exit do funkcji umożliwiającej przechwycenie kończącego się
 * programu */
#ifdef exit
#undef exit
#endif /* exit */
#define exit(status) mock_exit(status)
extern void mock_exit(int status);

/* Przekierowuje printf do funkcji umożliwiającej przetestowanie wyjścia */
#ifdef printf
#undef printf
#endif /* printf */
#define printf(...) mock_printf(__VA_ARGS__)
extern int mock_printf(const char *format, ...);

/* Przekierowuje fprintf do funkcji umożliwiającej przetestowania wyjścia
 * błędów */
#ifdef fprintf
#undef fprintf
#endif /* fprintf */
#define fprintf(...) mock_fprintf(__VA_ARGS__)
extern int mock_fprintf(FILE * const file, const char *format, ...);

/* Przekierowuje scanf do funkcji umożliwiającej przetestowanie wejścia */
#ifdef scanf
#undef scanf
#endif /* scanf */
#define scanf(format, ...) \
    mock_scanf(format"%n", ##__VA_ARGS__, &read_char_count)
extern int read_char_count;
extern int mock_scanf(const char *format, ...);

/* Przekierowuje fgetc do funkcji umożliwiającej przetestowanie wejścia */
#ifdef fgetc
#undef fgetc
#endif /* fgetc */
#define fgetc(...) mock_fgetc(__VA_ARGS__)
extern int mock_fgetc(FILE * const stream);

/* Przekierowuje calloc, malloc, realloc, free do odpowiadających im funkcji.
 * Umożliwia to przetestowanie błędów z pamięcią */
#ifdef calloc
#undef calloc
#endif /* calloc */
#define calloc(num, size) _test_calloc(num, size, __FILE__, __LINE__)
#ifdef malloc
#undef malloc
#endif /* malloc */
#define malloc(size) _test_malloc(size, __FILE__, __LINE__)
#ifdef realloc
#undef realloc
#endif /* realloc */
#define realloc(ptr, size) _test_realloc(ptr, size, __FILE__, __LINE__)
#ifdef free
#undef free
#endif /* free */
#define free(ptr) _test_free(ptr, __FILE__, __LINE__)
void* _test_calloc
    (size_t number_of_elements, size_t size, const char* file, int line);
void* _test_malloc(size_t size, char* const file, int line);
void* _test_realloc(void* const ptr, size_t size, char* const file, int line);
void _test_free(void* const ptr, char* const file, const int line);

/* Przedefiniowuje main, aby umożliwić istnienie funkcji main w
 * unit_tests_poly.c */
#define main(...) calc_poly_main(__VA_ARGS__)
int calc_poly_main(int argc, char *argv[]);

#endif /* UNIT_TESTING */

#endif /* UTILS_H*/