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
#include "cmocka.h"
#include "poly.h"

/* TODO napisz mock dla każdej funkcji z utils
 * TODO napisz setup i teardown dla testow
 * TODO napisz kazdy testo
 * TODO napisz wykonywanie tych testow
 */

static jmp_buf jmp_at_exit;
static int exit_status;

extern int calc_poly_main(int argc, char *argv[]);

/**
 * Atrapa funkcji main
 */
int mock_main(int argc, char *argv[]) {
    if (!setjmp(jmp_at_exit))
        return calc_poly_main(argc, argv);
    return exit_status;
}

/**
 * Atrapa funkcji exit
 */
void mock_exit(int status) {
    exit_status = status;
    longjmp(jmp_at_exit, 1);
}