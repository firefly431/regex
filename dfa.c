#include <assert.h>
#include "dfa.h"

#define EPSILON '\0'

#define WORD_BYTES (sizeof (unsigned int))
#define WORD_BITS (WORD_BYTES * 8)

/* powerset construction algorithm */

struct bitset {
    unsigned int *bits;
    size_t size; /* length of array, not number of elements */
};

struct bitset create_bitset(size_t size) {
    size_t n_words = (size + WORD_BITS - 1) / WORD_BITS;
    return (struct bitset) { (unsigned int *) calloc(n_words, WORD_BYTES), n_words };
}

void free_bitset(struct bitset x) {
    free(x.bits);
}

void bit_set(struct bitset x, size_t index) {
    assert(index < x.size * WORD_BITS);
    x.bits[index / WORD_BITS] |= 1 << (index % WORD_BITS);
}

bool bit_get(struct bitset x, size_t index) {
    assert(index < x.size * WORD_BITS);
    return !!(x.bits[index / WORD_BITS] & (1 << (index % WORD_BITS)));
}

struct bitset bit_union(struct bitset a, struct bitset b) {
    assert(a.size == b.size);
    struct bitset ret = create_bitset(a.size * WORD_BITS);
    for (int i = 0; i < a.size; i++)
        ret.bits[i] = a.bits[i] | b.bits[i];
    return ret;
}

struct bitset bit_intersect(struct bitset a, struct bitset b) {
    assert(a.size == b.size);
    struct bitset ret = create_bitset(a.size * WORD_BITS);
    for (int i = 0; i < a.size; i++)
        ret.bits[i] = a.bits[i] & b.bits[i];
    return ret;
}

void bit_equals(struct bitset a, struct bitset b) {
    assert(a.size == b.size);
    return !memcmp(a.bits, b.bits, a.size * WORD_BYTES);
}

struct nfa_temp {
    struct nfa_final nfa;
    node **imap;
} *nfa_traverse(struct nfa_final x) {
    struct nfa_temp *ret = (struct nfa_temp *)malloc(sizeof (struct nfa_temp));
    ret->nfa = x;
    ret->imap = (struct node **)calloc(x.size, sizeof (struct node *));
    /* populate imap */
    return ret;
}

void free_temp(struct nfa_temp *x) {
    free(x->imap);
    free(x);
}

struct bitset e_closure(struct bitset x, const struct nfa_temp *nfa) {
    struct bitset temp = create_bitset(x.size * WORD_BITS);
    struct bitset res = bit_union(x, temp);
    free_bitset(temp);
}

struct dfa convert_nfa(struct nfa_final x) {
    struct set_queue *Q = create_queue();
    /* done maps bitset to dfa_node */
    struct dfa_map *done = create_map();
    struct bitset initial = create_bitset(x.size);
    bit_set(initial, x.initial->id);
    struct nfa_temp *nfa = nfa_traverse(x);
    queue_add(Q, e_closure(initial));
    struct bitset cur;
    while ((cur = queue_remove(Q))) {
        if (map_contains(done, cur)) continue;
        /* go through transitions for each node in the set */
        /* calculate e-closure for each and add to Q */
        /* then add to done */
    }
    free_bitset(initial);
    /* create dfa graph by traversing from initial */
    free_temp(nfa);
    free_queue(done);
    free_queue(Q);
}
