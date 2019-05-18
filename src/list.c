#include "list.h"

struct Element {
    void *value;
    Iterator next;
};

struct LinkedList {
    Iterator begin;
    Iterator end;
};

void clean_list(List l) {
    l->begin = NULL;
    l->end = NULL;
}

void init_element(Iterator it) {
    it->value = NULL;
    it->next = NULL;
}

List empty_list() {
    List l = calloc(1, sizeof(struct LinkedList));
    if (!l)
        return NULL;
    clean_list(l);
    return l;
}

Iterator list_begin(List l) {
    if (!l) return NULL;
    return l->begin;
}

Iterator list_end(List l) {
    if (!l) return NULL;
    return l->end;
}

Iterator next(Iterator it) {
    return it->next;
}

void *get_value(Iterator it) {
    return it->value;
}

void *list_first_element(List l) {
    assert(l);
    return get_value(list_begin(l));
}

void *list_last_element(List l) {
    assert(l);
    return get_value(list_end(l));
}

bool reserve_back(List l) {
    if (!(l->begin)) {
        l->begin = l->end = calloc(1, sizeof(struct Element));
        if (!l->begin)
            return false;
        init_element(l->begin);
        return true;
    }
    else if (!l->end->next) {
        Iterator new_element = calloc(1, sizeof(struct Element));
        if (!new_element)
            return false;
        init_element(new_element);
        l->end->next = new_element;
        return true;
    }
    return true;
}

void unreserve_back(List l) {
    if (l->end && l->end->next) {
        free(l->end->next);
        l->end->next = NULL;
    }
}


bool emplace_back(List l, void *element_value) {
    if (l->begin && !l->begin->next && !l->begin->value) {
        l->begin->value = element_value;
        return true;
    }
    if (!(l->begin)) {
        if (!reserve_back(l))
            return false;
        l->begin->value = element_value;
        return true;
    }
    if (!(l->end->next) && !reserve_back(l))
        return false;
    l->end->next->value = element_value;
    assert(element_value != NULL);
    l->end->next->next = NULL;
    l->end = l->end->next; //check if this works
    return true;
}

void push_back(List l, Iterator it) { //CUTS OFF NEXT!!!
    it->next = NULL;
    if (!(l->begin)) {
        l->begin = l->end = it;
        return;
    }
    if (l->end->next) {
        free(l->end->next);
        l->end->next = NULL;
    }
    l->end = l->end->next = it;
}

void list_remove_at(Iterator prev, Iterator to_remove) {
    if (prev != NULL)
        prev->next = to_remove->next;
}


void list_replace_at(List original, Iterator to_be_replaced, List replacement) {
    assert(replacement->begin);
    Iterator nxt = to_be_replaced->next;
    if (nxt == NULL)
        original->end = replacement->end;
    to_be_replaced->value = replacement->begin->value;
    to_be_replaced->next = replacement->begin->next;
    unreserve_back(replacement);
    replacement->end->next = nxt;
}

void list_set_begin(List l, Iterator it) {
    l->begin = it;
}

void list_set_end(List l, Iterator it) {
    l->end = it;
}

void list_merge(List first, List second) {
    first->end->next = second->begin;
    first->end = second->end;
    free(second);
}

//Cleans list elements and their values - ONE LEVEL DEEP!!!
void list_shallow_clear(List l) {
    if (!l) return;
    for (Iterator it = l->begin; it;) {
        Iterator nxt = next(it);
        free(it->value);
        it->value = NULL;
        free(it);
        it = nxt;
    }
}

void list_iters_clear(List l) {
    if (!l) return;
    for (Iterator it = l->begin; it;) {
        Iterator nxt = next(it);
        free(it);
        it = nxt;
    }
}

bool is_empty(List l) {
    return !l->begin || !l->begin->value;
}

void *pop_front(List l) {
    assert(l->begin);
    Iterator tmp = l->begin->next;
    void *result = l->begin->value;
    free(l->begin);
    l->begin = tmp;
    return result;
}

