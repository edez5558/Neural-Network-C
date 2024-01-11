#ifndef CONV_LAYER_HPP
#define CONV_LAYER_HPP

#define KERNEL_VALID 0x01
#define KERNEL_FULL  0x02

#include "nlayer.h"
#include "optimizer_struct.h"

typedef unsigned char type_cr; 

typedef struct dimension_struct{
    number height;
    number width;
    number depth;
} dimension;

typedef struct convolutional_layer_struct{
    dimension in_shape;
    dimension out_shape;

    number  depth;
    number  kernel_size;

    type_cr kernel_type;

    matrix_float_array kernel;
    matrix_float_array B;
} convolutional_layer;



void convolutional_layer_init(convolutional_layer* n,dimension in,number kernel_size,number kernel_depth,type_cr kernel_type){
    //Kernel depth es igual a Input depth
    //Numero de bias es igual a depth
    //El tamanio de bias es igual al output dimension
    //Output depth es igual a kernel size
    n->in_shape = in;
    n->depth = kernel_depth;
    n->kernel_size = kernel_size;

    n->kernel_type = kernel_type;

    if(kernel_type == KERNEL_VALID){
        n->out_shape.height = in.height - kernel_size + 1;
        n->out_shape.width  = in.width  - kernel_size + 1;
        n->out_shape.depth  = kernel_depth; 
    }else{
        //CORRELATION_KERNEL_FULL
        n->out_shape.height = in.height + kernel_size - 1;
        n->out_shape.width  = in.width  + kernel_size - 1;
        n->out_shape.depth  = kernel_depth;
    }

    number size = in.depth * kernel_depth;

    matrix_float_array_init(&n->kernel,kernel_size,kernel_size,size);
    matrix_float_array_init(&n->B,n->out_shape.height,n->out_shape.width,kernel_depth);
}

void convolutional_layer_free(convolutional_layer* n){
    matrix_float_array_free(&n->kernel);
    matrix_float_array_free(&n->B);

    n->in_shape.depth = 0;
    n->in_shape.height = 0;
    n->in_shape.width = 0;
    n->out_shape = n->in_shape;

    n->depth = 0;
    n->kernel_size = 0;
}

void convolutional_layer_random_values(convolutional_layer* n,float min, float max){
    LENGHT size = n->kernel.depth;

    for(LENGHT i = 0; i < size; i++)
        matrix_random_value(&n->kernel.matrix[i],min,max);

    size = n->B.depth;

    for(LENGHT i = 0; i < size; i++)
        matrix_random_value(&n->B.matrix[i],min,max);    

}

void matrix_correlation_valid(matrix_float* input,matrix_float* kernel,matrix_float* output){
    for(LENGHT h_out = 0; h_out < output->height; h_out++){
        for(LENGHT w_out = 0; w_out < output->width; w_out++){
            float* celda = &(output->pMatrix[h_out * output->width + w_out]);

            for(LENGHT h_kernel = 0; h_kernel < kernel->height; h_kernel++){
                for(LENGHT w_kernel = 0; w_kernel < kernel->width; w_kernel++){
                    *celda += input->pMatrix[(h_out + h_kernel) * input->width + w_out + w_kernel] *
                              kernel->pMatrix[h_kernel * kernel->width + w_kernel];
                }
            }

        }
    }
}

void matrix_convolution_valid(matrix_float* input,matrix_float* kernel,matrix_float* output){
    for(LENGHT h_out = 0; h_out < output->height; h_out++){
        for(LENGHT w_out = 0; w_out < output->width; w_out++){
            float* celda = &(output->pMatrix[h_out * output->width + w_out]);

            for(LENGHT h_kernel = 0; h_kernel < kernel->height; h_kernel++){
                for(LENGHT w_kernel = 0; w_kernel < kernel->width; w_kernel++){
                    *celda += input->pMatrix[(h_out + h_kernel) * input->width + w_out + w_kernel] *
                              kernel->pMatrix[(kernel->height - h_kernel - 1) * kernel->width + (kernel->width - w_kernel - 1)];
                }
            }

        }
    }
}

void matrix_convolution_full(matrix_float* input, matrix_float* kernel, matrix_float* output){
    LENGHT c_h = kernel->height - 1;
    LENGHT c_w = kernel->width  - 1;
    
    LENGHT h_input;
    LENGHT w_input;

    for(LENGHT h_out = 0; h_out < output->height; h_out++){
        for(LENGHT w_out = 0; w_out < output->width; w_out++){
            float* celda = &output->pMatrix[h_out * output->width + w_out];

            for(LENGHT h_kernel = 0; h_kernel < kernel->height; h_kernel++){
                for(LENGHT w_kernel = 0; w_kernel < kernel->width; w_kernel++){
                    h_input = h_out + h_kernel - c_h;
                    w_input = w_out + w_kernel - c_w;

                    if(h_input < 0 || w_input < 0 || h_input >= input->height || w_input >= input->width)
                        continue;

                    *celda += input->pMatrix[h_input * input->width + w_input] * kernel->pMatrix[(kernel->height - h_kernel - 1) * kernel->width + (kernel->width - w_kernel - 1)];
                }
            }

        }
    }
}

void matrix_correlation_full(matrix_float* input, matrix_float* kernel, matrix_float* output){
    LENGHT c_h = kernel->height - 1;
    LENGHT c_w = kernel->width  - 1;
    
    LENGHT h_input;
    LENGHT w_input;

    for(LENGHT h_out = 0; h_out < output->height; h_out++){
        for(LENGHT w_out = 0; w_out < output->width; w_out++){
            float* celda = &output->pMatrix[h_out * output->width + w_out];

            for(LENGHT h_kernel = 0; h_kernel < kernel->height; h_kernel++){
                for(LENGHT w_kernel = 0; w_kernel < kernel->width; w_kernel++){
                    h_input = h_out + h_kernel - c_h;
                    w_input = w_out + w_kernel - c_w;

                    if(h_input < 0 || w_input < 0 || h_input >= input->height || w_input >= input->width)
                        continue;

                    *celda += input->pMatrix[h_input * input->width + w_input] * kernel->pMatrix[h_kernel * kernel->width + w_kernel];
                }
            }

        }
    }
}

void matrix_rotation_180(matrix_float* matrix){
    LENGHT longitud = (matrix->height * matrix->width)/2;

    LENGHT h_matrix = -1;
    LENGHT w_matrix = 0;

    for(LENGHT i = 0;i < longitud; i++){
        if((w_matrix = (i%matrix->width)) == 0) h_matrix++;

        float* aux1 = &(matrix->pMatrix[h_matrix * matrix->width + w_matrix]);
        float* aux2 = &(matrix->pMatrix[(matrix->height - h_matrix - 1) * matrix->width + (matrix->width - w_matrix - 1)]);
        
        float aux = *aux1;
        
        *aux1 = *aux2;
        *aux2 = aux;
    }
}

#endif