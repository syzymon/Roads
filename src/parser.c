#include "parser.h"

#define SEP ";"

static long long str_to_int(const char *s) {
    if (!strcmp(s, ""))
        return 0;
    size_t l = strlen(s);
    if (l == 0 || !(isdigit(s[0]) || s[0] == '-'))
        return 0;
    size_t nonzero_length = 0;
    for (size_t i = 0; i < l; ++i) {
        if ((i != 0 || s[0] != '-') && !isdigit(s[i]))
            return 0;
        if (nonzero_length > 0 || s[i] != '0')

            ++nonzero_length;
    }
    if (nonzero_length <= 11)
        return strtoll(s, NULL, 10);
    return 0;
}

int read_args(char **args, size_t k, bool end) {
    for (size_t i = 0; i < k; ++i) {
        args[i] = strtok(NULL, SEP);
        if (args[i] == NULL)
            return 2;
    }
    if (end && strtok(NULL, SEP) != NULL)
        return 1;
    return 0;
}

static int convert_year(const char *year_str) {
    long long candidate = str_to_int(year_str);
    if (candidate < INT_MIN || candidate > INT_MAX)
        return 0;
    return candidate;
}

static unsigned convert_distance(const char *dist_str) {
    long long candidate = str_to_int(dist_str);
    if (candidate < 1 || candidate > UINT_MAX)
        return 0;
    return candidate;
}

static void set_null(int k, char **arr) {
    for (int i = 0; i < k; ++i)
        arr[i] = NULL;
}

static bool correct_name(const char *city_name) {
    if (!strcmp(city_name, ""))
        return false;

    int current_character;
    while ((current_character = *city_name++)) {
        if (current_character == ';' ||
            (current_character >= 0 && current_character <= 31)) {
            return false;
        }
    }
    return true;
}

static int str_compare(const void *a, const void *b) {
    return (strcmp(*((const char **) a), *((const char **) b)));
}


static int add_route(Map *m, int route_nr) {
    char *prev[3], *current[3];
    set_null(3, prev);
    set_null(3, current);
    if (read_args(prev, 3, false) == 2 || !correct_name(prev[0]))
        return 1;

    int result = 0;

    List to_insert = empty_list();
    if (to_insert == NULL)
        return LINE_MEM_ERR;

    size_t arr_len = 1;
    do {
        set_null(3, current);
        read_args(current, 3, false);
        if (!current[0] || !correct_name(current[0])) {
            result = 1;
            break;
        }

        ++arr_len;

        RoadData data = calloc(1, sizeof(struct RoadData));
        if (data == NULL) {
            result = LINE_MEM_ERR;
            break;
        }

        data->city1 = prev[0];
        data->city2 = current[0];
        data->length = convert_distance(prev[1]);
        data->builtYear = convert_year(prev[2]);

        if (!emplace_back(to_insert, data)) {
            result = LINE_MEM_ERR;
            break;
        }

        memcpy(prev, current, sizeof current);
    } while (current[2] != NULL);

    if (!result && !current[1]) {
        //We sort all cities identifiers, to ensure they are unique on a path
        char *arr[arr_len];
        size_t index = 0;
        arr[index++] = ((RoadData) list_first_element(to_insert))->city1;
        FOREACH(it, to_insert) arr[index++] = ((RoadData) get_value(it))->city2;
        qsort(arr, arr_len, sizeof(char *), str_compare);
        for (size_t i = 0; i < arr_len - 1; ++i) {
            if (!strcmp(arr[i], arr[i + 1])) {
                result = 1;
                break;
            }
        }
        if (!result)
            result = !addRoute(m, route_nr, to_insert);
    }
    else
        result = 1;
    list_shallow_clear(to_insert);
    free(to_insert);
    return result;
}

static int add_road(Map *m) {
    char *args[4];
    if (read_args(args, 4, true))
        return 1;

    unsigned length = convert_distance(args[2]);
    int built_year = convert_year(args[3]);
    return addRoad(m, args[0], args[1], length, built_year) == false ? 1 : 0;
}

static int repair_road(Map *m) {
    char *args[3];
    if (read_args(args, 3, true))
        return 1;

    int repair_year = convert_year(args[2]);
    return repairRoad(m, args[0], args[1], repair_year) == false ? 1 : 0;
}

static bool is_zero(const char *s) {
    size_t l = strlen(s);
    for (size_t i = 0; i < l; ++i)
        if (s[i] != '0')
            return false;
    return true;
}

static int get_route_description(Map *m) {
    char *args[1];
    if (read_args(args, 1, true))
        return 1;

    long long candidate = str_to_int(args[0]);
    if (candidate >= 1 && candidate <= 999) {
        char *result = (char *) getRouteDescription(m, candidate);
        if (result != NULL) {
            printf("%s\n", result);
            free(result);
            return 0;
        }
    }
    else if ((candidate >= 1 && candidate <= UINT_MAX) || is_zero(args[0])) {
        puts("");
        return 0;
    }
    return 1;
}

static int execute_command(char *command_str, Map *m) {
    char *command_name = strtok(command_str, SEP);
    if (command_name == NULL)
        return 1;
    if (!strcmp(command_name, "addRoad"))
        return add_road(m);
    else if (!strcmp(command_name, "repairRoad"))
        return repair_road(m);
    else if (!strcmp(command_name, "getRouteDescription"))
        return get_route_description(m);
    else {
        long long candidate = str_to_int(command_name);
        if (candidate >= 1 && candidate <= 999)
            return add_route(m, (int) candidate);
    }
    return 1;
}

int read_line(Map *m) {
    char *line = NULL, *to_free = NULL;
    size_t linesize = 0;
    ssize_t len = getline(&line, &linesize, stdin);

    if (ferror(stdin)) { //Check if getline failed to allocate memory
        free(line);
        return LINE_MEM_ERR;
    }

    to_free = line;

    int result = 2; //sentinel
    if (len == -1)
        result = LINE_EOF;
    else if ((len >= 1 && line[0] == '#') || (len == 1 && line[0] == '\n'))
        result = LINE_OK;
    else if (len > 0 && line[len - 1] != '\n')
        result = LINE_ERR;
    else {
        if ((line[0] == SEP[0] || (len >= 2 && line[len - 2] == SEP[0])))
            result = LINE_ERR;

        //Zero character guarantees incorrect input.
        for (int i = 0; i < len - 1; ++i) {
            if (line[i] == 0) {
                result = LINE_ERR;
                break;
            }
        }

        //No way to have a correct input with double separator.
        for (int i = 0; i < len - 2; ++i) {
            if (line[i] == SEP[0] && line[i + 1] == SEP[0]) {
                result = LINE_ERR;
                break;
            }
        }
    }

    if (result != 2) {
        free(to_free);
        return result;
    }

    line[len - 1] = '\0';
    result = execute_command(line, m);
    free(to_free);
    return result;
}
