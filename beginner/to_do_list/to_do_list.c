#include <stdio.h>
#include <stdlib.h>

#define MAX_MAP_SIZE 100

typedef struct
{
    int key;
    char value[100];
} MapEntry;

typedef struct
{
    MapEntry entries[MAX_MAP_SIZE];
    int size;
} SimpleMap;

void put(SimpleMap *map, int key, const char *value)
{
    for (int i = 0; i < map->size; i++)
    {
        if (map->entries[i].key == key)
        {
            strcpy(map->entries[i].value, value);
            return;
        }
    }
    if (map->size < MAX_MAP_SIZE)
    {
        map->entries[map->size].key = key;
        strcpy(map->entries[map->size].value, value);
        map->size++;
    }
}

const char *get(SimpleMap *map, int key)
{
    for (int i = 0; i < map->size; i++)
    {
        if (map->entries[i].key == key)
        {
            return map->entries[i].value;
        }
    }
    return NULL;
}

int main()
{

    return 0;
}