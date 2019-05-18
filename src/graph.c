#include "graph.h"

City make_vertex(char *city_name) {
    City city = calloc(1, sizeof(struct Vertex));
    if (!city)
        return NULL;
    city->neighbours = empty_list();
    if (!city->neighbours) {
        free(city);
        return NULL;
    }
    city->city_name = city_name;
    return city;
}

bool update_year(Road r, int repair_year) {
    if (r->built_year > repair_year)
        return false;
    r->built_year = repair_year;
    return true;
}

void erase_vertex(City u) {
    if (u->neighbours) {
        for (Iterator it = list_begin(u->neighbours); it;) {
            Iterator nxt = next(it);
            Road r = get_value(it);
            if (r->refcount) {
                r->refcount = false;
            }
            else {
                if (r->routes_passing) {
                    list_iters_clear(r->routes_passing);
                    free(r->routes_passing);
                    r->routes_passing = NULL;
                }
                free(r);
            }
            free(it);
            it = nxt;
        }
    }
    free(u->city_name);
    u->city_name = NULL;
    free(u->neighbours);
    u->neighbours = NULL;
    free(u);
}

Road make_road(City u, City v, unsigned length, int built_year) {
    Road new = calloc(1, sizeof(struct Edge));
    if (!new)
        return NULL;
    new->routes_passing = empty_list();
    if (!new->routes_passing) {
        free(new);
        return NULL;
    }
    new->begin = u;
    new->end = v;
    new->length = length;
    new->built_year = built_year;
    new->refcount = true;
    return new;
}

bool populate_road(Road r) {
    if (!reserve_back(r->begin->neighbours))
        return false;
    if (!reserve_back(r->end->neighbours)) {
        unreserve_back(r->begin->neighbours);
        return false;
    }

    emplace_back(r->begin->neighbours, r);
    emplace_back(r->end->neighbours, r);
    return true;
}

void orientate_road(Road r, City new_road_begin) {
    if (r->begin != new_road_begin) {
        City tmp = r->begin;
        r->begin = r->end;
        r->end = tmp;
    }
}

static Iterator get_road_position(City u, City v) {

    FOREACH(it, u->neighbours) {
        assert(get_value(it));
        Road r = get_value(it);
        orientate_road(r, u);
        if (r->end == v)
            return it;
    }
    return NULL;
}

Road get_road(City u, City v) {
    Iterator it = get_road_position(u, v);
    if (!it)
        return NULL;
    return get_value(it);
}

static void erase_from_neighbours(City c, Road r) {
    Iterator prev = NULL;
    for (Iterator it = list_begin(c->neighbours); it;) {
        assert(get_value(it));
        Road current = get_value(it);
        if (current == r) {
            if (list_begin(c->neighbours) == list_end(c->neighbours))
                clean_list(c->neighbours);
            else if (list_begin(c->neighbours) == it) {
                list_set_begin(c->neighbours, next(it));
            }
            else if (list_end(c->neighbours) == it) {
                list_set_end(c->neighbours, prev);
            }
            list_remove_at(prev, it);
            free(it);
            break;
        }
        prev = it;
        it = next(it);
    }
}

void erase_road(Road r) {
    erase_from_neighbours(r->begin, r);
    erase_from_neighbours(r->end, r);
    list_iters_clear(r->routes_passing);
    free(r->routes_passing);
    r->routes_passing = NULL;
    free(r);
}


//returns "sign of a - b" so -1 when a < b
int operator_less(uint64_t distance1, int year1,
                  uint64_t distance2, int year2) {
    if (distance1 != distance2)
        return distance1 < distance2 ? -1 : 1;
    else if (year1 != year2)
        return year1 > year2 ? -1 : 1;
    return 0;
}


char *save_vertex_name(const char *input_name) {
    char *result = calloc(strlen(input_name) + 2, sizeof(char));
    if (!result) return NULL;
    strcpy(result, input_name);
    return result;
}