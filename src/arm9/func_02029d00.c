#include "types.h"

typedef struct Node {
    struct Node *prev;
    struct Node *next;
} Node;

void func_02029d00(Node *n, Node *list)
{
    Node *p = n->prev;
    Node *q = n->next;
    Node *t;

    if (p)
        p->next = q;
    if (q)
        q->prev = p;
    t = list->next;
    while (t) {
        list = t;
        t = t->next;
    }
    list->next = n;
    n->prev = list;
    n->next = 0;
}
