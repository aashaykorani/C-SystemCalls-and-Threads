#include <stdio.h>
#include <stdlib.h>

int main(){
    system("gcc run.c -lm -o run");
    system("gcc run2.c -lm -o run2");
    system("gcc system_call.c -lm -o system_call");
    system("gcc fast.c -lm -o fast -Wno-int-to-pointer-cast -Wno-pointer-to-int-cast -Wno-format");
    return 0;
}