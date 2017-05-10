#include <stdlib.h>
#include <stdio.h>
#include "list.h"
#include <assert.h>

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

List* CreateListElement(void *element) {
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

void AddElement(List *list, void *element) {
    List* new_list_element;
    
    new_list_element = CreateListElement(element);
    
    SetConnection(GetPrevious(list), new_list_element);
    SetConnection(new_list_element, list);
    
    return;
}

List* SetupList() {
    List *guardian;
    
    guardian = CreateListElement(NULL);
    
    return guardian;
}

void DeleteListElement(void (*Delete)(void*), List *list) {
    if (GetElement(list) != NULL) {
        Delete(GetElement(list));
    }

    free(list);
    
    return;
}

void DeleteList(void (*Delete)(void*), List *list) {
    if (GetElement(GetNext(list)) != NULL) {
        DeleteList(Delete, GetNext(list));
    }
    DeleteListElement(Delete, list);
    
    return;
}

List* CloneList(void* (*Clone)(void*), List *list_in) {
    if(list_in == NULL) return NULL;

    List *list;
    void *copy;
    
    list = SetupList();
    
    list_in = GetNext(list_in);
    
    while (GetElement(list_in) != NULL) {
        copy = Clone(GetElement(list_in));
        AddElement(list, copy);
        
        list_in = GetNext(list_in);
    }
    return list;
}

bool IsEmpty(List *list) {
    return GetElement(GetNext(list)) == NULL;
}
