#include "ops.h"
#include <math.h>
#include <float.h>
#include <stdio.h>






//only sets the varibale part of the expression struct
expression constructWeight(float value){
    expression weight;   
    weight.value = value;
    weight.a = NULL;
    weight.b = NULL;
    weight.func = NULL;
    weight.der = NULL;
    weight.forwardValue = value;
    weight.partialDerivateBackwardValue = 0;
    return weight;
}

expression constructConstant(float value){
    expression constant;
    constant.value = value;
    constant.a = NULL;
    constant.b = NULL;
    constant.func = NULL;
    constant.der = NULL;
    constant.forwardValue = value;
    constant.partialDerivateBackwardValue = NAN;
    return constant;
}

//sets a*b, where * is an operator and a, b are expressions
//AND sets the varibale part of the expression struct as the 
//smallest float so the program can know when it is dealing
//with an actual expression or with a variable pretending to be an expression
expression constructEvaluation(expression* a, expression* b, Operation func, OperationDer  der){
    expression eval;
    eval.value = NAN; 
    eval.a =  a; 
    eval.b =  b;
    eval.func = func;
    eval.der = der;
    eval.forwardValue = 0;
    eval.partialDerivateBackwardValue = NAN;
    return eval;
}
 
expression constructNullVar(){
    expression nullVar;
    nullVar.value = NAN;
    nullVar.a = NULL;
    nullVar.b = NULL;
    nullVar.func = NULL;
    nullVar.der = NULL;
    nullVar.forwardValue = NAN;
    nullVar.partialDerivateBackwardValue = NAN;
    return nullVar;
}

void setConstantValue(expression* constant, float value){
    (*constant).value = value;
   (*constant).forwardValue = value;
}

//evaluates an expression recursively by traversing the graph
float evaluate (expression* exp){
    if ((*exp).a == NULL && (*exp).b == NULL){
        return (*exp).value;
    }
    (*exp).forwardValue = (*exp).func(evaluate((*exp).a), evaluate((*exp).b));
    return (*exp).forwardValue;
}
           
void derive (expression* exp, float seed){
    if ((*exp).a == NULL && (*exp).b == NULL ){
        (*exp).partialDerivateBackwardValue+=seed;
    }
    else {
        (*exp).der((*exp).a,(*exp).b, seed);
    }
    
}

void clearCacheForward(expression* exp){
    if (!((*exp).a == NULL && (*exp).b == NULL)){
        (*exp).forwardValue = 0;
        clearCacheForward((*exp).a);
        clearCacheForward((*exp).b);
    } 
}

void clearCachePartialBackward(expression* exp){
        if ((*exp).a == NULL && (*exp).b == NULL && (*exp).partialDerivateBackwardValue != NAN){
                (*exp).partialDerivateBackwardValue=0;
            }
        else{
            clearCachePartialBackward((*exp).a);
            clearCachePartialBackward((*exp).b);
        }
}

void optimizeWeightsGradientDescent(expression* exp, float learningRate){
    if ((*exp).a == NULL && (*exp).b == NULL && (*exp).partialDerivateBackwardValue != NAN){
        float weightUpdate = (-1)*(*exp).partialDerivateBackwardValue*learningRate;
        (*exp).value += weightUpdate;
        (*exp).forwardValue += weightUpdate;
    }
    else{
    optimizeWeightsGradientDescent((*exp).a,  learningRate);
    optimizeWeightsGradientDescent((*exp).b,  learningRate);
    }   
}

float squaredError(float y, float p){
    float error = y-p;
    return error*error;
}
void squaredErroDer(expression* y, expression* p, float seed){
    
    derive(p,-2*((*y).forwardValue - (*p).forwardValue)*seed);
}

float addition(float a, float b){
    return a+b;
}
void additionDer(expression* a, expression* b, float seed){
   
    derive(a,seed);
    derive(b,seed);
}
float multiplication(float a, float b){
    return a*b;
}
void multiplicationDer(expression* a, expression* b, float seed){
    derive(a,(*b).forwardValue*seed);
    derive(b,(*a).forwardValue*seed);
}
float ReLu(float a, float b){
    if (a>0){
        return a;
    }
    return 0;
}
void ReLuDer(expression* a, expression* b, float seed){
    if ((*a).forwardValue >0){
    derive(a,seed);}
    else{
        derive(a,0);
    }
}

Operation sqErr = &squaredError;
OperationDer sqErrDer = &squaredErroDer;
Operation add =  &addition;
OperationDer addDer =  &additionDer;
Operation mul = &multiplication;
OperationDer mulDer = &multiplicationDer;
Operation relu = &ReLu;
OperationDer reluDer = &ReLuDer;



