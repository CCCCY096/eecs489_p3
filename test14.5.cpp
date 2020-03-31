#include <iostream>
#include <cstring>
#include <unistd.h>
#include "vm_app.h"
using namespace std;

int main(){
    char* filename1 = (char*) vm_map(nullptr, 0);
    strcpy(filename1, "data1.bin");
    char* block1 = (char*) vm_map(filename1, 1);
    char* block2 = (char*) vm_map(filename1, 2);
    char* block3 = (char*) vm_map(filename1, 3);
    char* block4 = (char*) vm_map(filename1, 4);
    int pid = fork();
    if(!pid){
        cout << *filename1 << endl;
        cout << "that's it" << endl; 
    }
    else{
        int pid2 = fork();
        if( !pid2 ){
            vm_yield();
            char* block11 = (char*) vm_map(filename1, 1);
            char* block22 = (char*) vm_map(filename1, 2);
            char* block33 = (char*) vm_map(filename1, 3);
            vm_yield();
            char* block44 = (char*) vm_map(filename1, 4);
            cout << *block11 << *block22 << *block33 << *block44 << endl;
            block44 = (char*) vm_map(filename1, 5);
            vm_yield();
        }else{
            char* block5 = (char*) vm_map(filename1, 5);
            cout<< "read to mem: "<< *block1 << endl;
            *block1 = '1';
            vm_yield();
            cout<< "read to mem 2: "<< *block2 << endl;
            *block3 = '3';
            cout<< "read to mem 4: "<< *block4 << endl;
            cout<< "read to mem 5: "<< *block5 << endl;
            // char* filename2 = (char*) vm_map(nullptr, 0);
            // strcpy(filename2, "shakespeare.txt");
            // cout << *filename1 << endl; // 1 in 13
            cout << *block1 << endl; //1 in 12, 2 in 16
            cout << *block2 << endl; //2 in 15, 3 in 13
            *block3 = '3'; //3 in 12, 4 in 16
            *block4 = '4'; //4 in 12, 5 in 16
            *filename1 = 'w';
            cout << *filename1 << endl;
            cout << *block5 << endl;
            vm_yield();
        }
    }
    return 0;
}
