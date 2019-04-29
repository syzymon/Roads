#include "shortest_paths.h"

#ifndef __ROUTES_H__
#define __ROUTES_H__

typedef struct Routes *RoutesList;

RoutesList routes_empty_list();

bool routes_exists(RoutesList routes, unsigned route_id);

bool routes_vertex_belongs_to(RoutesList routes, unsigned route_id,
                              City vertex);

City routes_get_begin(RoutesList routes, unsigned route_id);

City routes_get_end(RoutesList routes, unsigned route_id);

Path routes_get_path(RoutesList routes, unsigned route_id);

bool routes_add_with_id(RoutesList routes, unsigned route_id,
                        Path route_to_add);

bool routes_add_prefix(RoutesList routes, unsigned route_id,
                       Path path_to_append);

bool routes_add_suffix(RoutesList routes, unsigned route_id,
                       Path path_to_append);

bool routes_replace_road(RoutesList routes, Road road_to_extend);

//deletes road_to_delete, also from adj. lists, frees memory, commits replacements
void routes_delete_road(Road road_to_delete);

char const *routes_generate_description(RoutesList routes, unsigned route_id);

void routes_wipeout(RoutesList routes);

#endif /* __ROUTES_H__ */