#include <stdio.h>
#include <stdlib.h>
#include "parser.h"

#define ENSURE_MALLOC(x) do {if (!(x)) {fprintf(stderr, "malloc failed\n"); exit(1);}} while (0)

struct symbol {
    enum sym_type {
        END, CHAR, STAR, UNION, LPAREN, RPAREN, PLUS
    } type;
    int data;
};

const char *symbols[] = {"EOF", "CHAR", "'*'", "'|'", "'('", "')'", "'+'"};

#define LL 2

/* symbol type 0 is END */
struct symbol next[LL] = {{0}};

void nextsym() {
    int c;
    int i;
    for (i = LL - 2; i >= 0; --i)
        next[i] = next[i + 1];
    next[LL - 1].data = c = getchar();
    switch (c) {
    case EOF:
        next[LL - 1].type = END; break;
    case '*':
        next[LL - 1].type = STAR; break;
    case '+':
        next[LL - 1].type = PLUS; break;
    case '|':
        next[LL - 1].type = UNION; break;
    case '(':
        next[LL - 1].type = LPAREN; break;
    case ')':
        next[LL - 1].type = RPAREN; break;
    case '\\':
        next[LL - 1].data = c = getchar();
        if (c == EOF) {
            fprintf(stderr, "Backslash at end of file\n");
            exit(1);
        }
    default:
        next[LL - 1].type = CHAR;
    }
}

void init_parser() {
    for (int i = 0; i < LL; i++)
        nextsym();
}

bool accept(enum sym_type type) {
    if (next[0].type != type)
		return false;
    nextsym();
    return true;
}

bool accept_char(char *x) {
    if (next[0].type != CHAR)
		return false;
    *x = (char)next[0].data;
    nextsym();
    return true;
}

void expect(enum sym_type type) {
    if (!accept(type)) {
        fprintf(stderr, "Expected %s, not %s\n", symbols[type], symbols[next[0].type]);
        exit(1);
    }
}

void expect_char(char *x) {
    if (!accept_char(x)) {
        fprintf(stderr, "Expected %s, not %s\n", symbols[CHAR], symbols[next[0].type]);
        exit(1);
    }
}

struct string *parse_string() {
    struct string *ret = (struct string *)malloc(sizeof (struct string));
    ENSURE_MALLOC(ret);
    if (accept_char(&(ret->data.charsym)))
        ret->data.regex = NULL;
    else {
        if (!accept(LPAREN)) {
            free(ret);
            return NULL;
        }
        ret->data.regex = parse_regex();
        expect(RPAREN);
    }
    ret->flag = accept(STAR) ? F_STAR : accept(PLUS) ? F_PLUS : F_NONE;
    return ret;
}

struct group *parse_group() {
    struct group *ret = (struct group *)malloc(sizeof (struct group)), *cur = ret;
    ENSURE_MALLOC(ret);
    ret->string = parse_string();
    ret->next = NULL;
    if (!ret->string) {
        return ret;
    }
    while (true) {
        struct string *s = parse_string();
        if (!s)
            break;
        cur = cur->next = (struct group *)malloc(sizeof (struct group));
        ENSURE_MALLOC(cur);
        cur->string = s;
        cur->next = NULL;
    }
    return ret;
}

struct regex *parse_regex() {
    struct regex *ret = (struct regex *)malloc(sizeof (struct regex)), *cur = ret;
    do {
        ENSURE_MALLOC(cur);
        cur->group = parse_group();
        cur->next = NULL;
    } while (accept(UNION) && (cur = cur->next = (struct regex *)malloc(sizeof (struct regex)), true));
    return ret;
}

void free_string(struct string *x) {
    if (!x) return;
    free_regex(x->data.regex);
    free(x);
}

void free_group(struct group *x) {
    if (!x) return;
    free_string(x->string);
    free_group(x->next);
    free(x);
}

void free_regex(struct regex *x) {
    if (!x) return;
    free_group(x->group);
    free_regex(x->next);
    free(x);
}

void print_indent(int indent) {
    for (int i = 0; i < indent; i++)
        putchar(' ');
}

void print_string(struct string *x, int indent) {
    print_indent(indent);
    if (x->data.regex) {
        puts("string (");
        print_regex(x->data.regex, indent + 1);
    } else {
        printf("string (%c", x->data.charsym);
    }
    if (x->flag != F_NONE)
        putchar(x->flag == F_STAR ? '*' : '+');
    putchar(')');
}

void print_group(struct group *x, int indent) {
    print_indent(indent);
    if (!x->string) {
        printf("group ()");
        return;
    }
    puts("group (");
    print_string(x->string, indent + 1);
    if (x->next) {
        putchar('\n');
        print_group(x->next, indent + 1);
    }
    putchar(')');
}

void print_regex(struct regex *x, int indent) {
    print_indent(indent);
    puts("regex (");
    print_group(x->group, indent + 1);
    if (x->next) {
        putchar('\n');
        print_regex(x->next, indent + 1);
    }
    putchar(')');
}

#ifdef PARSER_MAIN
int main() {
    for (int i = 0; i < LL; i++)
        nextsym();
    struct regex *r = parse_regex();
    print_regex(r, 0);
    free_regex(r);
    return 0;
}
#endif
