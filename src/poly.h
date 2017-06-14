/** @file
   Interfejs klasy wielomianów

   @author Jakub Pawlewicz <pan@mimuw.edu.pl>,
           Jakub Wróblewski <jw386401@students.mimuw.edu.pl>
   @copyright Uniwersytet Warszawski
   @date 2017-04-09, 2017-05-13
*/

#ifndef __POLY_H__
#define __POLY_H__

#include <stddef.h>
#include <stdlib.h>
#include <assert.h>
#include "list.h"

/** Typ współczynników wielomianu */
typedef long poly_coeff_t;

/** Typ wykładników wielomianu */
typedef int poly_exp_t;

/**
 * Struktura przechowująca wielomian
 * Wielomian jest zbudowany z list jednomianów
 * Jeśli wielomian jest stały, to jego wartość jest
 * przechowywana w constant_value
 */
typedef struct Poly {
    List *mono_list; ///< lista wskaźników na jednomiany wielomianu
    poly_coeff_t constant_value; ///< wartość dla stałego wielomianu
} Poly;

/**
  * Struktura przechowująca jednomian
  * Jednomian ma postać `p * x^e`.
  * Współczynnik `p` może też być wielomianem.
  * Będzie on traktowany jako wielomian nad kolejną zmienną (nie nad x).
  */
typedef struct Mono {
    Poly p; ///< współczynnik
    poly_exp_t exp; ///< wykładnik
    bool is_allocated; ///< czy został zaalokowany
} Mono;

/**
 * Tworzy wielomian, który jest współczynnikiem.
 * @param[in] c : wartość współczynnika
 * @return wielomian
 */
static inline Poly PolyFromCoeff(poly_coeff_t c) {
    return (Poly) {.mono_list = SetupList(), .constant_value = c};
}

/**
 * Tworzy wielomian tożsamościowo równy zeru.
 * @return wielomian
 */
static inline Poly PolyZero() {
    return PolyFromCoeff(0); 
}

/**
 * Tworzy jednomian `p * x^e`.
 * Przejmuje na własność zawartość struktury wskazywanej przez @p p.
 * @param[in] p : wielomian - współczynnik jednomianu
 * @param[in] e : wykładnik
 * @return jednomian `p * x^e`
 */
static inline Mono MonoFromPoly(const Poly *p, poly_exp_t e) {
    return (Mono) {.p = *p, .exp = e, .is_allocated = false};
}

/**
 * Sprawdza, czy wielomian jest współczynnikiem.
 * @param[in] p : wielomian
 * @return Czy wielomian jest współczynnikiem?
 */
static inline bool PolyIsCoeff(const Poly *p) {
    return IsEmpty(p->mono_list);
}

/**
 * Sprawdza, czy wielomian jest tożsamościowo równy zeru.
 * @param[in] p : wielomian
 * @return Czy wielomian jest równy zero?
 */
static inline bool PolyIsZero(const Poly *p) {
    return IsEmpty(p->mono_list) && p->constant_value == 0;
}

/**
 * Usuwa wielomian z pamięci.
 * @param[in] p : wielomian
 */
void PolyDestroy(Poly *p);

/**
 * Usuwa jednomian z pamięci.
 * @param[in] m : jednomian
 */
void MonoDestroy(Mono *m);

/**
 * Robi pełną, głęboką kopię wielomianu.
 * @param[in] p : wielomian
 * @return skopiowany wielomian
 */
Poly PolyClone(const Poly *p);

/**
 * Robi pełną, głęboką kopię jednomianu.
 * @param[in] m : jednomian
 * @return skopiowany jednomian
 */
static inline Mono MonoClone(const Mono *m) {
    return (Mono) {.p = PolyClone(&(m->p)), .exp = m->exp,
        .is_allocated = false};
}

/**
 * Dodaje dwa wielomiany.
 * @param[in] p : wielomian
 * @param[in] q : wielomian
 * @return `p + q`
 */
Poly PolyAdd(const Poly *p, const Poly *q);

/**
 * Sumuje listę jednomianów i tworzy z nich wielomian.
 * Przejmuje na własność zawartość tablicy @p monos.
 * @param[in] count : liczba jednomianów
 * @param[in] monos : tablica jednomianów
 * @return wielomian będący sumą jednomianów
 */
Poly PolyAddMonos(unsigned count, const Mono monos[]);

/**
 * Mnoży dwa wielomiany.
 * @param[in] p : wielomian
 * @param[in] q : wielomian
 * @return `p * q`
 */
Poly PolyMul(const Poly *p, const Poly *q);

/**
 * Zwraca przeciwny wielomian.
 * @param[in] p : wielomian
 * @return `-p`
 */
Poly PolyNeg(const Poly *p);

/**
 * Odejmuje wielomian od wielomianu.
 * @param[in] p : wielomian
 * @param[in] q : wielomian
 * @return `p - q`
 */
Poly PolySub(const Poly *p, const Poly *q);

/**
 * Zwraca stopień wielomianu ze względu na zadaną zmienną (-1 dla wielomianu
 * tożsamościowo równego zeru).
 * Zmienne indeksowane są od 0.
 * Zmienna o indeksie 0 oznacza zmienną główną tego wielomianu.
 * Większe indeksy oznaczają zmienne wielomianów znajdujących się
 * we współczynnikach.
 * @param[in] p : wielomian
 * @param[in] var_idx : indeks zmiennej
 * @return stopień wielomianu @p p z względu na zmienną o indeksie @p var_idx
 */
poly_exp_t PolyDegBy(const Poly *p, unsigned var_idx);

/**
 * Zwraca stopień wielomianu (-1 dla wielomianu tożsamościowo równego zeru).
 * @param[in] p : wielomian
 * @return stopień wielomianu @p p
 */
poly_exp_t PolyDeg(const Poly *p);

/**
 * Sprawdza równość dwóch wielomianów.
 * @param[in] p : wielomian
 * @param[in] q : wielomian
 * @return `p = q`
 */
bool PolyIsEq(const Poly *p, const Poly *q);

/**
 * Wylicza wartość wielomianu w punkcie @p x.
 * Wstawia pod pierwszą zmienną wielomianu wartość @p x.
 * W wyniku może powstać wielomian, jeśli współczynniki są wielomianem
 * i zmniejszane są indeksy zmiennych w takim wielomianie o jeden.
 * Formalnie dla wielomianu @f$p(x_0, x_1, x_2, \ldots)@f$ wynikiem jest
 * wielomian @f$p(x, x_0, x_1, \ldots)@f$.
 * @param[in] p
 * @param[in] x
 * @return @f$p(x, x_0, x_1, \ldots)@f$
 */
Poly PolyAt(const Poly *p, poly_coeff_t x);

/**
 * Wypisuje podany wielomian.
 * @param[in] p : wielomian, który chcemy wypisać
 */
void PolyToString(Poly *p);

/**
 * Wypisuje podany jednomian.
 * @param[in] m : jednomian, który chcemy wypisać
 */
void MonoToString(Mono *m);

/**
 * Podnosi podany wielomian do danej potęgi.
 * @param[in] p : wielomian który chcemy potęgować
 * @param[in] exp : potęga do której chcemy podnieść wielomian
 * @return wielomian = (p)^exp
 */
Poly PolyToPower(const Poly *p, poly_exp_t exp);

/**
 * W danym wielomianie p pod zmienną x_i tego wielomianu podstawia wielomian
 * z danej tablicy będący pod indeksem i (x[i]). Zmienne oraz tablicę
 * wielomianów indeksujemy od zera. Jeśli liczba zmiennych wielomianu p jest
 * większa od liczby podanych wielomianów, to indeksy tablicy uzupełniamy
 * wielomianami zerowymi.
 * @param[in] p : wielomian pod którego zmienne chcemy podstawić wielomiany
 * @param[in] count : liczba wielomianów do podstawienia czyli liczba elementów
 *                    w tablicy Poly x[]
 * @param[in] x : tablica wielomianów które chcemy podstawić za zmienne
 * @return wielomian po podstawieniu
 */
Poly PolyCompose(const Poly *p, unsigned count, const Poly x[]);

#endif /* __POLY_H__ */