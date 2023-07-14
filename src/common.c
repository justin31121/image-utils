#include <stdio.h>
#include <stdbool.h>

typedef unsigned char u8;
typedef char s8;
typedef unsigned short u16;
typedef short s16;
typedef unsigned int u32;
typedef int s32;
typedef unsigned long u64;
typedef long s64;

#define panic(...) do{ fprintf(stderr, "%s:%d: error: %s: ", __FILE__, __LINE__, __func__); fprintf(stderr, __VA_ARGS__); fprintf(stderr, "\n"); exit(1);}while(0)

bool slurp_file(const char *name, char **buffer, size_t *buffer_size) {  
  FILE *f = fopen(name, "rb");
  if(!f) {
    return false;
  }

  if(fseek(f, 0, SEEK_END) < 0) {
    fclose(f);
    return false;
  }

  long m = ftell(f);
  if(m < 0) {
    fclose(f);
    return false;
  }  

  if(fseek(f, 0, SEEK_SET) < 0) {
    fclose(f);
    return false;
  }

  *buffer = (char *) malloc((size_t) m + 1);
  if(!(*buffer)) {
    fclose(f);
    return false;
  }

  size_t _m = (size_t) m;
  size_t n = fread(*buffer, 1, _m, f);
  if(n != _m) {
    fclose(f);
    exit(1);    
  }
  (*buffer)[n] = 0;

  *buffer_size = n;

  fclose(f);
  return true;
}
