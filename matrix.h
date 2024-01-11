#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef unsigned int LENGHT;

#define DEFINE_MATRIX_ARRAY(TYPE)\
    typedef struct TYPE##_array_struct {TYPE* matrix;\
                                       LENGHT depth;} TYPE##_array;\
                                       \
    void TYPE##_array_init(TYPE##_array* a,LENGHT height,LENGHT width,LENGHT depth){\
        a->matrix = (TYPE*)malloc(depth * sizeof(TYPE));\
        a->depth = depth;\
        for(LENGHT i = 0; i < depth; i++)\
            TYPE##_init(&a->matrix[i],height,width);\
    }\
    \
    void TYPE##_array_free(TYPE##_array* a){\
        for(LENGHT i = 0; i < a->depth; i++)\
            TYPE##_free(&a->matrix[i]);\
        free(a->matrix);\
        a->matrix = NULL;\
        a->depth = 0;\
    }\

#define DEFINE_MATRIX_ARRAY_FUNCTION_A(TYPE,FUNCTION,NAME)\
    TYPE* TYPE##_##NAME##_a(TYPE* a){\
        for(LENGHT i = 0; i < a->depth; i++){\
            FUNCTION(&a->matrix[i]);\
        }\
        return a;\
    }\
\

#define DEFINE_MATRIX_ARRAY_FUNCTION_AB(TYPE,FUNCTION,NAME)\
    TYPE* TYPE##_##NAME##_ab(TYPE* a,TYPE* b){\
        for(LENGHT i = 0; i < a->depth; i++){\
            FUNCTION(&a->matrix[i],&b->matrix[i]);\
        }\
        return a;\
    }\
\


#define DEFINE_MATRIX_OPERATOR(TYPE,OPERATOR,NAME)\
    TYPE* TYPE##_##NAME(TYPE* a, TYPE* b){\
        if(a->width == b->width){\
            LENGHT size = a->height * a->width;\
            if(a->height == b ->height){\
                for(LENGHT i = 0; i < size; i++)\
                    a->pMatrix[i] OPERATOR##= b->pMatrix[i];\
            }else{\
                for(LENGHT i = 0; i < size; i++)\
                    a->pMatrix[i] OPERATOR##= b->pMatrix[i%a->width];\
            }\
            return a;\
        }\
        return NULL;\
    }\

#define DEFINE_MATRIX_OPERATOR_TRI(TYPE,OPERATOR,NAME)\
    TYPE* TYPE##_##NAME##_tri(TYPE* a, TYPE* b,TYPE* result){\
        if(a->width == b->width && result->width == a->width){\
            LENGHT size = result->height * result->width;\
            if(a->height == b->height && result->height == a->height){\
                for(LENGHT i = 0; i < size; i++)\
                    result->pMatrix[i] = a->pMatrix[i] OPERATOR b->pMatrix[i];\
            }else{\
                for(LENGHT i = 0; i < size; i++)\
                    result->pMatrix[i] = a->pMatrix[i%a->width] OPERATOR b->pMatrix[i%a->width];\
            }\
            return result;\
        }\
        return NULL;\
    }\

#define DEFINE_MATRIX_FUNCTION(TYPE,FUNCTION,NAME)\
    TYPE* TYPE##_##NAME##_apply(TYPE* a){\
        LENGHT size = a->height * a->width;\
        for(LENGHT i = 0; i < size; i++)\
            a->pMatrix[i] = FUNCTION(a->pMatrix[i]);\
        return a;\
    }\

#define DEFINE_MATRIX_OPERATOR_ELEMENT(TYPE,ELEMENT,OPERATOR,NAME)\
    TYPE* TYPE##_##NAME##_##ELEMENT(TYPE* a, ELEMENT* b){\
        LENGHT size = a->height * a->width;\
        for(LENGHT i = 0; i < size; i++)\
            a->pMatrix[i] = a->pMatrix[i] OPERATOR *b;\
        return a;\
    }\

#define DEFINE_MATRIX(TYPE) \
    typedef struct matrix_##TYPE##_struct { TYPE* pMatrix;\
                                            LENGHT height;\
                                            LENGHT width;} matrix_##TYPE;\
    \
    void matrix_##TYPE##_init(matrix_##TYPE* a,LENGHT height, LENGHT width){\
        a->pMatrix = (TYPE*)malloc(height * width * sizeof(*a->pMatrix));\
        a->height = (LENGHT)height;\
        a->width = (LENGHT)width;\
    }\
    matrix_##TYPE* matrix_##TYPE##_get_init(LENGHT height, LENGHT width){\
        matrix_##TYPE* a = (matrix_##TYPE*)malloc(sizeof(*a));\
        matrix_##TYPE##_init(a,height,width);\
        return a;\
    }\
    \
    void matrix_##TYPE##_insert(matrix_##TYPE* a,TYPE value){\
        LENGHT size = a->height * a->width;\
        for(LENGHT i = 0; i < size; i++)\
            a->pMatrix[i] = value;\
    }\
    \
    void matrix_##TYPE##_free(matrix_##TYPE* a){\
        free(a->pMatrix);\
        a->pMatrix = NULL;\
        a->width = 0; a->height = 0;\
    }\
    \
    void matrix_##TYPE##_free_get(matrix_##TYPE** a){\
        matrix_##TYPE##_free(*a);\
        free(*a);\
        *a = NULL;\
    }\
    \
    void matrix_##TYPE##_set(matrix_##TYPE* a,void* b){\
        memcpy(a->pMatrix,b,sizeof(TYPE) * a->height * a->height);\
    }\
    \
    matrix_##TYPE* matrix_##TYPE##_copy(matrix_##TYPE* a, matrix_##TYPE* b){\
        matrix_##TYPE##_free(a);\
        matrix_##TYPE##_init(a,b->height,b->width);\
        memcpy(a->pMatrix,b->pMatrix,sizeof(TYPE) * a->height * a->width);\
        return a;\
    }\
    \
    matrix_##TYPE* matrix_##TYPE##_get_copy(matrix_##TYPE* a){\
        matrix_##TYPE* copy = (matrix_##TYPE*)malloc(sizeof(matrix_##TYPE));\
        matrix_##TYPE##_init(copy,a->height,a->width);\
        memcpy(copy->pMatrix,a->pMatrix,sizeof(TYPE) * a->height * a->width);\
        return copy;\
    }\
    \
    matrix_##TYPE* matrix_##TYPE##_get_transpose(matrix_##TYPE* a){\
        matrix_##TYPE* result = (matrix_##TYPE*)malloc(sizeof(matrix_##TYPE));\
        matrix_##TYPE##_init(result,a->width,a->height);\
        for(LENGHT i = 0; i < a->height; i++){\
            for(LENGHT j = 0; j < a->width; j++)\
                result->pMatrix[j * result->width + i] = a->pMatrix[i * a->width + j];\
        }\
        return result;\
    }\
    \
    matrix_##TYPE* matrix_##TYPE##_get_mmult(matrix_##TYPE* a, matrix_##TYPE* b){\
        if(a->width == b->height){\
            matrix_##TYPE* result = (matrix_##TYPE*)malloc(sizeof(matrix_##TYPE));\
            matrix_##TYPE##_init(result,a->height,b->width);\
            TYPE sum;\
            for(LENGHT i = 0; i < result->height; i++){\
                for(LENGHT j = 0; j < result->width; j++){\
                    sum = 0.0f;\
                    for(LENGHT k = 0; k < a->width; k++){\
                        sum += a->pMatrix[i * a->width + k] * b->pMatrix[k * b->width + j];\
                    }\
                    result->pMatrix[i * result->width + j] = sum;\
                }\
            }\
            return result;\
        }\
        return NULL;\
    }\
    \
    void matrix_##TYPE##_shape(matrix_##TYPE* a){\
        printf("(%u,%u)\n",a->height,a->width);\
    }\
    void matrix_##TYPE##_print(matrix_##TYPE* a){\
        for(LENGHT i = 0; i < a->height; i++){\
            for(LENGHT j = 0; j < a->width; j++){\
                printf("%.2f ",a->pMatrix[i * a->width + j]);\
            }\
            putchar('\n');\
        }\
    }\
    \

#define CREATE_MATRIX(TYPE, NAME, HEIGHT, WIDTH)\
        matrix_##TYPE NAME;\
        matrix_##TYPE##_init(&NAME,HEIGHT,WIDTH);

#define DEFINE_MATRIX_GET_OPERATOR(TYPE,OPERATOR,NAME)\
    TYPE* TYPE##_get_##NAME(TYPE* a, TYPE* b){\
        if(a->width == b->width){\
            TYPE* result = (TYPE*)malloc(sizeof(TYPE));\
            TYPE##_init(result,a->height,a->width);\
            LENGHT size = a->height * a->width;\
            if(a->height == b ->height){\
                for(LENGHT i = 0; i < size; i++)\
                    result->pMatrix[i] = a->pMatrix[i] OPERATOR b->pMatrix[i];\
            }else{\
                for(LENGHT i = 0; i < size; i++)\
                    result->pMatrix[i] = a->pMatrix[i] OPERATOR b->pMatrix[i%result->width];\
            }\
            return result;\
        }\
        return NULL;\
    }\
