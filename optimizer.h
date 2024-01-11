#ifndef OPTIMIZER_HPP
#define OPTIMIZER_HPP

#include "optimizer_struct.h"
#include "learning.h"
#include "conv_model.h"
#include "matrix_float.h"
#include <math.h>

void optimizer_neural_adam_free(optimizer_manager* op,neural_model* model){
    for(int i = 0; i < model->size; i++){
        matrix_float_free_get(&op->Vdw[i]);
        matrix_float_free_get(&op->Vdb[i]);
        matrix_float_free_get(&op->Sdw[i]);
        matrix_float_free_get(&op->Sdb[i]);
    }

    free(op->Vdw);
    free(op->Vdb);
    free(op->Sdw);
    free(op->Sdb);

    op->Vdw = NULL;
    op->Vdb = NULL;
    op->Sdw = NULL;
    op->Sdb = NULL;
}

void optimizer_conv_adam_free(optimizer_manager* op,convolutional_model* model){
    for(int i = 0; i < model->n_layers; i++){
        number size = model->layers[i].in_shape.depth * model->layers[i].depth;

        for(number s = 0; s < size; s++){
            matrix_float_free(&(op->Vdw[i][s]));
            matrix_float_free(&(op->Sdw[i][s]));
        }

        size = model->layers[i].depth;

        for(number s = 0; s < size; s++){
            matrix_float_free(&(op->Vdb[i][s]));
            matrix_float_free(&(op->Sdb[i][s]));
        }

        free(op->Vdw[i]);
        free(op->Vdb[i]);
        free(op->Sdw[i]);
        free(op->Sdb[i]);
    }

    free(op->Vdw);
    free(op->Vdb);
    free(op->Sdw);
    free(op->Sdb);

    op->Vdw = NULL;
    op->Vdb = NULL;
    op->Sdw = NULL;
    op->Sdb = NULL;
    op->f_optimizer = NULL;
}

void optimizer_neural_adam_init(optimizer_manager* op,neural_model* model){
    op->Vdw = (matrix_float**)malloc(model->size * sizeof(matrix_float*));
    op->Vdb = (matrix_float**)malloc(model->size * sizeof(matrix_float*));
    op->Sdw = (matrix_float**)malloc(model->size * sizeof(matrix_float*));
    op->Sdb = (matrix_float**)malloc(model->size * sizeof(matrix_float*));

    for(int i = 0; i < model->size; i++){
        op->Vdw[i] = (matrix_float*)malloc(sizeof(matrix_float));
        op->Vdb[i] = (matrix_float*)malloc(sizeof(matrix_float));
        op->Sdw[i] = (matrix_float*)malloc(sizeof(matrix_float));
        op->Sdb[i] = (matrix_float*)malloc(sizeof(matrix_float));

        matrix_float_init(op->Vdw[i],model->topology[i],model->topology[i+1]);
        matrix_float_init(op->Vdb[i],1,model->topology[i+1]);

        matrix_float_init(op->Sdw[i],model->topology[i],model->topology[i+1]);
        matrix_float_init(op->Sdb[i],1,model->topology[i+1]);

        matrix_float_insert(op->Vdw[i],0.0f);
        matrix_float_insert(op->Vdb[i],0.0f);
        matrix_float_insert(op->Sdw[i],0.0f);
        matrix_float_insert(op->Sdb[i],0.0f);
    }
}

void optimizer_conv_adam_init(optimizer_manager* op,convolutional_model* model){
    op->Vdw = (matrix_float**)malloc(model->n_layers * sizeof(matrix_float*));
    op->Vdb = (matrix_float**)malloc(model->n_layers * sizeof(matrix_float*));
    op->Sdw = (matrix_float**)malloc(model->n_layers * sizeof(matrix_float*));
    op->Sdb = (matrix_float**)malloc(model->n_layers * sizeof(matrix_float*));

    for(int i = 0; i < model->n_layers; i++){
        number size = model->layers[i].in_shape.depth * model->layers[i].depth;

        op->Vdw[i] = (matrix_float*)malloc(size * sizeof(matrix_float));
        op->Vdb[i] = (matrix_float*)malloc(model->layers[i].depth * sizeof(matrix_float));
        op->Sdw[i] = (matrix_float*)malloc(size * sizeof(matrix_float));
        op->Sdb[i] = (matrix_float*)malloc(model->layers[i].depth * sizeof(matrix_float));

        for(number s = 0; s < size; s++){
            matrix_float_init(&op->Vdw[i][s],model->layers[i].kernel_size,model->layers[i].kernel_size);
            matrix_float_init(&op->Sdw[i][s],model->layers[i].kernel_size,model->layers[i].kernel_size);
            matrix_float_insert(&op->Vdw[i][s],0.0f);
            matrix_float_insert(&op->Sdw[i][s],0.0f);
        }

        size = model->layers[i].depth;

        for(number s = 0; s < size; s++){
            matrix_float_init(&op->Vdb[i][s],model->layers[i].out_shape.height,model->layers[i].out_shape.width);
            matrix_float_init(&op->Sdb[i][s],model->layers[i].out_shape.height,model->layers[i].out_shape.width);
            matrix_float_insert(&op->Vdb[i][s],0.0f);
            matrix_float_insert(&op->Sdb[i][s],0.0f);
        }
    }
}

void optimizer_gradient_update(optimizer_manager* op,matrix_float* m,matrix_float* deltas,number capa,number external,float lr,type_parameter t_p){
    matrix_float_rest(m,matrix_float_mult_float(deltas,&lr));
}

void optimizer_adam_update(optimizer_manager* op,matrix_float* m,matrix_float* deltas,number capa,number external,float lr,type_parameter t_p){
    float aux1 = (1 - op->B1);
    float aux2 = (1 - op->B2);

    matrix_float* vd = (t_p == PARAMETER_K? &op->Vdw[capa][external] : &op->Vdb[capa][external]);
    matrix_float* sd = (t_p == PARAMETER_K? &op->Sdw[capa][external] : &op->Sdb[capa][external]);


    matrix_float* deltacpy = matrix_float_get_copy(deltas);
    matrix_float_mult(deltacpy,deltas);

    vd = matrix_float_sum(matrix_float_mult_float(vd,&op->B1),matrix_float_mult_float(deltas,&aux1));
    sd = matrix_float_sum(matrix_float_mult_float(sd,&op->B2),matrix_float_mult_float(deltacpy,&aux2));


    matrix_float_free_get(&deltacpy);

    matrix_float* vdc = matrix_float_get_copy(vd);
    matrix_float* sdc = matrix_float_get_copy(sd);

    float aux = 0.000000001f;
    matrix_float_sum_float(sdc,&aux);
    matrix_float_raiz_apply(sdc);
    
    matrix_float_div(vdc,sdc);
    
    matrix_float_free_get(&sdc);
    matrix_float_mult_float(vdc,&lr);
    
    matrix_float_rest(m,vdc);

    matrix_float_free_get(&vdc);
}

void optimizer_init(optimizer_manager* op,void* model,type_op t_op,type_model t_m){
    op->Vdw = NULL;
    op->Vdb = NULL;
    op->Sdw = NULL;
    op->Sdb = NULL;
    op->B1 = 0.9f;
    op->B2 = 0.999f;

    switch(t_op){
        case OPTIMIZER_ADAM: op->f_optimizer = &optimizer_adam_update;
            break;
        case OPTIMIZER_GRADIENT_DESCENT: op->f_optimizer = &optimizer_gradient_update;
            break;
    }

    if(t_op == OPTIMIZER_ADAM){
        switch(t_m){
            case MODEL_CONVOLUTIONAL: optimizer_conv_adam_init(op,(convolutional_model*)model);
                return;
            case MODEL_NEURAL: optimizer_neural_adam_init(op,(neural_model*)model);
                return;
        }
    }
}

void optimizer_free(optimizer_manager* op,void* model,type_op t_op,type_model t_m){
    if(t_op == OPTIMIZER_ADAM){
        switch(t_m){
            case MODEL_CONVOLUTIONAL: optimizer_conv_adam_free(op,(convolutional_model*)model);
                return;
            case MODEL_NEURAL : optimizer_neural_adam_free(op,(neural_model*)model);
                return;
        }
    }
}



#endif