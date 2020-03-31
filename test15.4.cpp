#include <iostream>
#include <cstring>
#include <unistd.h>
#include "vm_app.h"
using namespace std;

int main(){
    char* filename1 = (char*) vm_map(nullptr, 0);
    strcpy(filename1, "data1.bin");
    char* filename2 = (char*) vm_map(nullptr, 0);
    strcpy(filename2, "");
    char* block1 = (char*) vm_map(filename1, 1);
    char* block2 = (char*) vm_map(filename1, 2);
    int pid = fork();
    if(!pid){
        cout << *block1 << endl;
        cout << "that's it" << endl; 
    }
    else{
        memcpy(block1, filename1, VM_PAGESIZE);
        memcpy(block2, filename2, VM_PAGESIZE);
        cout << *block1 << endl;
        cout << *block2 << endl;
        char* block3 = (char*) vm_map(block1, 0);
        cout << *block3 << endl;
        char* block4 = (char*) vm_map(block2, 0);
        cout << *block4 << endl;
    }
    return 0;
}
