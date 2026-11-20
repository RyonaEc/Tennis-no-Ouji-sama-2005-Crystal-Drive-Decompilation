#include "types.h"

typedef struct Node {
    u32 p0[3];
    struct Node *prev;
    struct Node *next;
} Node;

typedef struct {
    u32 p0[31];
    Node *tail;
    Node *head;
} List;

Node *func_020259e0(List *l) {
    Node *n;
    Node *nx;

    n = l->tail;
    nx = n->next;
    if (nx == 0)
        l->head = 0;
    else
        nx->prev = 0;
    l->tail = nx;
    return n;
}