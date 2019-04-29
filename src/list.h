#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <limits.h>
#include <string.h>
#include <stdio.h>

#ifndef __LIST_H__
#define __LIST_H__

#define FOREACH(it, list) for(Iterator it = list_begin(list); \
                              it && it != next(list_end(list)); it = next(it))

typedef struct LinkedList *List;

typedef struct Element *Iterator;

void clean_list(List l);

List empty_list();

bool is_empty(List l);

Iterator list_begin(List l);

void list_set_begin(List l, Iterator it);

Iterator list_end(List l);

void list_set_end(List l, Iterator it);

Iterator next(Iterator it);

void *get_value(Iterator it);

void *list_first_element(List l);

void *list_last_element(List l);

bool reserve_back(List l);

void unreserve_back(List l);

bool emplace_back(List l, void *element_value);

void push_back(List l, Iterator it);

void *pop_front(List l);

void list_remove_at(Iterator prev, Iterator to_remove);

void list_replace_at(List original, Iterator to_be_replaced, List replacement);

void list_merge(List first, List second);

void list_shallow_clear(List l);

void list_iters_clear(List l);

#endif /* __LIST_H__ */