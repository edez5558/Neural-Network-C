#ifndef OPTIMIZER_STRUCT_HPP
#define OPTIMIZER_STRUCT_HPP

#include "matrix_float.h"

typedef char type_op;
typedef char type_model;
typedef char type_parameter;

#define PARAMETER_B 0x01
#define PARAMETER_W 0x02
#define PARAMETER_K 0x02

#define OPTIMIZER_GRADIENT_DESCENT 0x01
#define OPTIMIZER_ADAM             0x02

#define MODEL_NEURAL        0x01
#define MODEL_CONVOLUTIONAL 0x02

struct optimizer_manager_struct;

typedef void (*optimizer_func)(struct optimizer_manager_struct*,matrix_float*,matrix_float*,number,number,float,type_parameter);

typedef struct optimizer_manager_struct{
    matrix_float** Vdw;
    matrix_float** Vdb;
    matrix_float** Sdw;
    matrix_float** Sdb;
    float B1;
    float B2;
    optimizer_func f_optimizer;
}optimizer_manager;



#endif