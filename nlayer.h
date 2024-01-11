#ifndef NLAYER_HPP
#define NLAYER_HPP

#include "act_f.h"
#include "matrix_float.h"

typedef unsigned int number;
typedef unsigned int act_function;

typedef struct neural_layer_struct{
    number in_conn;
    number out_conn;
    act_f* act_func;

    matrix_float* W;
    matrix_float* B;
} neural_layer;

void matrix_random_value(matrix_float* n,float min,float max){
    LENGHT size = n->height * n->width;

    for(LENGHT i = 0; i < size; i++) 
        n->pMatrix[i] = (float)rand()/(float)RAND_MAX*(max-min) + min;
}

void neural_layer_init(neural_layer* n,number in, number out){
    n->W = (matrix_float*)malloc(sizeof(matrix_float));
    n->B = (matrix_float*)malloc(sizeof(matrix_float));

    matrix_float_init(n->W,in,out);
    matrix_float_init(n->B,1,out);
}

void neural_layer_free(neural_layer* n){
    matrix_float_free_get(&n->W);
    matrix_float_free_get(&n->B);

    n->in_conn = 0;
    n->out_conn = 0;
}

void neural_layer_random_values(neural_layer* n,float min, float max){
    matrix_random_value(n->W,min,max);
    matrix_random_value(n->B,min,max);
}

neural_layer** create_neural_layers(int* topology,LENGHT size,act_function* functions){
    neural_layer** nls = (neural_layer**)malloc((size - 1) * sizeof(neural_layer*));

    for(LENGHT i = 0; i < size - 1; i++){
        nls[i] = (neural_layer*)malloc(sizeof(neural_layer));
        neural_layer_init(nls[i],topology[i],topology[i+1]);
        nls[i]->act_func = parse_act_f(functions[i]);
    }

    return nls;
}

#endif