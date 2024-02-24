#include <stdio.h>
#include <math.h>
#include <float.h>
#include "ops.h"

//the type *Operation is a pointer to a function such as add(a,b)
typedef float (*Operation)(float, float); 

//an expression is either a variable such as 5 OR a function 
//which has not been evaluated of two expressions 
//such as expr1 + 3 where expr1 = 4 + 6 (remember that the numbers are expresions too) 
//in order to enforce this expressions are created one of two constructors

struct _expression; //needed for averting warnings

typedef void (*OperationDer)(struct _expression*,  struct _expression*, float); 


typedef struct _expression{ //CHANGE SO TTHAT ONLY V IS USED AND OTHER THIGNS CMPRE TO NULL AND USE ->
    float value; //stores intermediate value of variable
    struct _expression* a; //since this can be recursive and will end in a variable
    struct _expression* b; //and so can be thought of as being the graph
    Operation func;
    OperationDer der; 
    float forwardValue; //stores intermediate result of expression 
    float partialDerivateBackwardValue; //stores value of partial derivative 
                                //with respect to outmost expression 
} expression;




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



Operation add =  &addition;
OperationDer addDer =  &additionDer;
Operation mul = &multiplication;
OperationDer mulDer = &multiplicationDer;
Operation relu = &ReLu;
OperationDer reluDer = &ReLuDer;
Operation sqErr = &squaredError;
OperationDer sqErrDer = &squaredErroDer;



int main(){

    float inputs[] = {-2.3,-1.8,0.2,1.1,4.7,6,9,10,2,3,0,-1,-10};
    int length = sizeof(inputs) / sizeof(inputs[0]);
    float targets[length];
    for (int i =0; i<length;++i ){
        targets[i] = inputs[i]*2 - 3;
    }
    printf("Attempting to construct \n");
    expression x = constructWeight(0.1); 
    expression y = constructWeight(0.78);
    expression input = constructConstant(NAN);
    expression target = constructConstant(NAN);
    expression w1 = constructEvaluation(&x,&input,mul,mulDer);
    expression w2 = constructEvaluation(&y,&w1,add,addDer);
    expression loss = constructEvaluation(&target,&w2,sqErr,sqErrDer);
    int epochs = 1000;
    float lr = 0.01;
    printf("Training \n");
    for (int i =1; i<=epochs;i++){
        
        float lossEpoch =0;
        for (int datumIdx =0; datumIdx<length;++datumIdx ){
            setConstantValue(&input,inputs[datumIdx]);
            setConstantValue(&target,targets[datumIdx]);
            float output = evaluate(&loss);
            lossEpoch += output/length;
            derive(&loss,1);
            clearCacheForward(&loss);
        }
        if (i%24 ==1){
            printf("Epoch %d \n", i); 
            printf("Loss %f \n", lossEpoch); }
       
        x.partialDerivateBackwardValue /= length;
        y.partialDerivateBackwardValue /= length;
        //printf("d of x %f \n", x.partialDerivateBackwardValue); 
        //printf("d of y %f \n", y.partialDerivateBackwardValue); 

        optimizeWeightsGradientDescent(&loss, lr);
        clearCachePartialBackward(&loss);
    }
    printf("Weight x is %f \n", x.value); 
    printf("Weight y is %f \n", y.value); 
    return 0;
}




/*/
     printf("Attempting to construct \n");
    expression x = constructWeight(0.1); 
    expression y = constructWeight(-0.2);
    expression nullVar = constructNullVar();
    expression w1 = constructEvaluation(&x,&y,add,addDer);
    expression w2 = constructEvaluation(&x,&w1,mul,mulDer);
    expression w3 = constructEvaluation(&y,&y,mul,mulDer);
    expression w4 = constructEvaluation(&w3,&w2,add,addDer);
    expression w5 = constructEvaluation(&w4,&nullVar,relu,reluDer);
    float output = evaluate(&w5);
    printf("Output is %f \n", output); 
    derive(&w5,1);
    printf("derivative of w5 with respect to x %f \n", x.partialDerivateBackwardValue); 
    printf("derivative of w5 with respect to y %f \n", y.partialDerivateBackwardValue);
/*/

/*/
expression x1 = constructVariable(3); //variable x1 = 3 
expression x2 = constructVariable(4.2); //variable x2 = 4.2 
expression w1 = constructEvaluation(&x1,&x2,add, addDer); //w1 = x1 + x2 = 7.2
expression w2 = constructEvaluation(&w1,&x2,add, addDer); //w2 = w1 + x2 = 11.4
expression w3 = constructEvaluation(&w2,&w1,mul, mulDer); //w3 = w2 * w1 = 82.08
float output = evaluate(&w3); //  w3 = w2 * w1 = (w1 + x2) * (x1 + x2) 
                                //  = ((x1 + x2)+x2) * (x1 + x2) 
    
printf("%f \n", output); 
printf("%f \n", w2.forwardValue); 
printf("%f \n", w1.forwardValue); 
/*/