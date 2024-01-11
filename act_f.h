#ifndef ACT_F_HPP
#define ACT_F_HPP

#include <math.h>

#define SIGM_F 1
#define RELU_F 2
#define TANH_F 3
#define LINE_F 4

typedef struct act_f_struct{
    float (*function)(float);
    float (*derivada)(float);
} act_f;

float sigmFunction(float x){
    return (1.0/(1.0 + exp(-x)));
}

float sigmDerivada(float x){
    return (sigmFunction(x) * (1 - sigmFunction(x)));
}

float reluFunction(float x){
    return x >= 0.0? x : 0;
}

float reluDerivada(float x){
    return x >= 0.0? 1 : 0;
}

float tanhFunction(float x){
    return tanh(x);
}

float tanhDerivada(float x){
    double tmp = tanh(x);
    return (1 - (tmp * tmp));
}

float lineFunction(float x){
    return x;
}

float lineDerivada(float x){
    return 1.0f;
}

float cuadFunction(float x){
    return x * x;
}

float cuadDerivada(float x){
    return x;
}

static act_f act_f_line = {&lineFunction,&lineDerivada};
static act_f act_f_tanh = {&tanhFunction,&tanhDerivada};
static act_f act_f_relu = {&reluFunction,&reluDerivada};
static act_f act_f_sigm = {&sigmFunction,&sigmDerivada};

act_f* parse_act_f(unsigned int act){
    switch(act){
        case SIGM_F: return &act_f_sigm;
        case TANH_F: return &act_f_tanh;
        case RELU_F: return &act_f_relu;
        case LINE_F: return &act_f_line;
        default:     return NULL;
    }
}

#endif