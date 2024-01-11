#ifndef CONVERT_H
#define CONVERT_H

#include "matrix_float.h"

typedef struct color_struct{
    unsigned char r;
    unsigned char g;
    unsigned char b;
}color;

typedef struct color_buffer_struct{
    unsigned char b;
    unsigned char g;
    unsigned char r;
    unsigned char w;
}color_buffer;

matrix_float_array convert_img_to_matrix(char* img);
void convert_matrix_to_buffer(matrix_float_array* img,color_buffer* buffer);

void convert_matrix_to_buffer(matrix_float_array* img,color_buffer* buffer){
    size_t size = img->matrix[0].height * img->matrix[0].width; 

    for(int i = 0; i < size; i++){
        //printf("%d\n",r);
        buffer[i].r = ((unsigned char)(img->matrix[0].pMatrix[i] * 255.0f))%256;
        buffer[i].g = ((unsigned char)(img->matrix[1].pMatrix[i] * 255.0f))%256;
        buffer[i].b = ((unsigned char)(img->matrix[2].pMatrix[i] * 255.0f))%256;
        buffer[i].w = 255;
    }
}

matrix_float_array* convert_matrix_to_random_prime(matrix_float_array* reference,matrix_float_array* noise){
    matrix_float_array_copy(reference,noise);

    int width = noise->matrix->width;
    int height = noise->matrix->height;

    int quit_width = rand()%(width/2) + (width/3);
    int quit_height = rand()%(height/2) + (height/3);

    int random_x = rand()%width;
    int random_y = rand()%height;

    for(int i = 0; i < quit_height; i++){
        for(int j = 0; j < quit_width; j++){
            int x = random_x + j;
            int y = random_y + i;

            if(x >= width || y >= height) continue;

            noise->matrix[0].pMatrix[y * width + x] = 0.5f;
            noise->matrix[1].pMatrix[y * width + x] = 0.5f;
            noise->matrix[2].pMatrix[y * width + x] = 0.5f;
        }
    }

    return noise;
}

matrix_float_array convert_img_to_matrix(char* img){
    matrix_float_array img_matrix;
    
    int width,height,n_channel;

    color* imagen = (color*)stbi_load(img,&width,&height,&n_channel,3);

    matrix_float_array_init(&img_matrix,height,width,3);

    for(unsigned int i = 0; i < 3; i++){
        for(int h = 0;h < height; h++){
            for(int w = 0; w < width; w++){
                unsigned char* real_color = (unsigned char*)&imagen[h * width + w] + i;
                img_matrix.matrix[i].pMatrix[h * img_matrix.matrix[i].width + w] = (float)((float)*real_color/255.0f);
            }
        }
    }

    stbi_image_free(imagen);

    return img_matrix;
}


#endif