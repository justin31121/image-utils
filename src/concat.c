#include <stdio.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../thirdparty/stb_image_write.h"

#define STB_IMAGE_IMPLEMENTATION
#include "../thirdparty/stb_image.h"

#include "./common.c"

typedef struct Image Image;

struct Image {
  int width, height;
  u32 *data;
};

//concat play.png pause.png out.png
int main (int argc, char **argv) {

  if(argc < 4) {
    fprintf(stderr, "ERROR: Please provide atleast two inputs and an output\n");
    fprintf(stderr, "USAGE: %s <input_1> <input_2> ... <output>\n", argv[0]);
    exit(1);
  }
  const char *output = argv[argc-1];

  //Load Input
  Image *images = malloc(sizeof(Image) * (argc - 2));
  if(!images) {
    fprintf(stderr, "ERROR: Can not allocate enough memory\n");
    exit(1);
  }

  u32 max_height = 0;
  u32 width = 0;
  
  for(int i=0;i<argc-2;i++) {
    const char *input = argv[i+1];
    Image *image = &images[i];
    image->data = (u32 *) stbi_load(input, &image->width, &image->height, 0, 4);
    if(!image->data) {
      fprintf(stderr, "ERROR: Can not read file: '%s'."
	      " Make sure it is a valid image!\n", input);
      exit(1);
    }

    if(image->height > max_height) {
      max_height = image->height;
    }
    width += image->width;
  }

  u32 *out_data = (u32 *) malloc(sizeof(u32) * max_height * width);
  if(!out_data) {
    fprintf(stderr, "ERROR: Can not allocate enough memory\n");
    exit(1);    
  }
  memset(out_data, 0, sizeof(u32) * max_height * width);

  u32 x_pos = 0;
  for(int i=0;i<argc-2;i++) {
    printf("writting: %s\n", argv[i+1]);
    Image *image = &images[i];

    for(int y=0;y<image->height;y++) {
      for(int x=0;x<image->width;x++) {
	u32 pixel = image->data[(image->height - y - 1)*image->width+x];
	out_data[(max_height - y - 1)*width+(x_pos+x)] = pixel;
      }
    }

    x_pos += image->width;
  }

  if(!stbi_write_png(output, width, max_height, 4, out_data, width * sizeof(u32))) {
    fprintf(stderr, "ERROR: Can not save output to '%s'\n", output);
    exit(1);
  }

  //DO OUTPUT  
  printf("Saved %s (%dx%d)\n", output, width, max_height);
  //for(int i=0;i<argc-2;i++) stbi_image_free(images[i]->data);
  //free(images);
  //free(out_data);

  return 0;
}
