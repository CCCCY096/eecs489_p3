#include <iostream>
#include <cstring>
#include <unistd.h>
#include "vm_app.h"
using namespace std;

int main(){
    char* block1 = (char*) vm_map(nullptr, 0);
    strcpy(block1, "data1.bin"); 
    int pid = fork();
    if (pid) {
        int pid2 = fork();
    }
    return 0;
}