/** @file
   Implementacja klasy wielomianów

   @author Jakub Wróblewski <jw386401@students.mimuw.edu.pl>
   @date 2017-05-13
*/
#include <assert.h>
#include <stdio.h>
#include "poly.h"

/**
 * Funkcja symetryczna do MonoDestroy, ale na potrzeby implementacji list
 * przyjmuje voidowy wskażnik.
 * @param[in] element : element listy (jednomian)
 */
static void MonoDestroyVoid(void *element) {
    MonoDestroy((Mono*) element);
    
    return;
}

void PolyDestroy(Poly *p) {
    DeleteList(&MonoDestroyVoid, p->mono_list);
    
    return;
}

/**
 * Alokuje pamięc dla wskaźnika na podany jednomian.
 * @param[in] m : jednomian
 * @return Wskaźnik na podany jednomian
 */
static Mono* MallocMono(Mono m) {
    Mono *mono;
    
    mono = (Mono*) malloc(sizeof(Mono));
    assert(mono != NULL);
    *mono = m;
    mono->is_allocated = true;
    
    return mono;
}

/**
 * Funkcja symetryczna do MonoClone, ale na potrzeby implementacji list
 * przyjmuje i zwraca voidowy wskażnik.
 * @param[in] element : element listy (jednomian)
 * @return kopia elementu
 */
static void* MonoCopy(void *element) {
    Mono *copied_mono;
     
    copied_mono = MallocMono(MonoClone((Mono*) element));
    
    return (void*) copied_mono;
}

Poly PolyClone(const Poly *p) {
    Poly copied_poly;
    
    copied_poly.mono_list = CloneList(&MonoCopy, p->mono_list);
    copied_poly.constant_value = p->constant_value;
    
    return copied_poly;
}

/**
 * Sprawdz czy dany wielomian nie jest postaci (coeff)x^0. jeśli jest to
 * modyfikuje go do postaci coeff
 * @param[in] poly : wielomian
 */
static void CheckPoly(Poly *poly) {
    Mono *last_mono;
    List *list;
    poly_coeff_t constant_value;
    
    list = poly->mono_list;
    last_mono = (Mono*) GetElement(GetPrevious(list));
    
    if (last_mono != NULL) {
        if (last_mono->exp == 0 && PolyIsCoeff(&(last_mono->p))) {
            constant_value = last_mono->p.constant_value;
            
            PolyDestroy(poly);
            *poly = PolyFromCoeff(constant_value);
        }
    }
    
    return;
}

/**
 * Dodaje dany jednomian (lub jego kopię) na koniec podanej listy.
 * @param[in] list : lista do której chcemy dodać jednomian
 * @param[in] mono : jednomian
 * @param[in] clone : czy skopiować dany jednomian przed dodaniem?
 */
static void AddMono(List *list, Mono *mono, bool clone) {
    Mono *new_mono;
    
    if (PolyIsZero(&(mono->p))) {
        MonoDestroy(mono);
        return;
    }
    if (clone == true) {
        new_mono = MallocMono(MonoClone(mono));
    }
    else {
        new_mono = mono;
    }
    
    AddElement(list, (void*) new_mono);
    
    return;
}

Poly PolyAdd(const Poly *p, const Poly *q) {
    Poly result, poly_sum, p_clone;
    Mono *p_mono, *q_mono, *new_mono, *q_first_mono;
    List *p_list, *q_list, *result_list;
    
    if (PolyIsCoeff(p) && PolyIsCoeff(q)) {
        result = PolyFromCoeff(p->constant_value + q->constant_value);
    }
    
    else if (PolyIsCoeff(p)) {
        if (PolyIsZero(p)) {
            result = PolyClone(q);
        }
        else {
            q_first_mono = (Mono*) GetElement(GetNext(q->mono_list));
            
            if (q_first_mono->exp == 0) {
                poly_sum = PolyAdd(p, &(q_first_mono->p));
                new_mono = MallocMono(MonoFromPoly(&poly_sum, 0));
                
                result = PolyZero();
                result_list = result.mono_list;
                q_list = q->mono_list;
                q_list = GetNext(q_list);
                
                while (GetElement(q_list) != NULL) {
                    q_mono = (Mono*) GetElement(q_list);
                    
                    if (q_mono->exp == 0) {
                        if (PolyIsZero(&poly_sum)) {
                            MonoDestroy(new_mono);
                        }
                        else {
                            AddMono(result_list, new_mono, false);
                        }
                    }
                    else {
                        AddMono(result_list, q_mono, true);
                    }
                    
                    q_list = GetNext(q_list);
                }
                
                CheckPoly(&result);
            }
            else {
                p_clone = PolyFromCoeff(p->constant_value);
                
                new_mono = MallocMono(MonoFromPoly(&p_clone, 0));
                
                result = PolyClone(q);
                /* W ten sposób zamiast dodać element jako ostatni na liście,
                 * dodam go jako pierwszy */
                AddMono(GetNext(result.mono_list), new_mono, false);
            }
        }
    }
    
    else if (PolyIsCoeff(q)) {
        result = PolyAdd(q, p);
    }
    
    else {
        result = PolyZero();
        result_list = result.mono_list;
        
        p_list = p->mono_list;
        q_list = q->mono_list;
        p_list = GetNext(p_list);
        q_list = GetNext(q_list);
        
        while (GetElement(p_list) != NULL && GetElement(q_list) != NULL) {
            p_mono = (Mono*) GetElement(p_list);
            q_mono = (Mono*) GetElement(q_list);
            
            if (p_mono->exp < q_mono->exp) {
                AddMono(result_list, p_mono, true);
                
                p_list = GetNext(p_list);
            }
            else if (p_mono->exp > q_mono->exp) {
                AddMono(result_list, q_mono, true);
                
                q_list = GetNext(q_list);
            }
            else /* (p_mono->exp == q_mono->exp) */ {
                poly_sum = PolyAdd(&(p_mono->p), &(q_mono->p));
                
                if (!PolyIsZero(&poly_sum)) {
                    new_mono = MallocMono(
                        MonoFromPoly(&poly_sum, p_mono->exp));
                    AddMono(result_list, new_mono, false);
                }
                else {
                    PolyDestroy(&poly_sum);
                }
                
                p_list = GetNext(p_list);
                q_list = GetNext(q_list);
            }
        }
        
        while (GetElement(p_list) != NULL) {
            p_mono = (Mono*) GetElement(p_list);
            
            AddMono(result_list, p_mono, true);
            
            p_list = GetNext(p_list);
        }
        
        while (GetElement(q_list) != NULL) {
            q_mono = (Mono*) GetElement(q_list);
            
            AddMono(result_list, q_mono, true);
            
            q_list = GetNext(q_list);
        }
        
        CheckPoly(&result);
    }
    
    return result;
}

static int Cmp(const void *x, const void *y) {
    Mono *first_mono, *second_mono;
    
    first_mono = (Mono*) x;
    second_mono = (Mono*) y;
    
    if (first_mono->exp > second_mono->exp) {
        return 1;
    }
    else if (first_mono->exp == second_mono->exp) {
        return 0;
    }
    else /* (first_mono->exp < second_mono->exp) */ {
        return -1;
    }
}

Poly PolyAddMonos(unsigned count, const Mono monos[]) {
    Poly result, poly_from_monos, poly_from_next_mono, sum;
    Mono *mono_to_add;
    poly_exp_t monos_exp;
    
    qsort((void*) monos, count, sizeof(Mono), &Cmp);
    
    result = PolyZero();
    
    for(unsigned indeks = 0; indeks < count; indeks++) {
        poly_from_monos = monos[indeks].p;
        monos_exp = monos[indeks].exp;
        
        while (indeks != count - 1 &&
            monos[indeks].exp == monos[indeks + 1].exp) {
            
            poly_from_next_mono = monos[indeks + 1].p;
            sum = PolyAdd(&poly_from_monos, &poly_from_next_mono);
            
            PolyDestroy(&poly_from_monos);
            PolyDestroy(&poly_from_next_mono);
            
            poly_from_monos = sum;
            
            indeks++;
        }
        
        mono_to_add = MallocMono(MonoFromPoly(&poly_from_monos, monos_exp));
        
        AddMono(result.mono_list, mono_to_add, false);
    }

    CheckPoly(&result);
    
    return result;
}

Poly PolyMul(const Poly *p, const Poly *q) {
    Poly result, new_poly;
    Mono *current_mono, *mono_to_mul, *new_mono, *mono_from_queue;
    List *p_list, *q_list, *queue;
    unsigned monos_count = 0;
    
    if (PolyIsCoeff(p) && PolyIsCoeff(q)) {
        result = PolyFromCoeff(p->constant_value * q->constant_value);
    }
    
    else if (PolyIsCoeff(p)) {
        queue = SetupList();
        q_list = q->mono_list;
        q_list = GetNext(q_list);
        
        while (GetElement(q_list) != NULL) {
            mono_to_mul = (Mono*) GetElement(q_list);
            
            new_poly = PolyMul(p, &(mono_to_mul->p));
            
            if (!PolyIsZero(&new_poly)) {
                new_mono = MallocMono(
                    MonoFromPoly(&new_poly, mono_to_mul->exp));
                
                AddMono(queue, new_mono, false);
                
                monos_count++;
            }
            else {
                PolyDestroy(&new_poly);
            }
            
            q_list = GetNext(q_list);
        }
        
        Mono *monos = calloc(monos_count, sizeof(Mono));
        if (monos_count != 0) {
            assert(monos != NULL);
        }
        
        queue = GetNext(queue);
        for (unsigned indeks = 0; indeks < monos_count; indeks++) {
            mono_from_queue = (Mono*) GetElement(queue);
            monos[indeks] = *mono_from_queue;
            
            queue = GetNext(queue);
        }
        result = PolyAddMonos(monos_count, monos);
        
        DeleteList(&free, queue);
        free(monos);
    }
    
    else if (PolyIsCoeff(q)) {
        result = PolyMul(q, p);
    }
    
    else {
        queue = SetupList();
        p_list = p->mono_list;
        q_list = q->mono_list;
        
        p_list = GetNext(p_list);
        q_list = GetNext(q_list);
        
        while (GetElement(p_list) != NULL) {
            current_mono = (Mono*) GetElement(p_list);
            
            while (GetElement(q_list) != NULL) {
                mono_to_mul = (Mono*) GetElement(q_list);
                
                new_poly = PolyMul(&(current_mono->p), &(mono_to_mul->p));
                
                if (!PolyIsZero(&new_poly)) {
                    new_mono = MallocMono(MonoFromPoly(
                        &new_poly, current_mono->exp + mono_to_mul->exp));
                    
                    AddMono(queue, new_mono, false);
                    
                    monos_count++;
                }
                else {
                    PolyDestroy(&new_poly);
                }
                
                q_list = GetNext(q_list);
            }
            
            p_list = GetNext(p_list);
            q_list = GetNext(q_list);
        }
        
        Mono *monos = calloc(monos_count, sizeof(Mono));
        if (monos_count != 0) {
            assert(monos != NULL);
        }
        
        queue = GetNext(queue);
        for (unsigned indeks = 0; indeks < monos_count; indeks++) {
            mono_from_queue = (Mono*) GetElement(queue);
            monos[indeks] = *mono_from_queue;
            
            queue = GetNext(queue);
        }
        result = PolyAddMonos(monos_count, monos);
        
        DeleteList(&free, queue);
        free(monos);
    }
    
    return result;
}

Poly PolyNeg(const Poly *p) {
    Poly temp, result;
    
    temp = PolyFromCoeff(-1);
    
    result = PolyMul(&temp, p);
    
    PolyDestroy(&temp);
    
    return result;
}

Poly PolySub(const Poly *p, const Poly *q) {
    Poly result, neg_q;
    
    neg_q = PolyNeg(q);
    
    result = PolyAdd(p, &neg_q);
    
    PolyDestroy(&neg_q);
    
    return result;
}

static poly_exp_t max(poly_exp_t x, poly_exp_t y) {
    if (x > y) {
        return x;
    }
    return y;
}

poly_exp_t PolyDegBy(const Poly *p, unsigned var_idx) {
    List *list;
    Mono *current_mono;
    poly_exp_t result;

    if (PolyIsZero(p)) {
        return -1;
    }    
    
    result = 0;
    list = p->mono_list;
    list = GetNext(list);
    
    while(GetElement(list) != NULL) {
        current_mono = (Mono*) GetElement(list);
        
        if(var_idx > 0) {
            result = max(result, PolyDegBy(&(current_mono->p), var_idx - 1));
        }
        else if(var_idx == 0 && !PolyIsZero(&(current_mono->p))) {
            result = max(result, current_mono->exp);
        }
        
        list = GetNext(list);
    }
    
    return result;
}

poly_exp_t PolyDeg(const Poly *p) {
    List *list;
    Mono *current_mono;
    poly_exp_t result;
    
    if (PolyIsZero(p)) {
        return -1;
    }
    
    result = 0;
    list = p->mono_list;
    list = GetNext(list);
    
    while (GetElement(list) != NULL) {
        current_mono = (Mono*) GetElement(list);
        
        if (!PolyIsZero(&(current_mono->p))) {
            result = max(result,
                         PolyDeg(&(current_mono->p))+ current_mono->exp);
        }
        
        list = GetNext(list);
    }
    
    return result;
}

bool PolyIsEq(const Poly *p, const Poly *q) {
    List *p_list, *q_list;
    Mono *p_mono, *q_mono;
    
    if(PolyIsCoeff(p) || PolyIsCoeff(q)) {
        if((PolyIsCoeff(p) && PolyIsCoeff(q)) &&
            p->constant_value == q->constant_value) {
            return true;
        }
        else {
            return false;
        }
    }
    
    p_list = p->mono_list;
    q_list = q->mono_list;
    p_list = GetNext(p_list);
    q_list = GetNext(q_list);
    
    while (GetElement(p_list) != NULL && GetElement(q_list) != NULL) {
        p_mono = (Mono*) GetElement(p_list);
        q_mono = (Mono*) GetElement(q_list);
        
        if (p_mono->exp != q_mono->exp ||
            !PolyIsEq(&(p_mono->p), &(q_mono->p))) {
            return false;
        }
        
        p_list = GetNext(p_list);
        q_list = GetNext(q_list);
    }
    
    if (GetElement(p_list) != NULL || GetElement(q_list) != NULL) {
        return false;
    }
    
    return true;
}

/**
 * Podnosi liczbę do danej potęgi.
 * @param[in] value : liczba którą chcemy potęgować
 * @param[in] exp : potęga do której chcemy podnieść liczbę
 * @return wartość value^exp
 */
static poly_coeff_t FastExp(poly_coeff_t value, poly_exp_t exp) {
    poly_coeff_t x;
    
    if (exp == 0) {
        return 1;
    }
    
    else if (exp % 2 == 1) {
        x = FastExp(value, (exp - 1) / 2);
        
        return value * x * x;
    }
    
    else /* (exp % 2 == 0) */ {
        x = FastExp(value, exp / 2);
        
        return x * x;
    }
}

Poly PolyAt(const Poly *p, poly_coeff_t x) {
    Poly result, sum, new_poly, temp;
    Mono *current_mono;
    List *p_list;
    poly_coeff_t value;
    
    result = PolyZero();
    result.constant_value = p->constant_value;
    
    p_list = p->mono_list;
    p_list = GetNext(p_list);
    
    while (GetElement(p_list) != NULL) {
        current_mono = (Mono*) GetElement(p_list);
        
        value = FastExp(x, current_mono->exp);
        new_poly = PolyFromCoeff(value);
        temp = PolyMul(&new_poly, &(current_mono->p));
        
        sum = PolyAdd(&result, &temp);
        PolyDestroy(&result);
        result = sum;
        
        PolyDestroy(&new_poly);
        PolyDestroy(&temp);
        
        p_list = GetNext(p_list);
    }
    
    CheckPoly(&result);
    
    return result;
}

void PolyToString(Poly *p) {
    Mono *mono;
    List *list;
    
    if(PolyIsCoeff(p)) {
        printf("%ld", p->constant_value);
    }
    else {
        list = p->mono_list;
        
        list = GetNext(list);
        
        while (GetElement(list) != NULL) {
            mono = (Mono*) GetElement(list);
            
            MonoToString(mono);
            
            list = GetNext(list);
            
            if (GetElement(list) != NULL) {
                printf("+");
            }
        }
    }
    
    return;
}

void MonoToString(Mono *m) {
    printf("(");
    PolyToString(&(m->p));
    printf(",%d)", m->exp);
    return;
}

/**
 * Podnosi podany wielomian do danej potęgi.
 * @param[in] p : wielomian który chcemy potęgować
 * @param[in] exp : potęga do której chcemy podnieść wielomian
 * @return wielomian = (p)^exp
 */
Poly PolyToPower(const Poly *p, poly_exp_t exp) {
    Poly result, poly, temp;
    
    if (exp == 0) {
        return PolyFromCoeff(1);
    }
    
    poly = PolyToPower(p, exp / 2);
    
    result = PolyMul(&poly, &poly);/* upewnij się że PolyMul nie ma problemów
                                    z mnożeniem tego samego poly */
    if (exp % 2 == 1) {
        temp = PolyMul(&result, p);
        PolyDestroy(&result);
        result = temp;
    }
    
    PolyDestroy(&poly);
    
    return result;
}

Poly PolyCompose(const Poly *p, unsigned count, const Poly x[]) {
    Poly result, composed_poly, temp, exp_poly, new_poly;
    Mono *current_mono;
    List *p_list;
    
    if (PolyIsCoeff(p)) {
        return PolyFromCoeff(p->constant_value);
    }
    
    result = PolyZero();
    
    p_list = p->mono_list;
    p_list = GetNext(p_list);
    
    while (GetElement(p_list) != NULL) {
        current_mono = (Mono*) GetElement(p_list);
        
        if (count == 0 && current_mono->exp == 0) {
            /* jeśli funkcja PolyCompose przyjmie count = 0, to będziemy w niej
             * rekurencyjnie wywoływać PolyCompose tylko z count = 0.
             * PolyCompose gdzie count = 0, nie odwołuje się w żaden sposób
             * do tablicy x, jest ona nie istotna, dlatego zamiast tworzyć
             * tablicę z zerowym wielomianem lub wielomianami podaję NULL */
            composed_poly = PolyCompose(&current_mono->p, 0, NULL);
            
            temp = PolyAdd(&result, &composed_poly);
            
            PolyDestroy(&result);
            PolyDestroy(&composed_poly);
            
            result = temp;
        }
        else if (count != 0) {
            composed_poly = PolyCompose(&current_mono->p, count - 1, x + 1);

            exp_poly = PolyToPower(&(x[0]), current_mono->exp);
            
            new_poly = PolyMul(&composed_poly, &exp_poly);
            
            temp = PolyAdd(&result, &new_poly);
            
            PolyDestroy(&result);
            PolyDestroy(&composed_poly);
            PolyDestroy(&exp_poly);
            PolyDestroy(&new_poly);
            
            result = temp;
        }
        
        p_list = GetNext(p_list);
    }
    
    return result;
}