#include "poly.h"
#include <stdio.h> /* wyrzuć po debugu */

void PolyDestroy(Poly *p) {
    List *mono_list = p->mono_list;
    
    DeleteList(mono_list);
    
    return;
}

void* MonoCopy(void *element) {
    Mono *copied_mono;
    
    copied_mono = (Mono*) malloc(sizeof(Mono));
    *copied_mono = MonoClone((Mono*) element);
    
    return (void*) copied_mono;
}

Poly PolyClone(const Poly *p) {
    Poly poly;
    
    poly.mono_list = CloneList(&MonoCopy, p->mono_list);
    poly.constant_value = p->constant_value;
    
    return poly;
}

void CheckPoly(Poly *poly) {
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
}

void AddMono(List *list, Mono *mono) {
    Mono *new_mono;
    if (PolyIsZero(&(mono->p))) {
        return;
    }
    
    new_mono = (Mono*) malloc(sizeof(Mono));
    *new_mono = MonoClone(mono);
    
    AddElement(list, (void*) new_mono);
    
    return;
}

//upewnij sie ze mozesz przepisywac wielomiany - wolna wersja
//lub zmien to
Poly PolyAdd(const Poly *p, const Poly *q) {
    Poly result, poly, p_clone;
    Mono temp, *p_mono, *q_mono;
    List *p_list, *q_list, *result_list;
    
    if (PolyIsCoeff(p) && PolyIsCoeff(q)) {
        result = PolyFromCoeff(p->constant_value + q->constant_value);
    }
    
    else if (PolyIsCoeff(p)) {
        poly = PolyZero();
        p_list = poly.mono_list;
        p_clone = PolyClone(p);
        temp = MonoFromPoly(&p_clone, 0);
        
        AddMono(p_list, &temp);//optymalizuj? dwa razy klonujesz mono
        
        if(PolyIsZero(&poly)) {
            result = *q;
        }
        else {
            result = PolyAdd(&poly, q);
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
                AddMono(result_list, p_mono);
                
                p_list = GetNext(p_list);
            }
            else if (p_mono->exp > q_mono->exp) {
                AddMono(result_list, q_mono);
                
                q_list = GetNext(q_list);
            }
            else /* (p_mono->exp == q_mono->exp) */ {
                poly = PolyAdd(&(p_mono->p), &(q_mono->p));
                
                temp = MonoFromPoly(&poly, p_mono->exp);
                
                AddMono(result_list, &temp);
                
                p_list = GetNext(p_list);
                q_list = GetNext(q_list);
            }
        }
        
        while (GetElement(p_list) != NULL) {
            p_mono = (Mono*) GetElement(p_list);
            
            AddMono(result_list, p_mono);
            
            p_list = GetNext(p_list);
        }
        
        while (GetElement(q_list) != NULL) {
            q_mono = (Mono*) GetElement(q_list);
            
            AddMono(result_list, q_mono);
            
            q_list = GetNext(q_list);
        }
    }
    
    CheckPoly(&result);
    
    return result;
}

int Cmp(const void *x, const void *y) {
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
    Poly poly, temp;
    Mono last_mono;
    
    qsort((void*) monos, count, sizeof(Mono), &Cmp);
    poly = PolyZero();
    last_mono.exp = -1;
    last_mono.p = PolyZero();
    
    for(unsigned indeks = 0; indeks < count; indeks++) {
        if (last_mono.exp != monos[indeks].exp) {
            if (last_mono.exp != -1) {
                AddMono(poly.mono_list, &last_mono);
            }
            last_mono = monos[indeks];
        }
        else {
            temp = PolyAdd(&last_mono.p, &monos[indeks].p);
            last_mono = MonoFromPoly(&temp, last_mono.exp);
        }
    }
    if (last_mono.exp != -1) {
        AddMono(poly.mono_list, &last_mono);
    }
    
    return poly;
}

Poly PolyMul(const Poly *p, const Poly *q) {
    Poly result, temp, new_poly;
    Mono *current_mono, *mono_to_mul, *new_mono;
    List *p_list, *q_list;
    
    if (PolyIsCoeff(p) && PolyIsCoeff(q)) {
        result = PolyFromCoeff(p->constant_value * q->constant_value);
    }
    
    else if (PolyIsCoeff(p)) {
        result = PolyZero();
        
        q_list = q->mono_list;
        
        while (GetElement(q_list) != NULL) {
            mono_to_mul = (Mono*) GetElement(q_list);
            
            new_mono = (Mono*) malloc(sizeof(Mono));
            
            temp = PolyMul(p, &(mono_to_mul->p));
            *new_mono = MonoFromPoly(&temp, mono_to_mul->exp);
            
            new_poly = PolyZero();
            AddMono(new_poly.mono_list, new_mono);
            
            result = PolyAdd(&result, &new_poly);
            
            PolyDestroy(&new_poly);
            
            q_list = GetNext(q_list);
        }
    }
    
    else if (PolyIsCoeff(q)) {
        result = PolyMul(q, p);
    }
    
    else {
        result = PolyZero();
        
        p_list = p->mono_list;
        q_list = q->mono_list;
        
        p_list = GetNext(p_list);
        q_list = GetNext(q_list);
        
        while (GetElement(p_list) != NULL) {
            current_mono = (Mono*) GetElement(p_list);
            
            while (GetElement(q_list) != NULL) {
                mono_to_mul = (Mono*) GetElement(q_list);
                
                new_mono = (Mono*) malloc(sizeof(Mono));
                
                temp = PolyMul(&(current_mono->p), &(mono_to_mul->p));
                *new_mono =
                    MonoFromPoly(&temp, current_mono->exp + mono_to_mul->exp);
                                
                new_poly = PolyZero();
                AddMono(new_poly.mono_list, new_mono);
                
                result = PolyAdd(&result, &new_poly);
                
                PolyDestroy(&new_poly);
                
                q_list = GetNext(q_list);
            }
            
            p_list = GetNext(q_list);
        }
    }
    
    CheckPoly(&result);
    
    return result;
}

Poly PolyNeg(const Poly *p) {
    Poly poly, result;
    
    poly = PolyFromCoeff(-1);
    
    result = PolyMul(&poly, p);
    
    PolyDestroy(&poly);
    
    return result;
}

Poly PolySub(const Poly *p, const Poly *q) {
    Poly result, neg_q;
    
    neg_q = PolyNeg(q);
    
    result = PolyAdd(p, &neg_q);
    
    PolyDestroy(&neg_q);
    
    return result;
}

poly_exp_t max(poly_exp_t x, poly_exp_t y) {
    if (x > y) {
        return x;
    }
    return y;
}

poly_exp_t PolyDegBy(const Poly *p, unsigned var_idx) {
    if (PolyIsZero(p)) {
        return -1;
    }
    
    List *list;
    Mono *mono;
    poly_exp_t result;
    
    result = 0;
    list = p->mono_list;
    list = GetNext(list);
    
    while(GetElement(list) != NULL) {
        mono = (Mono*) GetElement(list);
        
        if(var_idx > 0) {
            result = max(result, PolyDegBy(&(mono->p), var_idx - 1));
        }
        else if(var_idx == 0 && !PolyIsZero(&(mono->p))) {
            result = max(result, mono->exp);
        }
        
        list = GetNext(list);
    }
    
    return result;
}

poly_exp_t PolyDeg(const Poly *p) {
    if (PolyIsZero(p)) {
        return -1;
    }
    
    List *list;
    Mono *mono;
    poly_exp_t result;
    
    result = 0;
    list = p->mono_list;
    list = GetNext(list);
    
    while (GetElement(list) != NULL) {
        mono = (Mono*) GetElement(list);
        
        if (!PolyIsZero(&(mono->p))) {
            result = max(result, PolyDeg(&(mono->p)) + mono->exp);
        }
        
        list = GetNext(list);
    }
    
    return result;
}

bool PolyIsEq(const Poly *p, const Poly *q) {
    List *p_list, *q_list;
    Mono *p_mono, *q_mono;
    
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

poly_coeff_t FastExp(poly_coeff_t value, poly_exp_t exp) {
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
    Poly result, new_poly, temp;
    Mono *current_mono;
    List *p_list;
    poly_coeff_t value;
    
    result = PolyZero();
    
    p_list = p->mono_list;
    
    p_list = GetNext(p_list);
    
    while (GetElement(p_list) != NULL) {
        current_mono = (Mono*) GetElement(p_list);
        
        value = FastExp(x, current_mono->exp);
        
        new_poly = PolyFromCoeff(value);
        
        temp = PolyMul(&new_poly, &(current_mono->p));
        result = PolyAdd(&result, &temp);
        
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
        }
    }
    
    return;
}

void MonoToString(Mono *m) {
    printf("(");
    PolyToString(&(m->p));
    printf(")x^%d ", m->exp);
    return;
}

Mono* Test() {
    Poly poly;
    Mono *mono;
    
    poly = PolyFromCoeff(1);
    
    mono = (Mono*) malloc(sizeof(Mono));
    
    *mono = MonoFromPoly(&poly, 6);
    
    return mono;
} 

int main() {
    printf("Hello\n");
    Poly poly, poly1, poly2;
    Mono mono, mono1, mono2, mono3, mono4;
    
    printf("PolyIsCoeff Test:\n");
    poly = PolyFromCoeff(0);
    printf(PolyIsZero(&poly) ? "true\n" : "false\n");
    
    poly = PolyFromCoeff(1);
    printf(PolyIsZero(&poly) ? "true\n" : "false\n");
    printf(PolyIsCoeff(&poly) ? "true\n" : "false\n");
    
    printf("PolyAdd Test:\n");
    poly1 = PolyFromCoeff(2);
    PolyToString(&poly); printf(" + "); PolyToString(&poly1); printf(" = ");
    poly2 = PolyAdd(&poly, &poly1);
    
    PolyToString(&poly2);
    printf("\n");
    
    printf("PolyAddMonos Test:\n");
    
    mono = MonoFromPoly(&poly, 5);
    printf("mono = ");MonoToString(&mono);
    mono1 = MonoFromPoly(&poly, 6);
    printf("\nmono1 = ");MonoToString(&mono1);
    mono2 = MonoFromPoly(&poly, 7);
    printf("\nmono2 = ");MonoToString(&mono2);
    mono3 = MonoFromPoly(&poly2, 5);
    printf("\nmono3 = ");MonoToString(&mono3);
    mono4 = MonoFromPoly(&poly2, 3);
    printf("\nmono4 = ");MonoToString(&mono4);
    
    Mono tab1[5] = {mono, mono, mono, mono, mono};
    Mono tab2[1] = {mono};
    Mono tab3[3] = {mono1, mono, mono2};
    Mono tab4[5] = {mono1, mono3, mono4, mono, mono4};
    
    poly1 = PolyAddMonos(1, tab2);
    PolyToString(&poly1);
    printf("\n");
    poly1 = PolyAddMonos(5, tab1);
    PolyToString(&poly1);
    printf("\n");
    poly1 = PolyAddMonos(3, tab3);
    PolyToString(&poly1);
    printf("\n");
    
    printf("PolyAdd Testv2:\n");
    poly = PolyFromCoeff(0);
    poly1 = PolyAddMonos(3, tab3);
    poly2 = PolyAdd(&poly, &poly1);
    PolyToString(&poly2);
    printf("\n");
    
    poly = PolyFromCoeff(1);
    poly1 = PolyAddMonos(3, tab3);
    poly2 = PolyAdd(&poly, &poly1);
    PolyToString(&poly2);
    printf("\n");
    
    poly = PolyAddMonos(3, tab3);
    poly1 = PolyAddMonos(3, tab3);
    poly2 = PolyAdd(&poly, &poly1);
    PolyToString(&poly2);
    printf("\n");
    
    poly = PolyAddMonos(5, tab4);
    poly1 = PolyAddMonos(3, tab3);
    poly2 = PolyAdd(&poly, &poly1);
    PolyToString(&poly); printf(" + "); PolyToString(&poly1); printf(" = ");
    PolyToString(&poly2);
    printf("\n");
    
    
    return 0;
}

/*
valgrind --error-exitcode=15 --leak-check=full --show-leak-kinds=all --errors-for-leak-kinds=all ./poly
*/

/*
    mono = MonoFromPoly(&poly, 5);
    MonoToString(&mono);
    poly1 = PolyFromCoeff(1);
    AddMono(poly1.mono_list, &mono);
    
    list = poly1.mono_list;
    printf(IsEmpty(list) ? "true\n" : "false\n");
    
    mono1 = (Mono*) GetElement(GetNext(list));
    
    //mono2 = MonoFromPoly(&poly, 1);
    list1 = SetupList();
    AddElement(list1, (void*) &mono);
    mono2 = (Mono*) GetElement(GetNext(list1));
    
    printf("%d ", mono.exp);
    printf("%d ", mono1->exp);//mono1->exp uninitialised
    printf("%d ", mono2->exp);
    
    poly3 = PolyFromCoeff(3);
    poly3.mono_list = list1;
    poly4 = PolyClone(&poly3);
    
    printf("%ld", poly4.constant_value);
    list2 = poly4.mono_list;
    printf(IsEmpty(list2) ? "true\n" : "false\n");
    printf(GetElement(list2) == NULL ? "true\n" : "false\n");
    list2 = GetNext(list2);
    printf(GetElement(list2) == NULL ? "true\n" : "false\n");
    
    mono3 = (Mono*) GetElement(list2);
    bad_poly = &(mono3->p);//mono3->p uninitialised
    PolyToString(bad_poly);
    printf(PolyIsCoeff(bad_poly) ? "true\n" : "false\n");
    printf("%ld\n", bad_poly->constant_value);
    MonoToString(mono3);//problemem jest poly przy tym mono
    PolyToString(&poly4);
     
    MonoToString(mono1);
    
    poly2 = &(mono1->p);
    PolyToString(poly2);
    
    printf(PolyIsCoeff(&poly1) ? "true\n" : "false\n");
*/