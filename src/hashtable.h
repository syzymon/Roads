#include <string.h>
#include "graph.h"

#ifndef DROGI_HASHTABLE_H
#define DROGI_HASHTABLE_H

#endif //DROGI_HASHTABLE_H

typedef struct Hashtable *Hashmap;
//contains list of all vertices, and table of lists of vertices with hash

Hashmap hashmap_empty();

//NULL when alloc fails or forbidden characters in city_name
City create_vertex_by_name(Hashmap hashmap, const char *city_name);

City get_vertex_by_name(Hashmap hashmap, const char *city_name);
// TODO return false when city_name empty str

void hashmap_clean_changes(Hashmap hashmap);

bool hashmap_commit(Hashmap hashmap);

void hashmap_wipeout(Hashmap hashmap);

//uint64_t generate_hash(const char *city_name, bool *correct);
