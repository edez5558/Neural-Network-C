#ifndef MATRIX_FLOAT_H
#define MATRIX_FLOAT_H

#include <math.h>
#include "matrix.h"
#include "act_f.h"

DEFINE_MATRIX(float)

DEFINE_MATRIX_FUNCTION(matrix_float,sqrt,raiz)
DEFINE_MATRIX_FUNCTION(matrix_float,sigmFunction,sigm)
DEFINE_MATRIX_FUNCTION(matrix_float,sigmDerivada,sigm_prime)
DEFINE_MATRIX_OPERATOR(matrix_float,/,div)
DEFINE_MATRIX_OPERATOR_ELEMENT(matrix_float,float,+,sum)

DEFINE_MATRIX_OPERATOR(matrix_float,+,sum)
DEFINE_MATRIX_OPERATOR(matrix_float,-,rest)
DEFINE_MATRIX_OPERATOR(matrix_float,*,mult)
DEFINE_MATRIX_GET_OPERATOR(matrix_float,-,rest)

DEFINE_MATRIX_OPERATOR_TRI(matrix_float,-,rest)

DEFINE_MATRIX_OPERATOR_ELEMENT(matrix_float,float,*,mult)

DEFINE_MATRIX_ARRAY(matrix_float)

DEFINE_MATRIX_ARRAY_FUNCTION_AB(matrix_float_array,matrix_float_mult,mult);
DEFINE_MATRIX_ARRAY_FUNCTION_A(matrix_float_array,matrix_float_sigm_prime_apply,sigm_prime);

void matrix_float_array_copy(matrix_float_array* source,matrix_float_array* dest){
    size_t size = source->matrix[0].height * source->matrix[0].width * sizeof(float);
    for(LENGHT i = 0; i < source->depth; i++){
        memcpy(dest->matrix[i].pMatrix,source->matrix[i].pMatrix,size);
    }
}

#endif