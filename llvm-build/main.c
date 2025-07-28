#include <stdio.h>

int main(void){

    int x = 22;
    
    if(x > 60 && x <= 70){
        if(x > 65 && x <= 58){
            return 25;
        }else{
            return 20;        
        }
    }else{
        return 55;
        
    }
    
    return 0;
}
