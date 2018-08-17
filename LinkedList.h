//implementation of linked list
//holds void pointers to data
//data type needs to be remembered and reinterpreted
//created by Silent Akufishi

#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "MemoryDebug.h"

/********************************/
/*STRUCTURES*/
/********************************/

typedef struct Node Node;

struct Node {
    void* data;
    Node* next;
    Node* prev;
};

typedef struct {
    Node *first;
    Node *last;
    size_t size;
} LinkedList;

/********************************/
/*PROTOTYPES*/
/********************************/

//allocate new linked list
LinkedList* newLL       (void);
//allocate new node
Node*       newNode     (Node* prev, Node* next, void* data);
//get number of elements in linked list
size_t      sizeLL      (LinkedList* ll);
//count number of elements in linked list
size_t      sizeLLEX    (LinkedList* ll);
//add node at the end of the linked list
bool        pushLL      (LinkedList* ll, void* data);
//add node into the linked list on specified position
bool        insertLL    (LinkedList* ll, size_t index, void* data);
//remove node from the top of the linked list
Node*       popLL       (LinkedList* ll);
//remove node from the linked list on specified position
bool        remove_iLL  (LinkedList* ll, size_t index);
//remove node from the linked list
bool        removeLL    (LinkedList* ll, Node* node);
//remove all nodes from linked list and deallocate nodes data
bool        clearLL     (LinkedList* ll);
//remove all nodes from linked list but do not deallocate nodes data
bool        lazy_clearLL(LinkedList* ll);
//deallocate the list
bool        deleteLL    (LinkedList* ll);
//deallocate the list but do not deallocate nodes data
bool        lazy_deleteLL(LinkedList* ll);
//get data pointer from specified position
void*       get_valueLL (LinkedList* ll, size_t index);
//get node pointer from specified position
Node*       get_nodeLL  (LinkedList* ll, size_t index);
//check if linked list is empty
//better than sizeLL == 0
bool        is_emptyLL  (LinkedList* ll);
//chech if linked list is not broken
bool        is_validLL  (LinkedList* ll);

#endif
