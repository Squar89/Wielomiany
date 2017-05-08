#include "poly.h"
#include <stdio.h> /* wyrzuć po debugu */

void PolyDestroy(Poly *p) {
    List *mono_list = p->mono_list;
    
    if (!IsEmpty(mono_list)) {
        DeleteList(mono_list);
    }
    
    return;
}

void* MonoCopy(void *mono) {
    Mono *copied_mono, temp;
    
    copied_mono = (Mono*) malloc(sizeof(Mono));
    temp = MonoClone(mono);
    copied_mono = &temp;
    
    return (void*) copied_mono;
}

Poly PolyClone(const Poly *p) {
    Poly poly;
    
    poly.mono_list = CloneList(&MonoCopy, p->mono_list);
    poly.constant_value = p->constant_value;
    
    return poly;
}

void CheckPoly(Poly *poly) {
    Poly temp;
    poly_coeff_t constant_value;
    List *list;
    Mono *last_mono;
    
    list = poly->mono_list;
    last_mono =/* (mono*) */GetElement(GetPrevious(list));
    
    if (last_mono != NULL) {
        if (last_mono->exp == 0 && PolyIsCoeff(&(last_mono->p))) {
            constant_value = last_mono->p.constant_value;
            
            PolyDestroy(poly);
            
            temp = PolyFromCoeff(constant_value);
            
            poly = &temp;
        }
    }
}

void AddMono(List *list, Mono *mono) {
    Mono temp, *new_mono;
    
    if (PolyIsZero(&(mono->p))) {
        return;
    }
    
    new_mono = (Mono*) malloc(sizeof(Mono));
    
    temp = MonoClone(mono);
    new_mono = &temp;
    
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
        
        AddMono(p_list, &temp);
        
        result = PolyAdd(&poly, q);
    }
    
    else if (PolyIsCoeff(q)) {
        result = PolyAdd(q, p);
    }
    
    else {
        result = PolyZero();
        result_list = result.mono_list;
        
        while (GetElement(p_list) != NULL && GetElement(q_list) != NULL) {
            p_mono = GetElement(p_list);
            q_mono = GetElement(q_list);
            
            if (p_mono->exp < q_mono->exp) {
                AddMono(result_list, p_mono);
                
                p_list = GetNext(p_list);
            }
            else if (p_mono->exp > q_mono->exp) {
                AddMono(result_list, q_mono);
                
                q_list = GetNext(q_list);
            }
            else // (p_mono->exp == q_mono->exp) {
                poly = PolyAdd(&(p_mono->p), &(q_mono->p));
                
                temp = MonoFromPoly(&poly, p_mono->exp);
                
                AddMono(result_list, &temp);
                
                p_list = GetNext(p_list);
                q_list = GetNext(q_list);
            }
        }
        
        while (GetElement(p_list) != NULL) {
            p_mono = GetElement(p_list);
            
            AddMono(result_list, p_mono);
            
            p_list = GetNext(p_list);
        }
        
        while (GetElement(q_list) != NULL) {
            q_mono = GetElement(q_list);
            
            AddMono(result_list, q_mono);
            
            q_list = GetNext(q_list);
        }
    
    CheckPoly(&result);
    
    return result;
}

int Cmp(const void *x, const void *y) {
    Mono *first_mono, *second_mono;
    
    first_mono = (Mono*) x;
    second_mono = (Mono*) y;
    
    if (first_mono > second_mono) {
        return 1;
    }
    else if (first_mono == second_mono) {
        return 0;
    }
    else /* (first_mono < second_mono) */ {
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
    Mono *current_mono, *mono_to_mul, *new_mono, temp_mono;
    List *p_list, *q_list;
    
    if (PolyIsCoeff(p) && PolyIsCoeff(q)) {
        result = PolyFromCoeff(p->constant_value * q->constant_value);
    }
    
    else if (PolyIsCoeff(p)) {
        result = PolyZero();
        
        q_list = q->mono_list;
        
        while (GetElement(q_list) != NULL) {
            mono_to_mul = GetElement(q_list);
            
            new_mono = (Mono*) malloc(sizeof(Mono));
            
            temp = PolyMul(p, &(mono_to_mul->p));
            temp_mono = MonoFromPoly(&temp, mono_to_mul->exp);
            new_mono = &temp_mono;
            
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
            current_mono = GetElement(p_list);
            
            while (GetElement(q_list) != NULL) {
                mono_to_mul = GetElement(q_list);
                
                new_mono = (Mono*) malloc(sizeof(Mono));
                
                temp = PolyMul(&(current_mono->p), &(mono_to_mul->p));
                temp_mono =
                    MonoFromPoly(&temp, current_mono->exp + mono_to_mul->exp);
                new_mono = &temp_mono;
                
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
        mono = GetElement(list);
        
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
        mono = GetElement(list);
        
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
        p_mono = GetElement(p_list);
        q_mono = GetElement(q_list);
        
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

Poly PolyAt(const Poly *p, poly_coeff_t x) {
    Poly poly;
    
    /* TODO */ return poly;
}

int main() {
    
    return 0;
}