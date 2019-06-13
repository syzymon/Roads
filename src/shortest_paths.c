#include "shortest_paths.h"

#define min(a, b) (a) < (b) ? (a) : (b)

const uint64_t INF = ((uint64_t) 1) << 60llu;


void initialize_vertex(City v, size_t iteration, bool dest) {
    if (v->path_data.iteration == iteration && !dest) return;
    v->path_data.iteration = iteration;
    v->path_data.min_year = INT_MAX;
    v->path_data.distance = INF;
    v->path_data.blocked = false;
    v->path_data.queued = false;
    v->path_data.doubled = false;
}

void clean_path(Path p) {
    list_shallow_clear(p);
    free(p);
}

static Path one_sided_shortest_path(City source, City dest, Path not_allowed,
                                    bool *mem, uint64_t *dst, int *year,
                                    bool remove) {
    static size_t call_number = 0;
    ++call_number;

    FOREACH(it, not_allowed) {
        assert(get_value(it));
        DirectedEdge *e = get_value(it);
        City v1 = e->edge->begin;
        City v2 = e->edge->end;
        v1->path_data.iteration = v2->path_data.iteration = call_number;
        v1->path_data.blocked = v2->path_data.blocked = true;
    }

    //Source is also contained in not_allowed, that's why exclude from blocked
    source->path_data.blocked = false;
    source->path_data.iteration = 0;

    List queue = empty_list();
    if (!queue) {
        *mem = false;
        return NULL;
    }
    initialize_vertex(dest, call_number, true);
    dest->path_data.distance = 0;
    if (!emplace_back(queue, dest)) {
        list_shallow_clear(queue);
        free(queue);
        *mem = false;
        return NULL;
    }

    while (!is_empty(queue)) {
        City v1 = pop_front(queue);
        v1->path_data.queued = false;

        FOREACH(it, v1->neighbours) {
            assert(get_value(it));
            Road edge = get_value(it);
            orientate_road(edge, v1);

            City v2 = edge->end;

            initialize_vertex(v2, call_number, false);

            if (!v2->path_data.blocked &&
                (!not_allowed || !(v1 == dest && v2 == source && remove))) {
                uint64_t dist = v1->path_data.distance + edge->length;
                int min_edge = min(v1->path_data.min_year, edge->built_year);
                int compares = operator_less(dist, min_edge,
                                             v2->path_data.distance,
                                             v2->path_data.min_year);
                if (compares < 0) {
                    v2->path_data.distance = dist;
                    v2->path_data.min_year = min_edge;
                    v2->path_data.doubled = v1->path_data.doubled;
                    v2->path_data.backward = edge;
                }
                if (compares < 0 || (!compares && !v2->path_data.doubled)) {
                    if (!compares)
                        v2->path_data.doubled = true;
                    if (!v2->path_data.queued) {
                        v2->path_data.queued = true;
                        if (!emplace_back(queue, v2)) {
                            list_shallow_clear(queue);
                            free(queue);
                            *mem = false;
                            return NULL;
                        }
                    }
                }
            }
        }
    }
    free(queue);

    if (source->path_data.iteration != call_number ||
        source->path_data.distance == INF || source->path_data.doubled)
        return NULL;

    *dst = source->path_data.distance;
    *year = source->path_data.min_year;

    Path result = empty_list();
    if (!result) {
        *mem = false;
        return false;
    }

    City cur_begin = source;
    while (cur_begin != dest) {
        Road path_road = cur_begin->path_data.backward;
        orientate_road(path_road, cur_begin);

        DirectedEdge *route_road = calloc(1, sizeof(DirectedEdge));
        if (!route_road || !reserve_back(result)) {
            if (route_road)
                free(route_road);
            clean_path(result);
            return false;
        }

        route_road->start = cur_begin;
        route_road->edge = path_road;
        emplace_back(result, route_road);

        cur_begin = path_road->end;
    }
    return result;
}


Path get_shortest_path(City source, City dest, Path not_allowed, bool *mem,
                       uint64_t *dist, int *year, bool remove) {
    Path source_dest = one_sided_shortest_path(source, dest, not_allowed, mem,
                                               dist, year, remove);

    Path dest_source = one_sided_shortest_path(dest, source, not_allowed, mem,
                                               dist, year, remove);


    if (mem == false || !source_dest || !dest_source) {
        if (source_dest)
            clean_path(source_dest);
        else if (dest_source)
            clean_path(dest_source);
        return NULL;
    }
    clean_path(dest_source);
    return source_dest;
}
