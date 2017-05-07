#ifndef LIST_H
#define LIST_H

#include <stdbool.h>

/* this struct represents list element with pointers
 * to previous and next element */

typedef struct List {
    struct List *previous;
    struct List *next;
    void *element;
} List;

extern bool CheckIfNullPointer(List *list);

extern void* GetElement(List *list);

extern List* GetPrevious(List *list);

extern List* GetNext(List *list);

extern List* CreateListElement(void* element);

extern void SetConnection(List *firstList, List *secondList);

extern void AddElement(List *list, void *element);

extern List* SetupList();

extern void DeleteListElement(List *list);

extern void DeleteList(List *list);

extern List* CloneList(void* (*Clone)(void*), List *list_in);

extern bool IsEmpty(List *list);

#endif /* LIST_H */