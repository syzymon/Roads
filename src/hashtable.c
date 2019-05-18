#include "hashtable.h"

#define INITIAL_SIZE 16
#define LOAD_FACTOR 0.75

struct Hashtable {
    size_t capacity; //current size of table
    size_t size; //current number of elements inserted
    size_t last_alloc_index;
    List *table;
    List pending_additions; //items waiting to commit
};

struct MapNode {
    uint64_t hash;
    City vertex;
};

typedef struct MapNode *Node;

Hashmap hashmap_empty() {
    Hashmap new_map = calloc(1, sizeof(struct Hashtable));

    if (!new_map) return NULL;

    new_map->table = calloc(INITIAL_SIZE, sizeof(List));

    bool allocated_memory = new_map->table;

    if (new_map->table) {
        for (size_t i = 0; i < INITIAL_SIZE; ++i) {
            if (!(new_map->table[i] = empty_list())) {
                allocated_memory = false;
                break;
            }
        }
    }

    new_map->pending_additions = empty_list();

    if (!new_map->table || !new_map->pending_additions || !allocated_memory) {
        if (new_map->pending_additions) {
            free(new_map->pending_additions);
            new_map->pending_additions = NULL;
        }

        if (!allocated_memory) {
            for (size_t i = 0; i < INITIAL_SIZE; ++i)
                if (new_map->table[i])
                    clean_list(new_map->table[i]);
        }

        if (new_map->table) {
            free(new_map->table);
            new_map->table = NULL;
        }
        free(new_map);
        return NULL;
    }

    new_map->capacity = INITIAL_SIZE;
    new_map->size = 0;
    new_map->last_alloc_index = new_map->capacity - 1;

    return new_map;
}


static uint64_t generate_hash(const char *city_name, bool *correct) {
    uint64_t hash = 5381;
    if (!strcmp(city_name, "")) {
        *correct = false;
        return 0;
    }

    int current_character;
    while ((current_character = *city_name++)) {
        if (current_character == ';' ||
            (current_character >= 0 && current_character <= 31)) {
            *correct = false;
            break;
        }
        hash += (hash << 5LLU) + (uint8_t) current_character;
    }

    return hash;
}

#define index_of_hash(hashmap, hash) ((hash) & ((hashmap)->capacity - 1))

static Node
find_node_by_hash(Hashmap hashmap, uint64_t hash, const char *city_name) {
    size_t index = index_of_hash(hashmap, hash);

    assert(hashmap->table[index]);

    FOREACH(it, hashmap->table[index]) {
        assert(get_value(it));

        Node current_element = get_value(it);
        if (current_element->hash == hash &&
            !strcmp(city_name, current_element->vertex->city_name)) {
            return current_element;
        }
    }
    return NULL;
}

static Node find_node(Hashmap hashmap, const char *city_name, uint64_t *hash,
                      bool *correct) {
    *hash = generate_hash(city_name, correct);
    if (!(*correct))
        return NULL;
    return find_node_by_hash(hashmap, *hash, city_name);
}

City create_vertex_by_name(Hashmap hashmap, const char *city_name) {
    bool correct = true;
    uint64_t hash;
    Node found = find_node(hashmap, city_name, &hash, &correct);

    if (found)
        return found->vertex;

    if (!correct || !reserve_back(hashmap->pending_additions))
        return NULL;

    found = calloc(1, sizeof(struct MapNode));
    char *persistent_name = save_vertex_name(city_name);
    City c = make_vertex(persistent_name);

    if (!found || !persistent_name || !c) {
        if (found)
            free(found);
        if (persistent_name)
            free(persistent_name);
        unreserve_back(hashmap->pending_additions);
        return NULL;
    }

    found->vertex = c;
    found->hash = hash;

    emplace_back(hashmap->pending_additions, found);
    return c;
}

City get_vertex_by_name(Hashmap hashmap, const char *city_name) {
    bool correct = true;
    uint64_t hash;
    Node found = find_node(hashmap, city_name, &hash, &correct);
    if (!found)
        return NULL;
    return found->vertex;
}

static bool needs_rehash(Hashmap hashmap) {
    return hashmap->last_alloc_index + 1 != hashmap->capacity ||
           hashmap->capacity * LOAD_FACTOR < hashmap->size;
}

static bool allocate(Hashmap hashmap) {
    while (hashmap->last_alloc_index + 1 < hashmap->capacity) {
        hashmap->table[hashmap->last_alloc_index +
                       1] = empty_list();
        if (!hashmap->table[hashmap->last_alloc_index + 1])
            return false;
        hashmap->last_alloc_index += 1;
    }
    return true;
}


static bool rehash(Hashmap hashmap) {
    if (hashmap->last_alloc_index + 1 != hashmap->capacity &&
        !allocate(hashmap)) {
        return false;
    }
    size_t new_capacity = hashmap->capacity * 2;
    List *tmp = realloc(hashmap->table,
                        sizeof(List) * new_capacity);
    if (!tmp)
        return false;

    hashmap->table = tmp;
    hashmap->capacity = new_capacity;

    if (!allocate(hashmap))
        return false;

    hashmap->capacity /= 2;


    for (size_t i = 0; i < hashmap->capacity; ++i) {
        assert(hashmap->table[i]);

        Iterator prev = NULL;
        Iterator new_begin = NULL;
        for (Iterator it = list_begin(hashmap->table[i]); it;) {
            Iterator next_iterator = next(it);

            Node to_move = get_value(it);

            if (i != (to_move->hash & (new_capacity - 1))) {
                list_remove_at(prev, it);

                push_back(hashmap->table[i + hashmap->capacity], it);
            }
            else {
                if (!prev)
                    new_begin = it;
                prev = it;
            }

            it = next_iterator;
        }

        list_set_begin(hashmap->table[i], new_begin);
        list_set_end(hashmap->table[i], prev);

    }

    hashmap->capacity = new_capacity;

    return true;
}

static void clean_map_nodes_list(List nodes_list) {
    for (Iterator it = list_begin(nodes_list); it;) {
        Iterator next_iterator = next(it);
        assert(get_value(it));
        Node to_delete = get_value(it);

        erase_vertex(to_delete->vertex);

        free(to_delete);
        free(it);

        it = next_iterator;
    }
}


void hashmap_clean_changes(Hashmap hashmap) {
    clean_map_nodes_list(hashmap->pending_additions);
    clean_list(hashmap->pending_additions);
}

bool hashmap_commit(Hashmap hashmap) {
    if (needs_rehash(hashmap) && !rehash(hashmap)) {
        return false;
    }

    for (Iterator it = list_begin(hashmap->pending_additions); it;) {
        Iterator next_iterator = next(it);

        Node to_add = get_value(it);
        size_t index = index_of_hash(hashmap, to_add->hash);

        if (!hashmap->table[index])
            hashmap->table[index] = empty_list();
        assert(hashmap->table[index]);

        push_back(hashmap->table[index], it);

        ++hashmap->size;

        if (!next_iterator) break;

        assert(get_value(next_iterator));

        it = next_iterator;
    }
    clean_list(hashmap->pending_additions);
    return true;
}


void hashmap_wipeout(Hashmap hashmap) {
    hashmap_clean_changes(hashmap);
    free(hashmap->pending_additions);
    hashmap->pending_additions = NULL;

    for (size_t i = 0; i < hashmap->capacity; ++i) {
        clean_map_nodes_list(hashmap->table[i]);
        free(hashmap->table[i]);
        hashmap->table[i] = NULL;
    }

    free(hashmap->table);
    hashmap->table = NULL;
    free(hashmap);
}

