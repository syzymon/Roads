#include "map.h"
#include "hashtable.h"
#include "routes.h"

struct Map {
    Hashmap vertices_table;
    RoutesList routes;
};


Map *newMap() {
    Map *map = calloc(1, sizeof(struct Map));
    if (map) {
        map->vertices_table = hashmap_empty();
        map->routes = routes_empty_list();
    }
    return map;
}

void deleteMap(Map *map) {
    if (map) {
        hashmap_wipeout(map->vertices_table);
        routes_wipeout(map->routes);
        free(map);
    }
}

bool addRoad(Map *map, const char *city1, const char *city2,
             unsigned length, int builtYear) {
    if (length == 0 || !strcmp(city1, city2) || !builtYear) return false;

    City v1 = create_vertex_by_name(map->vertices_table, city1);

    City v2 = create_vertex_by_name(map->vertices_table, city2);

    if (get_road(v1, v2)) {
        hashmap_clean_changes(map->vertices_table);
        return false;
    }

    Road new_road = make_road(v1, v2, length, builtYear);
    if (!new_road || !populate_road(new_road) ||
        !hashmap_commit(map->vertices_table)) {
        if (new_road) {
            erase_road(new_road);
        }
        hashmap_clean_changes(map->vertices_table);
        return false;

    }
    return true;
}

bool repairRoad(Map *map, const char *city1, const char *city2,
                int repairYear) {
    if (!strcmp(city1, city2) || !repairYear) return false;

    City v1 = get_vertex_by_name(map->vertices_table, city1);
    City v2 = get_vertex_by_name(map->vertices_table, city2);

    Road r;
    if (!v1 || !v2 || !(r = get_road(v1, v2)))
        return false;
    return update_year(r, repairYear);
}

bool newRoute(Map *map, unsigned routeId,
              const char *city1, const char *city2) {
    if (routeId < 1 || routeId > 999 || !strcmp(city1, city2) ||
        routes_exists(map->routes, routeId)) {
        return false;
    }

    City v1 = get_vertex_by_name(map->vertices_table, city1);
    City v2 = get_vertex_by_name(map->vertices_table, city2);

    if (!v1 || !v2) return false;

    Path new_route;
    bool mem = true;
    if (!(new_route = get_shortest_path(v1, v2, NULL, &mem)))
        return false;

    if (!routes_add_with_id(map->routes, routeId, new_route))
        return false;

    return true;
}

bool extendRoute(Map *map, unsigned routeId, const char *city) {
    if (routeId < 1 || routeId > 999 || !routes_exists(map->routes, routeId))
        return false;

    City new_end;
    if (!(new_end = get_vertex_by_name(map->vertices_table, city)))
        return false;

    if (routes_vertex_belongs_to(map->routes, routeId, new_end))
        return false;

    Path route_to_extend = routes_get_path(map->routes, routeId);

    bool mem1 = true, mem2 = true;

    City route_begin = routes_get_begin(map->routes, routeId);
    Path starting_from_begin = get_shortest_path(route_begin,
                                                 new_end, route_to_extend,
                                                 &mem1);

    if (!mem1)
        return false;

    Weight length_from_begin = get_vertex_weight(route_begin);

    City route_end = routes_get_end(map->routes, routeId);
    Path starting_from_end = get_shortest_path(route_end,
                                               new_end, route_to_extend, &mem2);

    if (!mem2) {
        list_shallow_clear(starting_from_begin);
        free(starting_from_begin);
        return false;
    }
    else if (!starting_from_begin && !starting_from_end)
        return false;
    else if (!starting_from_end)
        return routes_add_prefix(map->routes, routeId, starting_from_end);
    else if (!starting_from_begin)
        return routes_add_suffix(map->routes, routeId, starting_from_end);

    int compare_from_begin = weight_compare(length_from_begin,
                                            get_vertex_weight(route_end));

    if (compare_from_begin == 0)
        return false;
    else if (compare_from_begin < 0) {
        list_shallow_clear(starting_from_end);
        free(starting_from_end);
        return routes_add_prefix(map->routes, routeId, starting_from_begin);
    }
    else {
        list_shallow_clear(starting_from_begin);
        free(starting_from_begin);
        return routes_add_suffix(map->routes, routeId, starting_from_end);
    }
}

bool removeRoad(Map *map, const char *city1, const char *city2) {
    if (!strcmp(city1, city2)) return false;

    City v1 = get_vertex_by_name(map->vertices_table, city1);
    City v2 = get_vertex_by_name(map->vertices_table, city2);

    Road road_to_remove;
    if (!v1 || !v2 || !(road_to_remove = get_road(v1, v2))) return false;

    if (!routes_replace_road(map->routes, road_to_remove))
        return false;
    else
        routes_delete_road(road_to_remove);

    erase_road(road_to_remove);

    return true;
}

char const *getRouteDescription(Map *map, unsigned routeId) {
    return routes_generate_description(map->routes, routeId);
}