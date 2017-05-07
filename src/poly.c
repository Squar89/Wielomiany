#include "poly.h"
#include <stdio.h> /* wyrzuć po debugu */

void PolyDestroy(Poly *p) {
    List *mono_list = p->mono_list;
    if (mono_list != NULL) {
        DeleteList(mono_list);
    }
    /* TODO upewnij się że nie musisz uwalniać NULL pointera */
    
    free(p);
    return;
}

Poly PolyClone(const Poly *p) {
    Poly *poly;
    
    poly = (Poly*) malloc(sizeof(Poly));
    poly->mono_list = CloneList(p->mono_list);
    poly->constant_value = p->constant_value;
    
    /* TODO */ return *poly;
}

Poly PolyAdd(const Poly *p, const Poly *q) {
    Poly poly;
    
    /* TODO */ return poly;
}

Poly PolyAddMonos(unsigned count, const Mono monos[]) {
    Poly poly;
    
    /* TODO */ return poly;
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
    
    /* TODO */ return var_idx;
}

poly_exp_t PolyDeg(const Poly *p) {
    
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
    /*
    Poly p;
    p = PolyFromCoeff(5);
    
    printf("test\n");
    //printf("%l", p.constant_value);
    */
    
    return 0;
}