#include <iostream>
#include <cstring>
#include <unistd.h>
#include "vm_app.h"
using namespace std;

int main(){
    char* filename1 = (char*) vm_map(nullptr, 0);
    strcpy(filename1, "shakespeare.txt");
    cout << *filename1 << endl;
    filename1 = (char*) vm_map(filename1, 0);
    int pid = fork();
    if(!pid){
        int pid2 = fork();
        if(!pid2){
            cout << *filename1 << endl;
        }else{
            cout << *filename1 << endl;
            *filename1 = 'e';
            char* filename2 = (char*) vm_map(nullptr, 0);
            strcpy(filename2, "shakespeare.txt");
            filename2 = (char*) vm_map(nullptr, 0);
            strcpy(filename2, "shakespeare.txt");
            filename2 = (char*) vm_map(nullptr, 0);
            strcpy(filename2, "shakespeare.txt");
        }
    }else{
        *filename1 = '9';
    }
    return 0;
}
