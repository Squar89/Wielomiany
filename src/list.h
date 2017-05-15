/** @file
   Interfejs biblioteki list

   @author Jakub Wróblewski <jw386401@students.mimuw.edu.pl>
   @date 2017-05-13
*/
#ifndef LIST_H
#define LIST_H

#include <stdbool.h>

/**
 * Struktura przechowująca element listy.
 * Zbudowana jest z wskaznika na poprzedni i nastepny element
 * oraz wskaznika na sam element
 */
typedef struct List {
    struct List *previous;
    struct List *next;
    void *element;
} List;

/**
 * Sprawdza czy dana lista zawiera NULLowy wskaźnik na sąsiadów.
 * @param[in] list : lista
 * @return czy wskaźnik na któregoś z sąsiadów jest NULLem?
 */
extern bool CheckIfNullPointer(List *list);

/**
 * Zwraca element z listy.
 * @param[in] list : lista
 * @return element listy
 */
extern void* GetElement(List *list);

/**
 * Zwraca poprzednika elementu z listy.
 * @param[in] list : lista
 * @return poprzednik elementu listy
 */
extern List* GetPrevious(List *list);

/**
 * Zwraca następnika elementu z listy.
 * @param[in] list : lista
 * @return następnik elementu listy
 */
extern List* GetNext(List *list);

/**
 * Tworzy element listy.
 * @param[in] element : struktura dla której chcemy stworzyć element listy
 * @return lista
 */
extern List* CreateListElement(void* element);

/**
 * Tworzy połączenie między dwoma elementami listy.
 * następnikiem pierwszej listy zostaje druga lista
 * poprzednikiem drugiej listy zostaje pierwsza lista
 * @param[in] first_list : lista
 * @param[in] second_list : lista
 */
extern void SetConnection(List *first_list, List *second_list);

/**
 * Dodaje element do listy.
 * @param[in] list : lista do której chcemy dodać element
 * @param[in] element : element który chcemy dodać
 */
extern void AddElement(List *list, void *element);

/**
 * Przygotowuje pustą listę z strażnikiem.
 * @return lista
 */
extern List* SetupList();

/**
 * Usuwa element listy wykorzystując podaną funkcję.
 * @param[in] Delete : funkcja przy pomocy której zostanie usunięty element
 * @param[in] list : lista
 */
extern void DeleteListElement(void (*Delete)(void*), List *list);

/**
 * Usuwa całą podaną listę wykorzystując podaną funkcję.
 * @param[in] Delete : funkcja przy pomocy której zostanią usunięte elementy
 * @param[in] list : lista do usunięca
 */
extern void DeleteList(void (*Delete)(void*), List *list);

/**
 * Klonuje listę wykorzystując podaną funkcję.
 * @param[in] Clone : funkcja przy pomocy której zostaną skopiowane elementy
 * @param[in] list_in : lista do skopiowania
 * @return kopia podanej listy
 */ 
extern List* CloneList(void* (*Clone)(void*), List *list_in);

/**
 * Sprawdza czy dana lista jest pusta.
 * @param[in] list : lista
 * @return czy lista jest pusta?
 */
extern bool IsEmpty(List *list);

#endif /* LIST_H */