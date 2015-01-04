#ifndef INCLUDED_PARSER_H
#define INCLUDED_PARSER_H 

#include <stdbool.h>
/* use this if you don't have stdbool.h for some reason
typedef enum { false = 0, int = 1 } bool;
*/

void init_parser();

struct string;
struct group;
struct regex;

struct string {
    struct {
        struct regex  *regex;
        char           charsym;
    } data;
    enum flag {
        F_NONE = 0,
        F_STAR = 1,
        F_PLUS = 2
    } flag;
};

struct group {
    struct string *string;
    struct group  *next;
};

struct regex {
    struct group *group;
    struct regex *next;
};

struct string *parse_string();
struct group  *parse_group();
struct regex  *parse_regex();

void free_string(struct string *x);
void free_group(struct group *x);
void free_regex(struct regex *x);

void print_string(struct string *x, int indent);
void print_group(struct group *x, int indent);
void print_regex(struct regex *x, int indent);

#endif
