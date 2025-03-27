#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

const int containsKey(SimpleMap *map, int key)
{
    for (int i = 0; i < map->size; i++)
    {
        if (map->entries[i].key == key)
        {
            return 1;
        }
    }
    return 0;
}

int main()
{

    int choice = 0;
    SimpleMap map = {0};
    do
    {
        printf("\n\t\t\t\tEnter 1 to add a task\n\t\t\t\tEnter 2 to view all tasks\n\t\t\t\tEnter to 3 delete a task\n\t\t\t\tEnter 4 to exit\n\t\t\t\t");
        scanf("%d", &choice);
        switch (choice)
        {
        case 1:
        {
            int key;
            char value[100];
            printf("\n\t\t\t\tEnter task number: ");
            scanf("%d", &key);
            getchar(); // clear newline left by scanf
            printf("\t\t\t\tEnter task description: ");
            fgets(value, sizeof(value), stdin);
            value[strcspn(value, "\n")] = 0; // remove the newline from input
            if (containsKey(&map, key))
            {
                printf("\n\t\t\t\tTask already exists. Do you want to replace it[Y/n]?");
                char ch;
                scanf("%c", &ch);
                if (ch == 'n' || ch == 'N')
                {
                    break;
                }
            }
            put(&map, key, value);
            break;
        }
        case 2:
        {
            if (map.size == 0)
            {
                printf("\n\t\t\t\tNo tasks added yet\n");
                break;
            }
            printf("\n\t\t\t\tTasks:\n");
            for (int i = 0; i < map.size; i++)
            {
                printf("\n\t\t\t\tTask %d: %s\n", map.entries[i].key, map.entries[i].value);
            }
            break;
        }
        case 3:
            break;
        case 4:
            break;    
        default:
            printf("\n\t\t\t\tInvalid choice\n");
            break;
        }
    } while (choice != 4);

    return 0;
}