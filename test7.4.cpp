#include <iostream>
#include <cstring>
#include <unistd.h>
#include "vm_app.h"

using namespace std;

int main(){
    char* filename = (char*) vm_map(nullptr, 0);
    strcpy(filename, "data1.bin");
    // To make filname dirty and not referenced ////
    char *tmp1 = (char*) vm_map(nullptr, 0);
    char *tmp2 = (char*) vm_map(nullptr, 0);
    char *tmp3 = (char*) vm_map(nullptr, 0);
    char *tmp4 = (char*) vm_map(nullptr, 0);
    strcpy(tmp1, "data1.bin");
    strcpy(tmp2, "data1.bin");
    strcpy(tmp3, "data1.bin");
    strcpy(tmp4, "data1.bin");
    /////////////////////////////////////////////////
    char *p1 = (char *) vm_map (filename, 0);
    int pid = fork();
    if (!pid){
        cout << *filename << endl;
        cout << *p1;
        char *p2 = (char *) vm_map (filename, 1);
        cout << *p2 << endl;
        return 0;
    }else{
        char *p2 = (char *) vm_map (filename, 1);
        char *p3 = (char *) vm_map (filename, 1);
        filename = (char*) vm_map(nullptr, 0);
        cout << *p2 << endl;
        cout << *p3 << endl;
        return 0;
    }
    // return 0;
}