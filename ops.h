#ifndef OPS_H_   /* Include guard */
#define OPS_H_

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

expression constructWeight(float value);
expression constructConstant(float value);
expression constructEvaluation(expression* a, expression* b, Operation func, OperationDer  der);
expression constructNullVar();

void setConstantValue(expression* constant, float value);

float evaluate (expression* exp);
void derive (expression* exp, float seed);

void clearCacheForward(expression* exp);
void clearCachePartialBackward(expression* exp);

void optimizeWeightsGradientDescent(expression* exp, float learningRate);

float squaredError(float y, float p);
void squaredErroDer(expression* y, expression* p, float seed);

float addition(float a, float b);
void additionDer(expression* a, expression* b, float seed);

float multiplication(float a, float b);
void multiplicationDer(expression* a, expression* b, float seed);

float ReLu(float a, float b);
void ReLuDer(expression* a, expression* b, float seed);

Operation sqErr ;
OperationDer sqErrDer ;
Operation add ;
OperationDer addDer ;
Operation mul ;
OperationDer mulDer ;
Operation relu ;
OperationDer reluDer;


#endif 