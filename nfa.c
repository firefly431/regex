#include <stdio.h>
#include <stdlib.h>
#include "nfa.h"

#define EPSILON '\0'

int node_counter;

struct node *create_node() {
    struct node *ret = (struct node *)malloc(sizeof (struct node));
    ret->id = node_counter++;
    ret->edges.head = NULL;
    return ret;
}

void free_node(struct node *x) {
    if (!x) return;
    /* free edges */
    struct edge_node *cur;
    while ((cur = x->edges.head)) {
        x->edges.head = x->edges.head->next;
        free(cur);
    }
    free(x);
}

void add_edge(struct node *from, struct node *to, char transition) {
    /* add to front because it's easier and faster */
    struct edge_node *edge = (struct edge_node *)malloc(sizeof (struct edge_node));
    edge->to = to;
    edge->transition = transition;
    edge->next = from->edges.head;
    from->edges.head = edge;
}

struct nfa blank_nfa() {
    return (struct nfa) { create_node(), create_node() };
}

struct nfa empty_nfa() {
    struct node *a = create_node(), *b = create_node();
    add_edge(a, b, EPSILON);
    return (struct nfa) { a, b };
}

struct nfa nfa_union(struct nfa a, struct nfa b) {
    struct nfa ret = blank_nfa();
    add_edge(ret.initial, a.initial, EPSILON);
    add_edge(ret.initial, b.initial, EPSILON);
    add_edge(a.final, ret.final, EPSILON);
    add_edge(b.final, ret.final, EPSILON);
    return ret;
}

struct nfa nfa_concat(struct nfa a, struct nfa b) {
    /* avoid two passes by freeing and adding in same loop */
    struct edge_node *cur;
    while ((cur = b.initial->edges.head)) {
        b.initial->edges.head = b.initial->edges.head->next;
        add_edge(a.final, cur->to, cur->transition);
        free(cur);
    }
    return (struct nfa) { a.initial, b.final };
}

struct nfa nfa_star(struct nfa x) {
    struct nfa ret = empty_nfa();
    add_edge(ret.initial, x.initial, EPSILON);
    add_edge(x.final, ret.final, EPSILON);
    add_edge(x.final, x.initial, EPSILON);
    return ret;
}

struct nfa nfa_plus(struct nfa x) {
    add_edge(x.final, x.initial, EPSILON);
    return x;
}

struct nfa convert_regex(struct regex *x) {
    struct nfa a = convert_group(x->group);
    while (x->next)
        a = nfa_union(a, convert_group((x = x->next)->group));
    return a;
}

struct nfa convert_group(struct group *x) {
    struct nfa a = convert_string(x->string);
    while (x->next)
        a = nfa_concat(a, convert_string((x = x->next)->string));
    return a;
}

struct nfa convert_string(struct string *x) {
    if (!x) return empty_nfa();
    struct nfa a;
    if (x->data.regex)
        a = convert_regex(x->data.regex);
    else {
        a = blank_nfa();
        add_edge(a.initial, a.final, x->data.charsym);
    }
    if (x->flag)
        if (x->flag == F_STAR)
            a = nfa_star(a);
        else
            a = nfa_plus(a);
    return a;
}

struct nfa_final finalize(struct nfa x) {
    struct nfa_final ret = { x.initial, x.final, node_counter };
    node_counter = 0;
    return ret;
}
