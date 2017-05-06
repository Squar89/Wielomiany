#include <stdlib.h>
#include <stdio.h>
#include "list.h"

bool CheckIfNullPointer(List *list) {
    if (list->previous == NULL || list->next == NULL) {
        printf("List element contains NULL pointer\n");
        return true;
    }
    return false;
}

void* GetElement(List *list) {
    return list->element;
}

List* GetPrevious(List *list) {
    return list->previous;
}

List* GetNext(List *list) {
    return list->next;
}

List* CreateListElement(void* element) {
    List *list;
    list = (List*) malloc(sizeof(List));
    
    list->previous = list;
    list->next = list;
    list->element = element;
    
    return list;
}

void SetConnection(List *firstList, List *secondList) {
    firstList->next = secondList;
    secondList->previous = firstList;
    return;
}

void DeleteListElement(List *list) {
    free(list);
    return;
}