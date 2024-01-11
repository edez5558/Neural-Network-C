#ifndef CONV_MODEL_HPP
#define CONV_MODEL_HPP

#include "conv_layer.h"
#include "matrix_float.h"

struct{
    number kernel_size;
    number kernel_depth;
}typedef kernel_dimension;

struct{
    dimension input;
    dimension output;

    convolutional_layer* layers;
    number n_layers;
}typedef convolutional_model;

void convolutional_model_init(convolutional_model* model,dimension input,number n_layers,kernel_dimension* buff_kernels,type_cr* buff_type){
    model->layers = (convolutional_layer*)malloc(n_layers * sizeof(convolutional_layer));
    
    model->input = input;
    model->n_layers = n_layers;
    //Crear la capa inicial

    for(number i = 0; i < n_layers; i++){    
        if(i == 0){
            convolutional_layer_init(&model->layers[0],model->input,buff_kernels[0].kernel_size,buff_kernels[0].kernel_depth,buff_type[0]);
            convolutional_layer_random_values(&model->layers[0],-1.0f,1.0f);
            continue;
        }
        convolutional_layer_init(&model->layers[i],model->layers[i-1].out_shape,buff_kernels[i].kernel_size,buff_kernels[i].kernel_depth,buff_type[i]);
        convolutional_layer_random_values(&model->layers[i],-1.0f,1.0f);
    }

    model->output = model->layers[n_layers - 1].out_shape;
}

void convolutional_model_save(convolutional_model* model,char* file){
    FILE* file_output = fopen(file,"wb");
    
    if(file_output == NULL){
        printf("Error to save model\n");
        return;
    }

    //Numero de capas
    fwrite(&model->n_layers,sizeof(number),1,file_output);
    fwrite(&model->input,sizeof(dimension),1,file_output); 
    
    for(unsigned int i = 0; i < model->n_layers; i++){
        fwrite(&model->layers[i].kernel_type,sizeof(type_cr),1,file_output);
        fwrite(&model->layers[i].out_shape,sizeof(dimension),1,file_output);    

        number depth = model->layers[i].B.depth;
        number height = model->layers[i].B.matrix->height;
        number width = model->layers[i].B.matrix->width;

        fwrite(&depth,sizeof(LENGHT),1,file_output);
        fwrite(&height,sizeof(LENGHT),1,file_output);
        fwrite(&width,sizeof(LENGHT),1,file_output);

        for(unsigned int d = 0; d < depth; d++)
            fwrite(model->layers[i].B.matrix[d].pMatrix,sizeof(float),height * width,file_output);


        depth = model->layers[i].kernel.depth;
        height = model->layers[i].kernel.matrix->height;
        width = model->layers[i].kernel.matrix->width;

        fwrite(&depth,sizeof(LENGHT),1,file_output);
        fwrite(&height,sizeof(LENGHT),1,file_output);
        fwrite(&width,sizeof(LENGHT),1,file_output);

        for(unsigned int d = 0; d < depth; d++)
            fwrite(model->layers[i].kernel.matrix[d].pMatrix,sizeof(float),height * width,file_output);
        

    }

    fclose(file_output);
}

void convolutional_model_load(convolutional_model* model,char* file){
    FILE* file_input = fopen(file,"rb");

    if(file_input == NULL){
        printf("Error to load model\n");
        return;
    }

    fread(&model->n_layers,sizeof(number),1,file_input);
    fread(&model->input,sizeof(dimension),1,file_input); 

    model->layers = (convolutional_layer*)malloc(model->n_layers * sizeof(convolutional_layer));
    
    for(unsigned int i = 0; i < model->n_layers; i++){
        if(i == 0){
            model->layers[i].in_shape = model->input;
        }else{
            model->layers[i].in_shape = model->layers[i-1].out_shape;
        }
        fread(&model->layers[i].kernel_type,sizeof(type_cr),1,file_input);
        fread(&model->layers[i].out_shape,sizeof(dimension),1,file_input);

        number height = 0;
        number width = 0;
        number depth = 0;

        fread(&depth,sizeof(LENGHT),1,file_input);
        fread(&height,sizeof(LENGHT),1,file_input);
        fread(&width,sizeof(LENGHT),1,file_input);

        model->layers[i].B.depth = depth;
        model->layers[i].depth = depth;

        model->layers[i].B.matrix = (matrix_float*)malloc(depth * sizeof(matrix_float));

        for(unsigned int d = 0; d < depth; d++){
            matrix_float_init(&model->layers[i].B.matrix[d],height,width);
            fread(model->layers[i].B.matrix[d].pMatrix,sizeof(float),height * width,file_input);
        }       

        fread(&depth,sizeof(LENGHT),1,file_input);
        fread(&height,sizeof(LENGHT),1,file_input);
        fread(&width,sizeof(LENGHT),1,file_input);

        model->layers[i].kernel.depth = depth;
        model->layers[i].kernel_size = width;

        model->layers[i].kernel.matrix = (matrix_float*)malloc(depth * sizeof(matrix_float));

        for(unsigned int d = 0; d < depth; d++){
            matrix_float_init(&model->layers[i].kernel.matrix[d],height,width);
            fread(model->layers[i].kernel.matrix[d].pMatrix,sizeof(float),height * width,file_input);
        }
    }

    model->output = model->layers[model->n_layers - 1].out_shape;

    fclose(file_input);

}

void convolutional_model_free(convolutional_model* model){
    for(number i = 0; i < model->n_layers; i++){
        convolutional_layer_free(&model->layers[i]);
    }

    model->input.depth = 0;
    model->input.height = 0;
    model->input.width = 0;
    model->output = model->input;

    model->n_layers = 0;

    free(model->layers);
    model->layers = NULL;
}

#endif