#include "routes.h"

#define MAXIMUM_ROUTES_AMOUNT 1000

/**
 * Informacje o drodze krajowej.
 */
struct RouteData {
    City begin; /**< Początek drogi krajowej. **/
    City end; /**< Koniec drogi krajowej. **/
    List path; /**< Kolejne elementy drogi krajowej. **/
};

typedef struct RouteData *Route;

typedef struct DirectedEdge *RouteEdge;

/**
 * Struktura przechowująca listę dróg krajowych.
 */
struct Routes {
    Route routes[MAXIMUM_ROUTES_AMOUNT]; /**< Tablica dróg krajowych. **/
};

RoutesList routes_empty_list() {
    RoutesList l = calloc(1, sizeof(struct Routes));
    for (int i = 0; i < MAXIMUM_ROUTES_AMOUNT; ++i)
        l->routes[i] = NULL;
    return l;
}

bool routes_exists(RoutesList routes, unsigned route_id) {
    if (route_id < 1 || route_id > 999) return false;
    return (routes->routes[route_id] != NULL);
}

bool routes_vertex_belongs_to(RoutesList routes, unsigned route_id,
                              City vertex) {
    FOREACH(it, routes->routes[route_id]->path) {
        assert(get_value(it));

        RouteEdge path_edge = get_value(it);
        if (vertex == path_edge->edge->begin || vertex == path_edge->edge->end)
            return true;
    }
    return false;
}

City routes_get_begin(RoutesList routes, unsigned route_id) {
    return routes->routes[route_id]->begin;
}

City routes_get_end(RoutesList routes, unsigned route_id) {
    return routes->routes[route_id]->end;
}

Path routes_get_path(RoutesList routes, unsigned route_id) {
    return routes->routes[route_id]->path;
}

static void align_route_edge(RouteEdge e) {
    orientate_road(e->edge, e->start);
}

bool routes_add_with_id(RoutesList routes, unsigned route_id,
                        Path route_to_add) {
    assert(route_to_add && !is_empty(route_to_add));
    Route new = calloc(1, sizeof(struct RouteData));
    if (!new)
        return false;

    FOREACH(it, route_to_add) {
        RouteEdge current_edge = get_value(it);
        if (!reserve_back(current_edge->edge->routes_passing))
            return false;
    }

    FOREACH(it, route_to_add) {
        assert(get_value(it));
        RouteEdge current_edge = get_value(it);
        current_edge->route_id = route_id;
        emplace_back(current_edge->edge->routes_passing, it);
    }

    RouteEdge starting = list_first_element(route_to_add);

    new->begin = starting->start;

    RouteEdge ending = list_last_element(route_to_add);
    align_route_edge(ending);

    new->end = ending->edge->end;

    new->path = route_to_add;

    routes->routes[route_id] = new;
    return true;
}


/**
 * Ustawia id krawędziom ścieżki i dodatkowo dodaje je na listę dróg krajowych
 * @param path
 * @param route_id
 * @return
 */
static bool set_route_id(Path path, unsigned route_id) {
    FOREACH(it, path) {
        assert(get_value(it));
        RouteEdge edge = get_value(it);
        edge->route_id = route_id;
        emplace_back(edge->edge->routes_passing, it);
    }
    return true;
}

bool routes_add_prefix(RoutesList routes, unsigned route_id,
                       Path path_to_append) {
    if (!set_route_id(path_to_append, route_id))
        return false;
    Route current = routes->routes[route_id];
    RouteEdge second_beginning = list_first_element(path_to_append);
    current->begin = second_beginning->start;
    list_merge(path_to_append, current->path);
    current->path = path_to_append;
    return true;
}

bool routes_add_suffix(RoutesList routes, unsigned route_id,
                       Path path_to_append) {
    if (!set_route_id(path_to_append, route_id))
        return false;
    Route current = routes->routes[route_id];
    RouteEdge second_ending = list_last_element(path_to_append);
    align_route_edge(second_ending);
    current->end = second_ending->edge->end;
    list_merge(current->path, path_to_append);
    return true;
}

bool routes_replace_road(RoutesList routes, Road road_to_extend) {
    List routes_to_replace = empty_list();
    if (!routes_to_replace) return false;

    FOREACH(it, road_to_extend->routes_passing) {
        assert(get_value(it));
        Iterator edge_it = get_value(it);

        RouteEdge current_edge = get_value(edge_it);
        align_route_edge(current_edge);
        City source = current_edge->edge->begin;
        City destination = current_edge->edge->end;

        bool mem = true;
        uint64_t placeholder1;
        int placeholder2;
        Path replacement = get_shortest_path(source, destination,
                                             routes_get_path(routes,
                                                             current_edge->
                                                                     route_id),
                                             &mem, &placeholder1,
                                             &placeholder2, true);
        FOREACH(inner_edge_it, replacement) {
            RouteEdge inner_edge = get_value(inner_edge_it);
            inner_edge->route_id = current_edge->route_id;
        }

        if (!mem || !replacement || !reserve_back(routes_to_replace)) {
            FOREACH(delete_it, routes_to_replace) {
                assert(get_value(delete_it));
                Path to_delete = get_value(delete_it);
                list_shallow_clear(to_delete);
            }

            list_shallow_clear(routes_to_replace);
            free(routes_to_replace);
            return false;
        }

        emplace_back(routes_to_replace, replacement);
    }


    Iterator route_edges_it = list_begin(road_to_extend->routes_passing);
    FOREACH(it, routes_to_replace) {
        assert(get_value(route_edges_it));
        assert(get_value(it));
        Iterator to_delete = get_value(route_edges_it);
        RouteEdge current_edge = get_value(to_delete);

        List to_insert = get_value(it);

        list_replace_at(routes_get_path(routes, current_edge->route_id),
                        to_delete, to_insert);
        FOREACH(it_to_update_route, to_insert) {
            RouteEdge inner_edge = get_value(it_to_update_route);
            emplace_back(inner_edge->edge->routes_passing, it_to_update_route);
        }

        free(current_edge);
        free(to_insert);
        route_edges_it = next(route_edges_it);
    }

    list_iters_clear(routes_to_replace);
    free(routes_to_replace);
    return true;
}

void routes_delete_road(Road road_to_remove) {
    list_iters_clear(road_to_remove->routes_passing);
    free(road_to_remove->routes_passing);
    road_to_remove->routes_passing = empty_list();
}

char const *routes_generate_description(RoutesList routes, unsigned route_id) {
    if (!routes_exists(routes, route_id))
        return calloc(1, sizeof(char));

    char buf[20];
    sprintf(buf, "%u", route_id);

    size_t result_length = strlen(buf);

    Path route = routes_get_path(routes, route_id);

    FOREACH(it, route) {
        assert(get_value(it));
        RouteEdge current_edge = get_value(it);
        Road road_to_add = current_edge->edge;
        City edge_start = current_edge->start;

        result_length += 3; //średniki
        result_length += strlen(edge_start->city_name);
        sprintf(buf, "%u", road_to_add->length);
        result_length += strlen(buf);
        sprintf(buf, "%d", road_to_add->built_year);
        result_length += strlen(buf);
    }
    result_length += strlen(routes->routes[route_id]->end->city_name) + 1;

    char *result = calloc(result_length + 2, sizeof(char));
    if (!result)
        return NULL;

    sprintf(buf, "%u", route_id);
    strcat(result, buf);

    FOREACH(it, route) {
        assert(get_value(it));
        RouteEdge current_edge = get_value(it);
        Road road_to_add = current_edge->edge;
        City edge_start = current_edge->start;

        strcat(result, ";");
        strcat(result, edge_start->city_name);

        strcat(result, ";");
        sprintf(buf, "%u", road_to_add->length);
        strcat(result, buf);

        strcat(result, ";");
        sprintf(buf, "%d", road_to_add->built_year);
        strcat(result, buf);
    }

    strcat(result, ";");
    strcat(result, routes->routes[route_id]->end->city_name);
    assert(strlen(result) == result_length);

    return result;
}

void routes_wipeout(RoutesList routes) {
    for (size_t i = 0; i < MAXIMUM_ROUTES_AMOUNT; ++i) {
        if (routes->routes[i]) {
            list_shallow_clear(routes->routes[i]->path);
            free(routes->routes[i]->path);
            routes->routes[i]->path = NULL;
            free(routes->routes[i]);
            routes->routes[i] = NULL;
        }
    }
    free(routes);
}

static void erase_from_passing(Road c, Iterator list_it) {
    Iterator prev = NULL;
    for (Iterator it = list_begin(c->routes_passing); it;) {
        assert(get_value(it));
        Iterator current = get_value(it);
        if (current == list_it) {
            if (list_begin(c->routes_passing) == list_end(c->routes_passing))
                clean_list(c->routes_passing);
            else if (list_begin(c->routes_passing) == it) {
                list_set_begin(c->routes_passing, next(it));
            }
            else if (list_end(c->routes_passing) == it) {
                list_set_end(c->routes_passing, prev);
            }
            list_remove_at(prev, it);
            free(it);
            break;
        }
        prev = it;
        it = next(it);
    }
}

void routes_remove_id(RoutesList routes, unsigned route_id) {
    FOREACH(it, routes->routes[route_id]->path) {
        RouteEdge e = get_value(it);
        erase_from_passing(e->edge, it);
    }

    list_shallow_clear(routes->routes[route_id]->path);
    free(routes->routes[route_id]->path);
    routes->routes[route_id]->path = NULL;
    free(routes->routes[route_id]);
    routes->routes[route_id] = NULL;
}