#include <stdio.h>

#define STB_IMAGE_IMPLEMENTATION
#include "../thirdparty/stb_image.h"

#define HASHTABLE_IMPLEMENTATION
#include "./hashtable.h"

#include "./common.c"

typedef struct{
  u32 color;
  int count;
}Foo;

int foo_compare(const void *a, const void *b) {
  return (*(Foo *) b).count - (*(Foo *) a).count;
}

int main(int argc, char **argv) {

  //ARGS  
  if(argc != 2) {
    fprintf(stderr, "ERROR: Please provide a input\n");
    fprintf(stderr, "USAGE: %s <image>\n", argv[0]);
    exit(1);
  }

  const char *input = argv[1];
  
  //LOAD INPUT
  int width, height;
  u32 *data = (u32 *) stbi_load(input, &width, &height, 0, 4);

  if(!data) {
    fprintf(stderr, "ERROR: Can not read file: '%s'."
	    " Make sure it is a valid image!\n", input);
    exit(1);
  }

  int len = width * height;
  
  //PARSE INPUT
  Ht *ht = ht_init();
  int one = 1;

  for(int i=0;i<len;i++) {
    u32 pixel = data[i];
			  
    void *value = ht_get2(ht, (char *) &pixel, sizeof(u32));
    if(!value) {
      ht_insert2(ht, (char *) &pixel, sizeof(u32), &one, sizeof(int));
    } else {
      int *count = (int *) value;
      *count = (*count) + 1;
    }
  }
 
  //DO OUPTUT
  Foo *foos = (Foo *) malloc(sizeof(Foo) * ht->count);
  if(!foos) {
    fprintf(stderr, "ERROR: Can not allocage enough memory to proceed\n");
    exit(1);
  }

  int last = -1;
  Ht_Entry *entry = NULL;
  int k = 0;
  while(ht_next(ht, &last, &entry)) {
    foos[k].color = *(u32 *)entry->key;
    foos[k++].count = *(int *)entry->value;
  }

  qsort(foos, ht->count, sizeof(Foo), foo_compare);

  printf("Image contains this colors (%d):\n", len);
  for(int i=0;i<20 && i<ht->count;i++) {
    int count = foos[i].count;
    float p = (float) count * 100.0f / (float) len;
    printf("\t0x%08x -> %3.2f (%d)\n", foos[i].color, p, count);
  }
  
  return 0;
}
