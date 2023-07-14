#include <stdio.h>
#include <string.h>

#define STB_IMAGE_IMPLEMENTATION
#include "../thirdparty/stb_image.h"

#include "./common.c"

#define STRING_IMPLEMENTATION
#include "./string.h"

typedef enum{
  OPERAND_RAW = 0,
  OPERAND_RGBA,
}Operand;

void operand_raw(const char *input, const char *output, string name) {
  size_t size;
  char *__data;
  if(!slurp_file(input, &__data, &size)) {
    exit(1);
  }
  u32 *data = (u32 *) __data;

  FILE *f = fopen(output, "wb");
  if(!f) {
    fprintf(stderr, "ERROR: Can not write to file '%s': %s", output, strerror(errno));
    exit(1);
  }
  fprintf(f, "#ifndef "String_Fmt"H_H_\n", String_Arg(name));
  fprintf(f, "#define "String_Fmt"H_H_\n\n", String_Arg(name));

  fprintf(f, "static int "String_Fmt"_size = %d;\n", String_Arg(name), (int) size);
  fprintf(f, "static char "String_Fmt"_data[] = {", String_Arg(name));
  char *_data = (char *) data;
  for(int i=0;i<size;i++) {
    fprintf(f, "%d", _data[i]);
    if(i != size - 1) {
      fprintf(f, ", ");
    }
  }
  fprintf(f, "};\n\n");

  fprintf(f, "#endif //"String_Fmt"H_H_", String_Arg(name));  
  fclose(f);
}

void operand_rgba(const char *input, const char *output, string name) {
  int width, height;
  u32 *data = (u32 *) stbi_load(input, &width, &height, 0, 4);

  if(!data) {
    fprintf(stderr, "ERROR: Can not read file: '%s'."
	    " Make sure it is a valid image!\n", input);
    exit(1);
  }

  FILE *f = fopen(output, "wb");
  if(!f) {
    fprintf(stderr, "ERROR: Can not write to file '%s': %s", output, strerror(errno));
    exit(1);
  }
  fprintf(f, "#ifndef "String_Fmt"H_H_\n", String_Arg(name));
  fprintf(f, "#define "String_Fmt"H_H_\n\n", String_Arg(name));

  fprintf(f, "static int "String_Fmt"_width = %d;\n", String_Arg(name), width);
  fprintf(f, "static int "String_Fmt"_height = %d;\n\n", String_Arg(name), height);
  fprintf(f, "static unsigned int "String_Fmt"_data[] = {", String_Arg(name));

  for(int i=0;i<width*height;i++) {
    fprintf(f, "0x%08x", data[i]);
    if(i != width*height - 1) {
      fprintf(f, ", ");
    }
  }
    
  fprintf(f, "};\n\n");

  fprintf(f, "#endif //"String_Fmt"H_H_", String_Arg(name));  
  fclose(f);
}


char *shift(int *argc, char ***argv) {
  if(*argc == 0) {
    return NULL;
  } else {
    (*argc)--;
    char *arg = *argv[0];
    (*argv)++;
    return arg;
  }
}

char *program;

void usage() {
  fprintf(stderr, "ERROR: Please provide enough arguments\n");
  fprintf(stderr, "USAGE: %s [-raw / -rgba / -font] [-s <size>] <input> <output>\n", program);
  exit(1);  
}

//bake nike.jpg logo.h
int main(int argc, char **argv) {
  
  Operand operand = 0;
  program = shift(&argc, &argv);
  
  char *next = shift(&argc, &argv);
  if(next == NULL) {
    usage();
  }

  string next_string = string_from_cstr(next);
  if(string_eq(next_string, STRING("-raw"))) {
    operand = OPERAND_RAW;
    next = shift(&argc, &argv);    
  } else if(string_eq(next_string, STRING("-rgba"))) {
    operand = OPERAND_RGBA;
    next = shift(&argc, &argv);
  }

  if(next == NULL) {
    usage();
  }
  int font_size;

  next_string = string_from_cstr(next);
  if(string_eq(next_string, STRING("-s"))) {
    next = shift(&argc, &argv);
    string digit_string = string_from_cstr(next);
    if(!string_chop_int(&digit_string, &font_size) || digit_string.len) {
      fprintf(stderr, "ERROR: Can not parse integer: '%s'\n", next);
      exit(1);
    }
    next = shift(&argc, &argv);
  }

  if(next == NULL) {
    usage();
  }
  const char *input = next;

  next = shift(&argc, &argv);
  if(next == NULL) {
    usage();
  }
  const char *output = next;

  string output_string = string_from_cstr(output);
  string part;
  while(output_string.len) {
    part = string_chop_by_delim(&output_string, '.');
  }
  string name = string_from_cstr(output);
  s32 name_len = part.data - output - 2;
  if(name_len < 0) {
    fprintf(stderr, "ERROR: '%s' must be a valid c-filename\n", output);
    exit(1);
  }
  
  string_chop_right(&name, name.len - name_len - 1);

  switch(operand) {
  case OPERAND_RAW: {
    operand_raw(input, output, name);
  } break;
  case OPERAND_RGBA: {
    operand_rgba(input, output, name);
  } break;
  default: {
    fprintf(stderr, "ERROR: Unimplemented Operand\n");
    exit(1);
  } break;    
  }
  
  printf("Saved %s\n", output);
  
  return 0;
}
