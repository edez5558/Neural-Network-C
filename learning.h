#ifndef LEARNING_H
#define LEARNING_H

#include "model.h"
#include "optimizer_struct.h"
#include "matrix_float.h"

matrix_float* matrix_get_mean_horizontal(matrix_float* a);
float l2cost(matrix_float* Yp,matrix_float* Yr);
matrix_float* matrix_get_l2cost_derivada(matrix_float* Yp,matrix_float* Yr);
matrix_float* matrix_apply_activation(matrix_float* matrix,float (*f)(float));
float model_execute(neural_model* model,matrix_float* X, matrix_float* Y,char train,optimizer_manager* op,float lr);

matrix_float* matrix_apply_activation(matrix_float* matrix,float (*f)(float)){
    LENGHT size = matrix->height * matrix->width;
    for(int i = 0; i < size; i++)
        matrix->pMatrix[i] = f(matrix->pMatrix[i]);

    return matrix;
}


float model_execute(neural_model* model,matrix_float* X, matrix_float* Y,char train,optimizer_manager* op, float lr){
    matrix_float** out = (matrix_float**)malloc((model->size + 1) * 2 * sizeof(matrix_float*));
    float cost = 0.0f;

    out[0] = X;
    out[1] = X;

    for(int i = 0; i < model->size; i++){
        out[(i+1)*2] = matrix_float_sum(matrix_float_get_mmult(out[i*2+1],model->nn[i]->W),model->nn[i]->B);
        out[(i+1)*2+1] = matrix_apply_activation(matrix_float_get_copy(out[(i+1)*2]),model->nn[i]->act_func->function);
    }

    if(train){
        matrix_float* deltas[2];
        matrix_float* _W = NULL;

        cost = l2cost(out[model->size*2+1],Y);
        
        for(int i = model->size - 1; i >= 0; i--){
            matrix_float* z = out[(i+1)*2];
            matrix_float* a = out[(i+1)*2+1];
 
            if(i == model->size - 1){
                deltas[0] = matrix_float_mult(matrix_get_l2cost_derivada(a,Y),matrix_apply_activation(z,model->nn[i]->act_func->derivada));
            }else{
                deltas[1] = matrix_float_mult(matrix_float_get_mmult(deltas[0],_W),matrix_apply_activation(z,model->nn[i]->act_func->derivada));
                matrix_float_free_get(&_W);
            }
            matrix_float_free_get(&out[(i+1)*2]);
            matrix_float_free_get(&out[(i+1)*2+1]);
            a = NULL;

            matrix_float* aux;
            matrix_float* transpose;

            if(i != 0){
                _W = matrix_float_get_transpose(model->nn[i]->W);
            }

            if(i != model->size - 1){
                aux = deltas[0];
                deltas[0] = deltas[1];
                matrix_float_free_get(&aux);
            }


            //optimizer_manager* op,matrix_float* m,matrix_float* deltas,number capa,number external,float lr,type_parameter t_p
            //print_matrix(model->nn[i]->B);
            op->f_optimizer(op,model->nn[i]->B,(aux = matrix_get_mean_horizontal(deltas[0])),i,0,lr,PARAMETER_B);
            matrix_float_free_get(&aux);

            //print_matrix(model->nn[i]->W);
            transpose = matrix_float_get_transpose(out[i*2+1]);
            op->f_optimizer(op,model->nn[i]->W,(aux = matrix_float_get_mmult(transpose,deltas[0])),i,0,lr,PARAMETER_W);
            matrix_float_free_get(&transpose);
            matrix_float_free_get(&aux);
        }

        matrix_float_free_get(&deltas[0]);
    }else{
        matrix_float_copy(Y,out[model->size*2+1]);

        for(int i = 1; i < model->size + 1; i++){
            matrix_float_free_get(&out[i*2]);
            matrix_float_free_get(&out[i*2+1]);
        }
            
    }

    free(out);

    return cost;
}

matrix_float* matrix_get_mean_horizontal(matrix_float* a){
    matrix_float* result = (matrix_float*)malloc(sizeof(matrix_float));
    matrix_float_init(result,1,a->width);  

    for(LENGHT i = 0; i < a->width; i++){
        result->pMatrix[i] = 0.0f;
        for(LENGHT j = 0; j < a->height; j++){
            result->pMatrix[i] += a->pMatrix[j * a->width + i];
        }
        result->pMatrix[i] /= a->height;
    }
    return result;
}

matrix_float* matrix_get_l2cost_derivada(matrix_float* Yp,matrix_float* Yr){
    matrix_float* cost = matrix_float_get_rest(Yp,Yr);
    return cost;
}

float l2cost(matrix_float* Yp, matrix_float* Yr){
    float cost = 0;
    LENGHT size = Yp->width * Yp->height;
    for(int i = 0; i < size; i++){
        float rest = Yp->pMatrix[i] - Yr->pMatrix[i];
        cost += (rest * rest);
    }
    cost /= (float)size;
    return cost;
}

#endif