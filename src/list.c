#include<stdlib.h>
#include<stdio.h>
#include"list.h"

bool checkIfNullPointer(List *list) {
    if (list->previous == NULL || list->next == NULL) {
        printf("List element contains NULL pointer\n");
        return true;
    }
    return false;
}

int getLabel(List *list) {
    return list->label;
}

List* getPrevious(List *list) {
    return list->previous;
}

List* getNext(List *list) {
    return list->next;
}

List* createListElement(int label) {
    List *list;
    list = (List*) malloc(sizeof(List));
    
    list->previous = list;
    list->next = list;
    list->label = label;
    
    return list;
}

void setConnection(List *firstList, List *secondList) {
    firstList->next = secondList;
    secondList->previous = firstList;
    return;
}

void deleteListElement(List *list) {
    free(list);
    return;
}