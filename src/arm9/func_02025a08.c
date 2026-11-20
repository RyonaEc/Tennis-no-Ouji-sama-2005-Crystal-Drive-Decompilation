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

void func_02025a08(List *l, Node *n) {
    Node *nx;
    Node *pv;

    nx = n->next;
    pv = n->prev;
    if (nx == 0)
        l->head = pv;
    else
        nx->prev = pv;
    if (pv == 0)
        l->tail = nx;
    else
        pv->next = nx;
}