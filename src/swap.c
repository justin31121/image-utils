#include <stdio.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../thirdparty/stb_image_write.h"

#define STB_IMAGE_IMPLEMENTATION
#include "../thirdparty/stb_image.h"

#include "./common.c"

#define STRING_IMPLEMENTATION
#include "./string.h"

//swap tads.png tads_out.png ff0000 00ff00
int main(int argc, char **argv) {

  //ARGS
  if(argc != 5) {
    fprintf(stderr, "ERROR: Please enough arguments\n");
    fprintf(stderr, "USAGE: %s <image_in> <image_out> <color_in> <color_out>\n", argv[0]);
    exit(1);
  }

  const char *input = argv[1];
  const char *output = argv[2];
  string color_in_string = string_from_cstr(argv[3]);
  string color_out_string = string_from_cstr(argv[4]);
  u32 color_in, color_out;

  string_chop_string(&color_in_string, STRING("0x"));
  uint64_t hex_value;
  if(!string_chop_hex(&color_in_string, &hex_value) ||
     color_in_string.len) {
    fprintf(stderr, "ERROR: Can not parse <color_in>. Expected u32 '%s'\n", argv[3]);
  }
  color_in = (u32) hex_value;
  color_in |= 0xff000000;

  string_chop_string(&color_out_string, STRING("0x"));
  if(!string_chop_hex(&color_out_string, &hex_value) ||
     color_out_string.len) {
    fprintf(stderr, "ERROR: Can not parse <color_out>. Expected u32 '%s'\n", argv[4]);
  }
  color_out = (u32) hex_value;
  
  //LOAD INPUT
  int width, height;
  u32 *data = (u32 *) stbi_load(input, &width, &height, 0, 4);

  if(!data) {
    fprintf(stderr, "ERROR: Can not read file: '%s'."
	    " Make sure it is a valid image!\n", input);
    exit(1);
  }

  //DO
  int len = width * height;
  for(int i=0;i<len;i++) {
    u32 pixel = data[i];
    if(pixel == color_in) {
      data[i] = color_out;
    }
  }

  if(!stbi_write_png(output, width, height, 4, data, width * sizeof(u32))) {
    fprintf(stderr, "ERROR: Can not save file: '%s'.", output);
    exit(1);
  }

  //DO OUTPUT  
  printf("Saved %s\n", output);
  
  return 0;
}
