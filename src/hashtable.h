#ifndef __HASHTABLE_H__
#define __HASHTABLE_H__

#include <string.h>
#include "graph.h"

typedef struct Hashtable *Hashmap;

Hashmap hashmap_empty();

City create_vertex_by_name(Hashmap hashmap, const char *city_name);

City get_vertex_by_name(Hashmap hashmap, const char *city_name);

void hashmap_clean_changes(Hashmap hashmap);

bool hashmap_commit(Hashmap hashmap);

void hashmap_wipeout(Hashmap hashmap);

#endif /* __HASHTABLE_H__ */