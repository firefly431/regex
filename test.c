#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include <assert.h>
#include "parser.h"
#include "nfa.h"

FILE *f;
struct node **nodes;

void traverse(struct node *x) {
    nodes[x->id] = x;
    for (struct edge_node *cur = x->edges.head; cur; cur = cur->next) {
        fprintf(f, " %d -> %d", x->id, cur->to->id);
        fputs(" [label=\"", f);
        if (cur->transition == '\0')
            fputs("\xc9\x9b", f);
        else
            fputc(cur->transition, f);
        fputs("\"];\n", f);
        if (!nodes[cur->to->id])
            traverse(cur->to);
        else
            assert(nodes[cur->to->id] == cur->to);
    }
}

int main() {
    init_parser();
    struct regex *r = parse_regex();
    print_regex(r, 0);
    putchar('\n');
    struct nfa result = convert_regex(r);
    printf("%d nodes allocated\n", node_counter);
    /* nodes[i] points to the node with id i */
    nodes = (struct node **)calloc(node_counter, sizeof (struct node *));
    if (!nodes) {
        fprintf(stderr, "malloc failed\n");
        free_regex(r);
        return 1;
    }
    /* generate dot file */
    f = fopen("out.dot", "w");
    if (!f) {
        fprintf(stderr, "could not open out.dot for writing\n");
        free(nodes);
        free_regex(r);
        return 1;
    }
    fputs("digraph G {\n", f);
    traverse(result.initial);
    fputs("}\n", f);
    fclose(f);
    /* generate a few random sequences */
    srand((unsigned)time(NULL));
    for (int i = 0; i < 10; i++) {
        struct node *cur = result.initial;
        while (cur != result.final) {
            /* weighted reservoir sampling */
            struct edge_node *next = cur->edges.head;
            double key = ((double)rand()) / RAND_MAX;
            key = pow(key, next->transition == '\0' ? 2 : 1);
            for (struct edge_node *en = next->next; en; en = en->next) {
                double k = ((double)rand()) / RAND_MAX;
                k = pow(k, en->transition == '\0' ? 2 : 1);
                if (k > key) {
                    key = k;
                    next = en;
                }
            }
            if (next->transition != '\0')
                putchar(next->transition);
            cur = next->to;
        }
        putchar('\n');
    }
    for (int i = 0; i < node_counter; i++)
        free_node(nodes[i]);
    free(nodes);
    free_regex(r);
    return 0;
}
