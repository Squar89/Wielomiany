/** @file
   Interfejs klasy wielomianów

   @author Jakub Pawlewicz <pan@mimuw.edu.pl>,
           Jakub Wróblewski <jw386401@students.mimuw.edu.pl>
   @copyright Uniwersytet Warszawski
   @date 2017-04-09, 2017-05-06 TODO
*/

#ifndef __POLY_H__
#define __POLY_H__

#include <stdbool.h>
#include <stddef.h>
#include "list.h"

/** Typ współczynników wielomianu */
typedef long poly_coeff_t;

/** Typ wykładników wielomianu */
typedef int poly_exp_t;

/**
 * Struktura przechowująca wielomian
 * TODO
 */
typedef struct Poly
{
    List *mono_list; ///< lista wskaźników na jednomiany wielomianu
    poly_coeff_t constant_value; ///< wartość dla stałego wielomianu
    bool is_coeff; ///< bool stwierdzający czy wielomian jest współczynnikiem
    /* TODO upewnij się czy nie musisz trzymać wskaźnika na mono wyżej */
} Poly;

/**
  * Struktura przechowująca jednomian
  * Jednomian ma postać `p * x^e`.
  * Współczynnik `p` może też być wielomianem.
  * Będzie on traktowany jako wielomian nad kolejną zmienną (nie nad x).
  */
typedef struct Mono
{
    Poly p; ///< współczynnik
    poly_exp_t exp; ///< wykładnik
} Mono;

/**
 * Tworzy wielomian, który jest współczynnikiem.
 * @param[in] c : wartość współczynnika
 * @return wielomian
 */
static inline Poly PolyFromCoeff(poly_coeff_t c) {
    /* TODO */
    //poly z pusta lista i wartoscia rowna c
}

/**
 * Tworzy wielomian tożsamościowo równy zeru.
 * @return wielomian
 */
static inline Poly PolyZero() {
    /* TODO */
    //return PolyFromCoeff(0); 
}

/**
 * Tworzy jednomian `p * x^e`.
 * Tworzony jednomian przejmuje na własność (kopiuje) wielomian @p p.
 * @param[in] p : wielomian - współczynnik jednomianu
 * @param[in] e : wykładnik
 * @return jednomian `p * x^e`
 */
static inline Mono MonoFromPoly(Poly *p, poly_exp_t e) {
    return (Mono) {.p = *p, .exp = e};
}

/**
 * Sprawdza, czy wielomian jest współczynnikiem.
 * @param[in] p : wielomian
 * @return Czy wielomian jest współczynnikiem?
 */
static inline bool PolyIsCoeff(const Poly *p) {
    /* TODO */
    //w poly trzymamy wskaznik do mono lub null jesli jest tym zewnetrznym
}

/**
 * Sprawdza, czy wielomian jest tożsamościowo równy zeru.
 * @param[in] p : wielomian
 * @return Czy wielomian jest równy zero?
 */
static inline bool PolyIsZero(const Poly *p) {
    /* TODO */
    //wchodzi rekurencyjnie i sprawdza caly, ale dla przyspieszenia zawsze gdy
    //wielomian jest rowny zero to go usuwam(uwazaj zeby wszedzie to
    //kontrolowac)
}

/**
 * Usuwa wielomian z pamięci.
 * @param[in] p : wielomian
 */
void PolyDestroy(Poly *p);
//wchodzisz rekurencyjnie i niszczysz kolejne oba poly i mono

/**
 * Usuwa jednomian z pamięci.
 * @param[in] m : jednomian
 */
static inline void MonoDestroy(Mono *m) {
    /* TODO */
}

/**
 * Robi pełną kopię wielomianu.
 * @param[in] p : wielomian
 * @return skopiowany wielomian
 */
Poly PolyClone(const Poly *p);

/**
 * Robi pełną kopię jednomianu.
 * @param[in] m : jednomian
 * @return skopiowany jednomian
 */
static inline Mono MonoClone(const Mono *m) {
    /* TODO */
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
 * @param[in] count : liczba jednomianów
 * @param[in] monos : tablica jednomianów
 * @return wielomian będący sumą jednomianów
 */
Poly PolyAddMonos(unsigned count, const Mono monos[]);
//sortuje tablice i dodaje wspolczynniki mono ktore maja te same wykladniki

/**
 * Mnoży dwa wielomiany.
 * @param[in] p : wielomian
 * @param[in] q : wielomian
 * @return `p * q`
 */
Poly PolyMul(const Poly *p, const Poly *q);
//po prostu rekurencyjnie mnozysz wspolczynniki i dodajesz wykladniki
//na koniec mnoze razy stała

/**
 * Zwraca przeciwny wielomian.
 * @param[in] p : wielomian
 * @return `-p`
 */
Poly PolyNeg(const Poly *p);
//pomnoz przez wielomian stały rowny -1

/**
 * Odejmuje wielomian od wielomianu.
 * @param[in] p : wielomian
 * @param[in] q : wielomian
 * @return `p - q`
 */
Poly PolySub(const Poly *p, const Poly *q);
//dodanie negatywnego wielomianu

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
//rozpatruje dla kazdego mono
//wchodze odpowiednio gleboko i znajduje stopien wielomianu w dany sposob:
//st mono = suma stopni jego wspolczynnika + jego stopien
//st poly = max ze stopni mono

/**
 * Zwraca stopień wielomianu (-1 dla wielomianu tożsamościowo równego zeru).
 * @param[in] p : wielomian
 * @return stopień wielomianu @p p
 */
poly_exp_t PolyDeg(const Poly *p);
//PolyDegBy(p, 0);

/**
 * Sprawdza równość dwóch wielomianów.
 * @param[in] p : wielomian
 * @param[in] q : wielomian
 * @return `p = q`
 */
bool PolyIsEq(const Poly *p, const Poly *q);
//pilnuj sortowania i wywalania wielomianow rownych zero

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
//szybkie potegowanie i nic specjalnego

#endif /* __POLY_H__ */

/*
Stosowane konwencje w tym projekcie:

nazwy funkcji oraz typów - konwencja trzecia (camelcase z dużej litery),
nazwy stałych - konwencja druga (duże litery i podkreślnik),
nazwy zmiennych - konwencja pierwsza (małe litery i podkreślnik),
nazwy typów liczbowych - konwencja pierwsza (małe litery i podkreślnik) + _t na końcu identyfikatora.
*/