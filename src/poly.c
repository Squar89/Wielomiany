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

Poly PolyAdd(const Poly *p, const Poly *q) {
    Poly poly;
    
    /* TODO */ return poly;
}

int Cmp(const void *x, const void *y) {
    Mono *first_mono, *second_mono;
    
    first_mono = (Mono*) x;
    second_mono = (Mono*) y;
    
    if (first_mono > second_mono) {
        return 1;
    }
    if (first_mono == second_mono) {
        return 0;
    }
    if (first_mono < second_mono) {
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
            if (last_mono.exp != -1 || !PolyIsZero(&last_mono.p)) {
                AddElement(poly.mono_list, (void*) &last_mono);
            }
            last_mono = monos[indeks];
        }
        else {
            temp = PolyAdd(&last_mono.p, &monos[indeks].p);
            last_mono = MonoFromPoly(&temp, last_mono.exp);
        }
    }
    if (last_mono.exp != -1 || !PolyIsZero(&last_mono.p)) {
        AddElement(poly.mono_list, (void*) &last_mono);
    }
    
    return poly;
}

Poly PolyMul(const Poly *p, const Poly *q) {
    Poly poly;
    
    /* TODO */ return poly;
}

Poly PolyNeg(const Poly *p) {
    Poly poly;
    
    /* TODO */ return poly;
}

Poly PolySub(const Poly *p, const Poly *q) {
    Poly poly;
    
    /* TODO */ return poly;
}

poly_exp_t PolyDegBy(const Poly *p, unsigned var_idx) {
    if (PolyIsZero(p)) {
        return -1;
    }
    
    //wchodzi na odpowiedni poziom i sumuje exp
    /* TODO */ return var_idx;
}

poly_exp_t PolyDeg(const Poly *p) {
    //max + własny i  na kazdym poziomie
    /* TODO */ return 0;
}

bool PolyIsEq(const Poly *p, const Poly *q) {
    
    /* TODO */ return false;
}

Poly PolyAt(const Poly *p, poly_coeff_t x) {
    Poly poly;
    
    /* TODO */ return poly;
}

int main() {
    
    return 0;
}