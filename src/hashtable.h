#ifndef HASHTABLE_H_H
#define HASHTABLE_H_H

#define HT_ITEM_CAP 4
#define HT_CAP 128

#ifndef HASHTABLE_DEF
#define HASHTABLE_DEF static inline
#endif //HASHTABLE_DEF

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

typedef struct {
  void *value;
  size_t value_size;
  
  char *key;
  size_t key_size;
}Ht_Entry;

typedef struct {
  Ht_Entry *entries;
  size_t size;
  size_t count;
}Ht_Item;

typedef struct {
  Ht_Item *items;
  size_t size;
  size_t count;
}Ht;

HASHTABLE_DEF Ht *ht_init();
HASHTABLE_DEF void ht_insert(Ht *ht, const char *key, const void *value, size_t value_size);
HASHTABLE_DEF void ht_insert2(Ht *ht,
			      const char *key, size_t key_len,
			      const void *value, size_t value_size);
HASHTABLE_DEF void *ht_get(const Ht *ht, const char* key);
HASHTABLE_DEF void *ht_get2(const Ht *ht, const char* key, size_t key_len);
HASHTABLE_DEF bool ht_has(const Ht *ht, const char *key);
HASHTABLE_DEF bool ht_remove(Ht *ht, const char* key);
HASHTABLE_DEF bool ht_next(const Ht *ht, int *last, Ht_Entry **entry);
HASHTABLE_DEF void ht_free(Ht *ht);

#ifdef HASHTABLE_IMPLEMENTATION

#ifdef HT_NOTNULL

#define CUSTOM_CHECK_NOTNULL(obj, name) do{ \
  if(obj==NULL) {\
  fprintf(stderr, "ERROR: Tried to perform operation on NULL. Expected "name".\n");\
  exit(1);\
  }\
}while(0)

#define HT_ENTRY_CHECK_NOTNULL(obj) CUSTOM_CHECK_NOTNULL(obj, "Ht_Entry *")
#define HT_ITEM_CHECK_NOTNULL(obj) CUSTOM_CHECK_NOTNULL(obj, "Ht_Item *")
#define HT_CHECK_NOTNULL(obj) CUSTOM_CHECK_NOTNULL(obj, "Ht *")

#else

#define HT_ENTRY_CHECK_NOTNULL(obj)
#define HT_ITEM_CHECK_NOTNULL(obj)
#define HT_CHECK_NOTNULL(obj)

#endif

//const ht->size || !const ht_item->size
//!const ht->count
HASHTABLE_DEF bool ht_next(const Ht *ht, int *last, Ht_Entry **entry) {
  HT_CHECK_NOTNULL(ht);
  if(!entry) return false;
  if(!last) return false;
  int c = 0;
  for(size_t i=0;i<ht->size;i++) {
    if(ht->items[i].size==0) continue;
    for(size_t j=0;j<ht->items[i].count;j++) {
      if(*last < c) {
	*last = c;
	*entry = &ht->items[i].entries[j];
	return true;
      }
      c++;
    }
  }

  return false;
}

HASHTABLE_DEF void ht_entry_create(Ht_Entry *entry,
		     const char *key, size_t key_size,
		     const void *value, size_t value_size) {
  HT_ENTRY_CHECK_NOTNULL(entry);
  entry->key = (char *) malloc(key_size+1);
  if(!entry->key) {
    fprintf(stderr, "ERROR: Can not allocate enough memory: ht_entry_create\n");
    exit(1);
  }
  memcpy(entry->key, key, key_size);
  entry->key_size = key_size;

  entry->value = malloc(value_size);
  if(!entry->value) {
    fprintf(stderr, "ERROR: Can not allocate enough memory: ht_entry_create\n");
    exit(1);
  }
  memcpy(entry->value, value, value_size);
  entry->value_size = value_size;
}

HASHTABLE_DEF void ht_entry_free(Ht_Entry *entry) {
  HT_ENTRY_CHECK_NOTNULL(entry);
  if(entry->key) free(entry->key);
  if(entry->value) free(entry->value);
}

HASHTABLE_DEF void ht_item_init(Ht_Item *item) {
  HT_ITEM_CHECK_NOTNULL(item);

  item->size = HT_ITEM_CAP;
  item->count = 0;
  item->entries = (Ht_Entry *) malloc(item->size * sizeof(Ht_Entry));
  if(!item->entries) {
    fprintf(stderr, "ERROR: Can not allocate enough memory: ht_item_init\n");
    exit(1);    
  }
}

HASHTABLE_DEF void ht_item_increment(Ht_Item *item) {
  HT_ITEM_CHECK_NOTNULL(item);

  item->size *= 2;
  item->entries = (Ht_Entry *) realloc(item->entries, item->size * sizeof(Ht_Entry));
  if(!item->entries) {
    fprintf(stderr, "ERROR: Can not allocate enough memory: ht_item_inc\n");
    exit(1);    
  }  
}

HASHTABLE_DEF void ht_item_free(Ht_Item *item) {
  HT_ITEM_CHECK_NOTNULL(item);
  if(!item->entries) return;
  for(size_t i=0;i<item->count;i++) ht_entry_free(&item->entries[i]);
  free(item->entries);
}

HASHTABLE_DEF unsigned long hash_function(const char* str, size_t *size) {
  unsigned long hash = 0;
  int c = *str;

  size_t f = 0;
  while(c) {
    hash = ((hash << 5) + hash) + (unsigned long) c;
    c=*str++;
    f++;
  }

  if(size) *size = f - 1;

  return hash % HT_CAP;
}

HASHTABLE_DEF unsigned long hash_function2(const char* str, size_t str_size) {
  unsigned long hash = 0;
  int c = *str;

  while(str_size--) {
    hash = ((hash << 5) + hash) + (unsigned long) c;
    c=*str++;
  }

  return hash % HT_CAP;
}

HASHTABLE_DEF Ht* ht_init() {
  Ht *ht = (Ht *) malloc(sizeof(Ht));
  if(!ht) {
    fprintf(stderr, "ERROR: Can not allocate enough memory: ht_init\n");
    exit(1);    
  }
  
  ht->size = HT_CAP;
  ht->count = 0;  
  ht->items = (Ht_Item *) malloc(ht->size * sizeof(Ht_Item));
  if(!ht->items) {
    free(ht);
    fprintf(stderr, "ERROR: Can not allocate enough memory: ht_init\n");
    exit(1);    
  }

  for(int i=0;i<HT_CAP;i++) {
    ht->items[i] = (Ht_Item) {0};
  }

  return ht;
}

HASHTABLE_DEF void ht_insert(Ht *ht,
	       const char *key,
	       const void *value, size_t value_size) {
  ht_insert2(ht, key, strlen(key)+1, value, value_size);
}

HASHTABLE_DEF void ht_insert2(Ht *ht,
			      const char *key, size_t key_len,
			      const void *value, size_t value_size) {
  HT_CHECK_NOTNULL(ht);
  int index = (int) hash_function2(key, key_len);

  Ht_Item *item = &ht->items[index];
  
  if(item->size==0) {
    //INITIAL ALLOCATION
    ht_item_init(item);
  }
  else if(item->count==item->size) {
    //ALLOACTE MORE MEMORY
    ht_item_increment(item);
  }

  for(size_t i=0;i<item->count;i++) {
    if(item->entries[i].key_size == key_len &&
       memcmp(item->entries[i].key, key, key_len)==0) {
      ht_entry_free(&item->entries[i]);
      ht_entry_create(&item->entries[i],
		      key, (size_t) key_len,
		      value, value_size);
      return;
    }
  }

  //ADD ITEM
  ht_entry_create(&item->entries[item->count++],
		  key, (size_t) key_len,
		  value, value_size);
  ht->count++;  
}

HASHTABLE_DEF bool ht_remove(Ht *ht, const char* key) {
  HT_CHECK_NOTNULL(ht);
  size_t key_size;
  int index = (int) hash_function(key, &key_size);
  Ht_Item *item = &ht->items[index];

  if(item->size==0) return false;

  for(size_t i=0;i<item->count;i++) {
    if(item->entries[i].key_size == key_size && 
       memcmp(item->entries[i].key, key, key_size)==0) {
      ht_entry_free(&item->entries[i]);
      item->count--;
      ht->count--;
      return true;
    }
  }

  return false;
}

HASHTABLE_DEF bool ht_has(const Ht *ht, const char *key) {
  HT_CHECK_NOTNULL(ht);

  size_t key_size = strlen(key) + 1;  
  int index = (int) hash_function(key, NULL);
  Ht_Item *item = &ht->items[index];

  if(item->size==0) return false;

  for(size_t i=0;i<item->count;i++) {
    if(item->entries[i].key_size == key_size && 
       memcmp(item->entries[i].key, key, key_size)==0) {
      return true;
    }
  }

  return false;  
}

HASHTABLE_DEF void *ht_get(const Ht *ht, const char* key) {
  return ht_get2(ht, key, strlen(key)+1);
}

HASHTABLE_DEF void *ht_get2(const Ht *ht, const char* key, size_t key_len) {
  HT_CHECK_NOTNULL(ht);
  int index = (int) hash_function2(key, key_len);

  Ht_Item *item = &ht->items[index];

  if(item->size==0) return NULL;
  
  for(size_t i=0;i<item->count;i++) {
    if(item->entries[i].key_size == key_len &&
       memcmp(item->entries[i].key, key, key_len)==0) {
      return item->entries[i].value;
    }
  }

  return NULL;
}

HASHTABLE_DEF void ht_free(Ht *ht) {
  HT_CHECK_NOTNULL(ht);
  for(int i=0;i<HT_CAP;i++) if(ht->items[i].size>0) ht_item_free(&ht->items[i]);
  free(ht->items);
  free(ht);
}

#endif //HASHTABLE_IMPLEMENTATION

#endif //HASHTABLE_H_H
