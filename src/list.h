#ifndef LIST_H
#define LIST_H

/* this struct represents list element with pointers
 * to previous and next element */
typedef struct List {
    struct List *previous;
    struct List *next;
    int label;
} List;
typedef enum { false, true } bool;

extern bool checkIfNullPointer(List *list);

extern int getLabel(List *list);

extern List* getPrevious(List *list);

extern List* getNext(List *list);

extern List* createListElement(int label);

extern void setConnection(List *firstList, List *secondList);

extern void deleteListElement(List *list);

#endif /* LIST_H */