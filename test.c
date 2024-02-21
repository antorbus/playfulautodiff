#include <stdio.h>
#include <math.h>
#include <float.h>

typedef float (*Operation)(float, float); 
typedef struct OperationDer* (**FunctionPointer)(); 
typedef struct { 
    struct expression* a; 
    Operation func;
    //OperationDer der; 
    FunctionPointer der;
} expression;
typedef void (*OperationDer)( expression*,  expression*, float); 

float multiplication(float a, float b){
    return 0;
}
void multiplicationDer(expression* a, expression* b, float seed){}

OperationDer temp(){
    return multiplicationDer;
}
Operation mul = &multiplication;
//OperationDer mulDer = &multiplicationDer;
FunctionPointer mulDer = temp;

int main(){
    expression test;
    test.func = mul;
    test.der = mulDer; 
    return 0;
}