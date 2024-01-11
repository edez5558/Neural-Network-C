#ifndef CONV_LEARNING_HPP
#define CONV_LEARNING_HPP

#define TRUE  1
#define FALSE 0

#include "conv_model.h"
#include "optimizer.h"

#include <windows.h>

#include "convert.h"

typedef char bool_t;

void convolutional_get_forward(convolutional_layer* n,matrix_float_array* X,matrix_float_array* Y){
    matrix_float_array_init(&Y[0],n->out_shape.height,n->out_shape.width,n->out_shape.depth);
    matrix_float_array_init(&Y[1],n->out_shape.height,n->out_shape.width,n->out_shape.depth);

    for(int i = 0; i < Y->depth; i++){
        matrix_float_insert(&Y[0].matrix[i],0.0f);

        for(int depth_input = 0; depth_input < n->in_shape.depth; depth_input++){
            if(n->kernel_type == KERNEL_VALID)
                matrix_correlation_valid(&X->matrix[depth_input],&n->kernel.matrix[depth_input * n->out_shape.depth + i],&Y[0].matrix[i]);
            else
                matrix_correlation_full(&X->matrix[depth_input],&n->kernel.matrix[depth_input * n->out_shape.depth + i],&Y[0].matrix[i]);
        }

        matrix_float_sum(&Y[0].matrix[i],&n->B.matrix[i]);
        matrix_float_set(&Y[1].matrix[i],Y[0].matrix[i].pMatrix);

        matrix_float_sigm_apply(&Y[1].matrix[i]);
    }

}

matrix_float_array convolutional_get_backward(convolutional_layer* n,matrix_float_array* input,matrix_float_array* y_raw,matrix_float_array* derived,optimizer_manager* op,number capa,float lr){
    matrix_float_array x_derived;
    matrix_float_array_init(&x_derived,n->in_shape.height,n->in_shape.width,n->in_shape.depth);
    
    matrix_float kernel_tmp;
    matrix_float_init(&kernel_tmp,n->kernel_size,n->kernel_size);

    matrix_float_array_mult_ab(derived,matrix_float_array_sigm_prime_a(y_raw));

    for(LENGHT i_depth = 0; i_depth < x_derived.depth; i_depth++){
        matrix_float_insert(&x_derived.matrix[i_depth],0.0f);
        
        for(LENGHT k_depth = 0; k_depth < n->depth; k_depth++){
            matrix_float_insert(&kernel_tmp,0.0f);
            number kernel_location = i_depth * n->depth + k_depth;

            if(n->kernel_type == KERNEL_VALID){
                //X Derivada
                matrix_convolution_full(&derived->matrix[k_depth],&n->kernel.matrix[kernel_location],&x_derived.matrix[i_depth]);
                
                //K Derivada
                matrix_correlation_valid(&input->matrix[i_depth],&derived->matrix[k_depth],&kernel_tmp);

                op->f_optimizer(op,&n->kernel.matrix[kernel_location],&kernel_tmp,capa,kernel_location,lr,PARAMETER_K);
            }else{
                //X Derivada
                matrix_convolution_valid(&derived->matrix[k_depth],&n->kernel.matrix[kernel_location],&x_derived.matrix[i_depth]);
                //K Derivada
                matrix_correlation_valid(&derived->matrix[k_depth],&input->matrix[i_depth],&kernel_tmp);
                matrix_rotation_180(&kernel_tmp);
                
                op->f_optimizer(op,&n->kernel.matrix[kernel_location],&kernel_tmp,capa,kernel_location,lr,PARAMETER_K);
            }
        }
    }

    for(LENGHT k_depth = 0; k_depth < n->depth; k_depth++){
        op->f_optimizer(op,&n->B.matrix[k_depth],&derived->matrix[k_depth],capa,k_depth,lr,PARAMETER_B);
    }

    matrix_float_free(&kernel_tmp);

    

    return x_derived;
}

float convolutional_l2cost(matrix_float_array* Yp,matrix_float_array* Yr){
    float cost = 0.0f;
    float rest = 0.0f;

    number size = Yp->matrix[0].height * Yp->matrix[0].width;

    for(number d = 0; d < Yp->depth; d++){
        for(number p = 0; p < size; p++){
            float v_yr = Yr->matrix[d].pMatrix[p];
            float v_yp = Yp->matrix[d].pMatrix[p];
            rest += -(v_yr * log10f(v_yp) + (1.0f - v_yr) * log10f(1.0f - v_yp));
        }
        rest /= (float)size;
    }
    rest /= (float)Yp->depth;

    return rest;
    /*
    float cost = 0.0f;
    float rest;


    number size = Yp->matrix[0].height * Yp->matrix[0].width;

    for(number d = 0; d < Yp->depth; d++){
        for(number p = 0; p < size; p++){
            rest = Yp->matrix[d].pMatrix[p] - Yr->matrix[d].pMatrix[p];
            cost += rest * rest;
        }
        cost /= (float)size;
    }
    cost /= (float)Yp->depth;

    return cost; 
    */
}


matrix_float_array convolutional_get_l2cost_derivada(matrix_float_array* Yp,matrix_float_array* Yr){
    matrix_float_array derived;
    
    matrix_float_array_init(&derived,Yp->matrix[0].height,Yp->matrix[0].width,Yp->depth);

    number size = Yp->matrix[0].height * Yp->matrix[0].width;

    for(number d = 0; d < Yp->depth; d++){
        for(number i = 0; i < size; i++){
            float v_yr = Yr->matrix[d].pMatrix[i];
            float v_yp = Yp->matrix[d].pMatrix[i];

            derived.matrix[d].pMatrix[i] = (((1.0f-v_yr)/(1.0f-v_yp)) - (v_yr/v_yp))/(float)size;
        }
    }

    /*
    for(number d = 0; d < Yp->depth; d++){
        matrix_float_rest_tri(&Yp->matrix[d],&Yr->matrix[d],&derived.matrix[d]);
    }
    */
    return derived;
}

float convolutional_model_execute(convolutional_model* model,matrix_float_array* X,matrix_float_array* Y,bool_t train,optimizer_manager* op,float lr,HDC* hdc,BITMAPINFO* bitmap,void* memory){
    matrix_float_array* outputs = (matrix_float_array*)malloc((model->n_layers + 1) * 2 * sizeof(matrix_float_array));

    outputs[0] = *X;
    outputs[1] = *X;

    for(number i = 1; i <= model->n_layers; i++){
        convolutional_get_forward(&model->layers[i-1],&outputs[(i-1)*2+1],&outputs[i*2]);
    }

    float l2cost = 0.0f;

    convert_matrix_to_buffer(&outputs[model->n_layers*2+1],(color_buffer*)memory);
    StretchDIBits(*hdc,180,0,160,160,0,0,40,40,memory,bitmap,DIB_RGB_COLORS,SRCCOPY);

    if(train){
		l2cost = convolutional_l2cost(&outputs[model->n_layers * 2 + 1],Y);
		
        matrix_float_array Yd = convolutional_get_l2cost_derivada(&outputs[model->n_layers * 2 + 1],Y);
        matrix_float_array Xd;

        for(int i = model->n_layers - 1;i >= 0;i--){
            Xd = convolutional_get_backward(&model->layers[i],&outputs[i*2+1],&outputs[(i+1)*2],&Yd,op,i,lr);
            matrix_float_array_free(&Yd);
            Yd = Xd;
        }

        matrix_float_array_free(&Yd); 

        FREE_OUTPUTS:
        for(number i = 1; i <= model->n_layers; i++){
            matrix_float_array_free(&outputs[i*2]);
            matrix_float_array_free(&outputs[i*2+1]);
        }

        free(outputs);

        return l2cost;
    }

    
    
    goto FREE_OUTPUTS;
}
    

#endif