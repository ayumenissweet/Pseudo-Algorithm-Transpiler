#include "../include/map.h"
#include "../include/hash.h"

static char* dupKey(const char* str){
    size_t len = strlen(str) + 1;
    char* copy = malloc(len);
    if(!copy) return NULL;
    memcpy(copy, str, len);
    return copy;
}

Hashmap initMap(){
    Hashmap map;
    map.heads = malloc(sizeof(Node*) * 16);
    map.loadFactor = 0;
    map.length = 16;
    for(int i = 0; i < 16; i++){
        map.heads[i] = NULL;
    }
    return map;
}

Hashmap resizeMap(Hashmap ogMap){
    Hashmap map;
    int length = ogMap.length;
    map.heads = malloc(sizeof(Node*) * length * 2);
    map.loadFactor = ogMap.loadFactor;
    map.length = length * 2;
    for(int i = 0; i < length * 2; i++){
        map.heads[i] = NULL;
    }

    for(int i = 0; i < length; i++){
        if(!ogMap.heads[i]) continue;
        Node* p = ogMap.heads[i];
        while(p != NULL){
            uint64_t newHash = getRandomIndex(hash(p->key), length * 2);
            Node* temp = p->next;
            p->next = map.heads[newHash];
            map.heads[newHash] = p;
            p = temp;
        }
    }

    free(ogMap.heads);
    return map;
}

Node* createNode(const char* str, float value){
    Node* p = (Node*) malloc(sizeof(Node));
    if(!p) {printf("Error allocating\n"); return NULL;}
    p->key = dupKey(str);
    if(!p->key){
        printf("Error allocating\n");
        free(p);
        return NULL;
    }
    p->next = NULL;
    p->value = value;
    return p;
}

Node* findNode(Node* head, const char* str){
    while(head != NULL){
        if(!strcmp(head->key, str)){
            return head;
        }
        head = head->next;
    }
    return NULL;
}

Node* getTail(Node* head){
    while(head->next != NULL){
        head = head->next;
    }
    return head;
}

float* get(Hashmap map, const char* str){
    int index = getRandomIndex(hash(str), map.length);
    if(!map.heads[index]) return NULL;
    Node* found = findNode(map.heads[index], str);
    return found ? &(found->value) : NULL;
}

void set(Hashmap* map, const char* str, float value){
    int index = getRandomIndex(hash(str), map->length);
    if(!(*map).heads[index]){  // bucket empty
        (*map).heads[index] = createNode(str, value);
        if(!(*map).heads[index]) return;
        (*map).loadFactor++;
        if(((double)map->loadFactor / map->length) >= 0.75){
            (*map) = resizeMap(*map);
        }
    }else if(!strcmp((*map).heads[index]->key, str)){
        (*map).heads[index]->value = value;
    }else{
        Node* p = (*map).heads[index];
        while(p != NULL){
            if(!strcmp(p->key, str)){ // key found, replace
                p->value = value;
                return;
            }
            p = p->next;
        }
        // key not found in chain: append at tail
        Node* newNode = createNode(str, value);
        if(!newNode) return;
        getTail((*map).heads[index])->next = newNode;
        map->loadFactor++;
        if(((double)map->loadFactor / map->length) >= 0.75){
            (*map) = resizeMap(*map);
        }
    }
}

void removeMap(Hashmap *map, const char* str){
    int index = getRandomIndex(hash(str), map->length);
    if(!(*map).heads[index]) return;

    if(!strcmp((*map).heads[index]->key, str)){
        Node* p = map->heads[index];
        map->heads[index] = p->next;
        free(p->key);
        free(p);
        map->loadFactor--;
        return;
    }

    Node* p = map->heads[index]->next;
    Node* prev = map->heads[index];
    while(p != NULL){
        if(!strcmp(p->key, str)){
            prev->next = p->next;
            free(p->key);
            free(p);
            map->loadFactor--;
            return;
        }
        prev = p;
        p = p->next;
    }
}

int size(Hashmap map){
    return map.loadFactor;
}

bool containsKey(Hashmap map, const char* str){
    int index = getRandomIndex(hash(str), map.length);
    if(!map.heads[index]) return false;
    return findNode(map.heads[index], str) != NULL;
}

bool containsValue(Hashmap map, float value){
    for(int i = 0; i < map.length; i++){
        if(!map.heads[i]) continue;
        Node* p = map.heads[i];
        while(p != NULL){
            if(p->value == value) return true;
            p = p->next;
        }
    }
    return false;
}

void displayMap(Hashmap map){
    for(int i = 0; i < map.length; i++){
        if(!map.heads[i]) continue;
        Node* p = map.heads[i];
        while(p != NULL){
            printf("[%d] : %s, %g", i, p->key, p->value);
            if(p->next)
                printf(" -> ");
            p = p->next;
        }
        printf("\n");
    }
    printf("%d elements\n", map.loadFactor);
}

void freeMap(Hashmap *map){
    for(int i = 0; i < map->length; i++){
        Node* p = map->heads[i];
        while(p != NULL){
            Node* next = p->next;
            free(p->key);
            free(p);
            p = next;
        }
    }
    free(map->heads);
    map->heads = NULL;
    map->length = 0;
    map->loadFactor = 0;
}

Hashmap initMapArray_impl(MapEntry *entries, size_t count){
    Hashmap map = initMap();
    for(size_t i = 0; i < count; i++){
        set(&map, entries[i].key, entries[i].value);
    }
    return map;
}

Iterator iterator_init(Hashmap *map){
    return (Iterator){
        .map = map,
        .current = map->heads[0],
        .head = 0
    };
}

Node* iter_next(Iterator *iter){
    Node* current = iter->current;
    while(current == NULL && ++iter->head < iter->map->length){
        current = iter->map->heads[iter->head];
    }
    if(current != NULL){
        iter->current = current->next;
    }
    return current;
}
