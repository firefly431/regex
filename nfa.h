#ifndef INCLUDED_NFA_H
#define INCLUDED_NFA_H 

#include "parser.h"

extern int node_counter;

/* some ids may be unused */
struct node {
    int id;
    struct edge_list {
        struct edge_node {
            struct node *to;
            char transition;
            struct edge_node *next;
        } *head;
    } edges;
};

struct node *create_node();
void free_node(struct node *x);
void add_edge(struct node *from, struct node *to, char transition);

struct nfa {
    struct node *initial, *final;
};

struct nfa create_nfa(struct node *initial, struct node *final);

struct nfa convert_regex(struct regex *x);
struct nfa convert_group(struct group *x);
struct nfa convert_string(struct string *x);

struct nfa_final {
    struct node *initial, *final;
    size_t size;
} finalize(struct nfa x);

#endif
