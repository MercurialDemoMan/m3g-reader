////////////////////////////////////////////
//main.c by Silent Akufishi
////////////////////////////////////////////
#include "LinkedList.h"

//functions
//allocate new linked list
LinkedList* newLL(void) {
    LinkedList* x = (LinkedList*)my_malloc(sizeof(LinkedList));
    x->first = x->last = NULL;
    x->size  = 0;
    return x;
}

//allocate new node
Node* newNode(Node* prev, Node* next, void* data) {
    Node* new_node = (Node*)my_malloc(sizeof(Node));
    new_node->prev = prev;
    new_node->next = next;
    new_node->data = data;
    return new_node;
}

//return linked list size
size_t sizeLL(LinkedList* ll) { return ll->size; }

//count linked list size
size_t sizeLLEX(LinkedList* ll) {
    if(ll == NULL) {
        printf("sizeLL(LinkedList* ll) ERROR: linked list is not allocated!\n");
        return 0;
    }
    
    if(is_emptyLL(ll)) {
        return 0;
    } else if(is_validLL(ll)) {
        size_t counter      = 1;
        Node* current_node  = ll->first;
        
        while(current_node->next != NULL) {
            current_node = current_node->next;
            counter++;
        }
        
        return counter;
        
    } else {
        printf("sizeLL(LinkedList* ll) ERROR: Linked list is not valid!\n");
        return 0;
    }
}

//add node at the end of the list
bool pushLL(LinkedList* ll, void* data) {
    
    if(ll == NULL) {
        printf("pushLL(LinkedList* ll, void* data) ERROR: linked list is not allocated!\n");
        return 0;
    }
    
    if(is_emptyLL(ll)) {
        
        Node* new_node = (Node*)my_malloc(sizeof(Node));
        
        new_node->next      = new_node->prev = NULL;
        new_node->data      = data;
        
        ll->first = new_node;
        ll->last  = new_node;
        
    } else if(is_validLL(ll)) {
        
        Node* new_node = (Node*)my_malloc(sizeof(Node));
        
        new_node->next      = NULL;
        new_node->prev      = ll->last;
        new_node->data      = data;
        
        ll->last->next      = new_node;
        ll->last            = new_node;
        
    } else {
        printf("pushLL(LinkedList* ll, void* data) ERROR: Linked list is not valid!\n");
        return 0;
    }
    
    ll->size++;
    
    return 1;
}

//insert data into linked list on specified position
bool insertLL(LinkedList* ll, size_t index, void* data) {
    
    if(ll == NULL) {
        printf("insertLL(LinkedList* ll, size_t index, void* data) ERROR: linked list is not allocated!\n");
        return 0;
    }
    
    size_t ll_size = sizeLL(ll);
    
    if(index > ll_size) {
        printf("insertLL(LinkedList* ll, size_t position, void* data) ERROR: index out of range!\n");
        return 0;
    }
    
    if(is_validLL(ll)) {
        
        //empty linked list
        if(is_emptyLL(ll)) {
            
            Node* new_node = newNode(NULL, NULL, data);
            ll->first = ll->last = new_node;
            
        } else {
            //same as push function
            if(index == ll_size) {
                
                pushLL(ll, data);
                
            //first node
            } else if(index == 0) {
                
                Node* new_node  = newNode(NULL, ll->first, data);
                ll->first->prev = new_node;
                ll->first       = new_node;
                
            //last node
            } else if(index == ll_size - 1) {
                
                Node* new_node       = newNode(ll->last->prev, ll->last, data);
                ll->last->prev->next = new_node;
                ll->last->prev       = new_node;
                
            //middle
            } else {
                
                Node* new_node     = newNode(NULL, NULL, data);
                Node* current_node = get_nodeLL(ll, index);
                
                new_node->prev = current_node->prev;
                new_node->next = current_node;
                
                current_node->prev->next = new_node;
                current_node->prev       = new_node;
                
            }
        }
    } else {
        printf("insertLL(LinkedList* ll, size_t index, void* data) ERROR: Linked list is not valid!\n");
        return 0;
    }
    
    ll->size++;
    
    return 1;
}

//remove node from the top of the linked list
Node* popLL(LinkedList* ll) {
    
    if(ll == NULL) {
        printf("popLL(LinkedList* ll) ERROR: linked list is not allocated!\n");
        return NULL;
    }
    
    if(is_emptyLL(ll)) {
        printf("popLL(LinkedList* ll) ERROR: Linked list is empty!\n");
        return NULL;
    }
    
    if(is_validLL(ll)) {
        
        Node* current_node = ll->last;
        
        if(ll->first == ll->last) {
            ll->first = ll->last = NULL;
        } else {
            ll->last->prev->next = NULL;
            ll->last = ll->last->prev;
        }
        
        ll->size--;
        
        return current_node;
        
    } else {
        printf("popLL(LinkedList* ll) ERROR: Linked list is not valid!\n");
        return NULL;
    }
}

//remove node from linked list
bool remove_iLL(LinkedList* ll, size_t index) {
    
    if(ll == NULL) {
        printf("remove_iLL(LinkedList* ll, size_t index) ERROR: linked list is not allocated!\n");
        return 0;
    }
    
    size_t ll_size = sizeLL(ll);
    
    if(ll_size == 0 || index > ll_size - 1) {
        printf("remove_iLL(LinkedList* ll, size_t index) ERROR: index out of range!\n");
        return 0;
    }
    
    if(is_validLL(ll)) {
        
        //in linked list is one node
        if(ll->first == ll->last) {
            
            my_free(ll->first->data);
            my_free(ll->first);
            ll->first = ll->last = NULL;
            
            //multiple nodes
        } else {
            
            //get node
            Node* current_node  = get_nodeLL(ll, index);
            
            //first node
            if(current_node == ll->first) {
                
                my_free(ll->first->data);
                
                ll->first = current_node->next;
                ll->first->prev = NULL;
                
            //last node
            } else if(current_node == ll->last) {
                
                my_free(ll->last->data);
                
                ll->last = current_node->prev;
                ll->last->next = NULL;
                
            //middle
            } else {
                
                my_free(current_node->data);
                
                current_node->prev->next = current_node->next;
                current_node->next->prev = current_node->prev;
            }
            
            my_free(current_node);
        }
    } else {
        printf("remove_iLL(LinkedList* ll, size_t index) ERROR: Linked list is not valid!\n");
        return 0;
    }
    
    ll->size--;
    
    return 1;
}

//remove node from linked list
bool removeLL(LinkedList* ll, Node* node) {
    
    if(ll == NULL) {
        printf("removeLL(LinkedList* ll, Node* node) ERROR: linked list is not allocated!\n");
        return 0;
    }
    
    if(is_emptyLL(ll)) {
        printf("removeLL(LinkedList* ll, Node* node) ERROR: node doesn't exist!\n");
        return 0;
    }
    
    size_t ll_size = sizeLL(ll);
    
    if(is_validLL(ll)) {
        
        //check if node exists
        Node* current_node = ll->first;
        
        size_t counter     = 0;
        
        while(counter != ll_size) {
            if(current_node == node) { break; }
            current_node = current_node->next;
            counter++;
        }
        
        if(current_node == NULL) {
            printf("removeLL(LinkedList* ll, Node* node) ERROR: node doesn't exist!\n");
            return 0;
        }
            
        //first node
        if(current_node == ll->first) {
            
            my_free(ll->first->data);
            
            ll->first = current_node->next;
            ll->first->prev = NULL;
            
            //last node
        } else if(current_node == ll->last) {
            
            my_free(ll->last->data);
            
            ll->last = current_node->prev;
            ll->last->next = NULL;
            
            //middle
        } else {
            
            my_free(current_node->data);
            
            current_node->prev->next = current_node->next;
            current_node->next->prev = current_node->prev;
        }
        
        my_free(current_node);
        
    } else {
        printf("removeLL(LinkedList* ll, Node* node) ERROR: Linked list is not valid!\n");
        return 0;
    }
    
    ll->size--;
    
    return 1;
}

//deallocate all data inside linked list
bool clearLL(LinkedList* ll) {
    
    if(ll == NULL) {
        printf("clearLL(LinkedList* ll) ERROR: Linked list is not allocated!\n");
        return 0;
    }
    
    if(is_emptyLL(ll)) {
        //pass
    } else if(is_validLL(ll)) {
        
        Node* current_node = ll->last;
        
        while(ll->first != ll->last && ll->last != NULL) {
            
            ll->last = ll->last->prev;
            if(current_node->data != NULL) { my_free(current_node->data); }
            my_free(current_node);
            current_node = ll->last;
            
        }
        if(current_node->data != NULL) { my_free(current_node->data); }
        my_free(current_node);
        
        ll->first = NULL;
        ll->last  = NULL;
        
    } else {
        printf("clearLL(LinkedList* ll) ERROR: Linked list is not valid!\n");
        return 0;
    }
    
    ll->size = 0;
    
    return 1;
}

//deallocate all data inside linked list
bool lazy_clearLL(LinkedList* ll) {
    
    if(ll == NULL) {
        printf("clearLL(LinkedList* ll) ERROR: Linked list is not allocated!\n");
        return 0;
    }
    
    if(is_emptyLL(ll)) {
        //pass
    } else if(is_validLL(ll)) {
        
        Node* current_node = ll->last;
        
        while(ll->first != ll->last && ll->last != NULL) {
            
            ll->last = ll->last->prev;
            my_free(current_node);
            current_node = ll->last;
            
        }
        my_free(current_node);
        
        ll->first = NULL;
        ll->last  = NULL;
        
    } else {
        printf("clearLL(LinkedList* ll) ERROR: Linked list is not valid!\n");
        return 0;
    }
    
    ll->size = 0;
    
    return 1;
}

//deallocate linked list
bool deleteLL(LinkedList* ll) {
    
    if(ll == NULL) {
        printf("deleteLL(LinkedList* ll) WARNING: linked list is already deallocated!\n");
        return 0;
    }
    
    clearLL(ll);
    my_free(ll);
    return 1;
}

//deallocate linked list
bool lazy_deleteLL(LinkedList* ll) {
    if(ll == NULL) {
        printf("deleteLL(LinkedList* ll) WARNING: linked list is already deallocated!\n");
        return 0;
    }
    
    lazy_clearLL(ll);
    my_free(ll);
    return 1;
}

//get data pointer from node
void* get_valueLL(LinkedList* ll, size_t index) {
    
    if(ll == NULL) {
        printf("get_valueLL(LinkedList* ll, size_t index) ERROR: linked list is not allocated!\n");
        return 0;
    }
    
    size_t ll_size = sizeLL(ll);
    
    if(ll_size == 0 || index > ll_size - 1) {
        printf("get_valueLL(LinkedList* ll, size_t index) ERROR: index out of range!\n");
        return NULL;
    }
    
    if(is_validLL(ll)) {
        
        Node* current_node  = get_nodeLL(ll, index);
        return current_node->data;
        
    } else {
        printf("get_valueLL(LinkedList* ll, size_t index) ERROR: Linked list is not valid!\n");
        return 0;
    }
}

Node* get_nodeLL(LinkedList* ll, size_t index) {
    
    if(ll == NULL) {
        printf("get_nodeLL(LinkedList* ll, size_t index) ERROR: linked list is not allocated!\n");
        return 0;
    }
    
    size_t ll_size = sizeLL(ll);
    
    if(ll_size == 0 || index > ll_size - 1) {
        printf("get_nodeLL(LinkedList* ll, size_t index) ERROR: index out of range!");
    }
    
    if(is_validLL(ll)) {
        Node* current_node = ll->first;
        
        size_t counter     = 0;
        
        if(index > ll_size / 2) {
            current_node = ll->last;
            counter = ll_size - 1;
            while(counter != index) {
                current_node = current_node->prev;
                if(current_node == NULL) { break; }
                counter--;
            }
        } else {
            while(counter != index) {
                current_node = current_node->next;
                if(current_node == NULL) { break; }
                counter++;
            }
        }
        
        if(current_node == NULL) {
            printf("get_nodeLL(LinkedList* ll, size_t index) WARING: node is not in the linked list!\n");
        }
        
        return current_node;
        
    } else {
        printf("get_nodeLL(LinkedList* ll, size_t index) ERROR: Linked list is not valid!\n");
        return NULL;
    }
}

//check if linked list is empty
bool is_emptyLL(LinkedList* ll) {
    return ll->first == NULL && ll->last == NULL;
}

//check if linked list is valid
bool is_validLL(LinkedList* ll) {
    return (ll->first == NULL && ll->last == NULL) || (ll->first != NULL && ll->last != NULL);
}
