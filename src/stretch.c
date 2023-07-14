#include <stdio.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../thirdparty/stb_image_write.h"

#define STB_IMAGE_IMPLEMENTATION
#include "../thirdparty/stb_image.h"

#include "./common.c"

#define STRING_IMPLEMENTATION
#include "./string.h"

u32 rgba_mix(u32 color, u32 base) {

  u8 color_alpha = (0xff000000 & color) >> 24;

  u8 base_red = 0xff & base;
  u8 red = color_alpha * ((0xff & color) - base_red) / 0xff + base_red;
  
  u8 base_green = (u8) ((0xff00 & base) >> 8);
  u8 green = color_alpha * (((0xff00 & color) >> 8) - base_green) / 0xff + base_green;
  
  u8 base_blue = (u8) ((0xff0000 & base) >> 16);
  u16 blue = color_alpha * (((0xff0000 & color) >> 16) - base_blue) / 0xff + base_blue;
  
  return (color_alpha << 24) | (blue << 16) | (green << 8) | red;
}

void copy(u32* src, u32 width, u32 height,
	  u32* src2, u32 width2, u32 height2,
	  int x0, int y0, int w, int h) {
  for(int dy = 0; dy<h; ++dy) {
    for(int dx = 0; dx<w; ++dx) {
      int x = x0 + dx;
      int y = y0 + dy;
      int nx = dx*width2/w;
      int ny = dy*height2/h;


      if(x<0 || x>=(int) width || y<0 || y>=(int) height) {
	continue;
      }      
      src[y*width+x] = rgba_mix(src2[ny*width2+nx], src[y*width+x]);
    }
  }
}

//stretch play.png play2.png 64 64
int main(int argc, char **argv) {

  //ARGS
  if(argc < 5) {
    fprintf(stderr, "ERROR: Please provide enough arguments\n");
    fprintf(stderr, "USAGE: %s <input> <output> <output_width> <output_height>\n", argv[0]);
    exit(1);
  }

  int out_width, out_height;
  string width_string = string_from_cstr(argv[3]);
  if(!string_chop_int(&width_string, &out_width) || width_string.len) {
    fprintf(stderr, "ERROR: Can not parse integer: '%s'\n", argv[3]);
    exit(1);
  }
  string height_string = string_from_cstr(argv[4]);
  if(!string_chop_int(&height_string, &out_height) || height_string.len) {
    fprintf(stderr, "ERROR: Can not parse integer: '%s'\n", argv[4]);
    exit(1);
  }

  const char *input = argv[1];
  const char *output = argv[2];

  //LOAD INPUT
  int in_width, in_height;
  u32* data = (u32 *) stbi_load(input, &in_width, &in_height, 0, 4);
  if(!data) {
    fprintf(stderr, "ERROR: Can not read file: '%s'."
	    " Make sure it is a valid image!\n", input);
    exit(1);
  }

  u32* out_data = (u32 *) malloc(sizeof(u32) * out_width * out_height);
  if(!out_data) {
    fprintf(stderr, "ERROR: Can not allocate enough memory\n");
    exit(1);    
  }

  //DO
  copy(out_data, out_width, out_height, data, in_width, in_height, 0, 0, out_width, out_height);

  if(!stbi_write_png(output, out_width, out_height, 4, out_data, out_width * sizeof(u32))) {
    fprintf(stderr, "ERROR: Can not save output to '%s'\n", output);
    exit(1);
  }

  //DO OUTPUT
  printf("Saved %s (%dx%d)\n", output, out_width, out_height);
  //stbi_image_free(images[i]->data);
  //free(out_data);
  
  return 0;
}
