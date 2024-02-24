#include "ops.h"

#include <stdio.h>
#include <math.h>
#include <float.h>





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