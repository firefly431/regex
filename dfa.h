#ifndef INCLUDED_DFA_H
#define INCLUDED_DFA_H 

#include "nfa.h"

struct dfa {
    struct node *initial;
    size_t size;
};

struct dfa convert_nfa(struct nfa_final x);

#endif
