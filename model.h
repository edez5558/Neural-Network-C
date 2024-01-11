#ifndef MODEL_HPP
#define MODEL_HPP

#include "nlayer.h"

typedef struct neural_model_struct{
    neural_layer** nn;
    unsigned int size;
    int* topology;
    act_function* functions;
}neural_model;

void neural_model_init(neural_model* m){
    m->functions = NULL;
    m->topology = NULL;
    m->nn = NULL;
    m->size = 0;
}

void neural_model_init_void(neural_model* m,int* topology,int size,act_function* function){
    m->nn = create_neural_layers(topology,size,function);

    for(LENGHT i = 0; i < size - 1; i++)
        neural_layer_random_values(m->nn[i],-1.0f,1.0f);
    
    m->size = size - 1;
    m->topology = (int*)malloc((size + 1) * sizeof(int));
    memcpy(m->topology,topology,(size + 1) * sizeof(int));
    m->functions = (act_function*)malloc((size - 1) * sizeof(act_function));
    memcpy(m->functions,function,(size - 1) * sizeof(act_function));
}

void neural_model_free(neural_model* m){
    for(LENGHT i = 0; i < m->size; i++){
        neural_layer_free(m->nn[i]);
        free(m->nn[i]);
        m->nn[i] = NULL;    
    }
    free(m->nn);
    m->nn = NULL;
    free(m->topology);
    m->topology = NULL;
    free(m->functions);
    m->functions = NULL;
}

void neural_model_save(neural_model* m,const char* path){
    FILE* file = fopen(path,"wb");
    if(file == NULL){
        return;
    }

    fwrite(&m->size,sizeof(unsigned int),1,file);
    fwrite(m->topology,sizeof(int),m->size + 1,file);
    fwrite(m->functions,sizeof(act_function),m->size,file);

    for(LENGHT i = 0; i < m->size; i++){
        fwrite(m->nn[i]->W->pMatrix,sizeof(float),m->nn[i]->W->height * m->nn[i]->W->width,file);
        fwrite(m->nn[i]->B->pMatrix,sizeof(float),m->nn[i]->B->height * m->nn[i]->B->width,file);
    }
}

void neural_model_load(neural_model* m,const char* path){
    FILE* file = fopen(path,"rb");

    if(file == NULL){
        return;
    }

    neural_model_free(m);

    fread(&m->size,sizeof(unsigned int),1,file);
    fread(m->topology,sizeof(int),m->size + 1,file);
    fread(m->functions,sizeof(act_function),m->size,file);

    m->nn = create_neural_layers(m->topology,m->size,m->functions);
    for(LENGHT i = 0; i < m->size; i++){
        fread(m->nn[i]->W->pMatrix,sizeof(float),m->nn[i]->W->height * m->nn[i]->W->width,file);
        fread(m->nn[i]->B->pMatrix,sizeof(float),m->nn[i]->B->height * m->nn[i]->B->width,file);
    }
}

#endif