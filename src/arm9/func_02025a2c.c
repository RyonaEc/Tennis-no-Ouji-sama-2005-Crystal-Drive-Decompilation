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

void func_02025a2c(List *l, Node *n) {
    Node *h;

    h = l->head;
    if (h == 0) {
        l->tail = n;
    } else {
        Node *t = l->tail;
        t->next = n;
    }
    n->prev = h;
    n->next = 0;
    l->head = n;
}