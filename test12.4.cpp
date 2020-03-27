#include <iostream>
#include <cstring>
#include <unistd.h>
#include "vm_app.h"
using namespace std;

int main(){
    char* filename1 = (char*) vm_map(nullptr, 0);
    strcpy(filename1, "shakespeare.txt");
    cout << *filename1 << endl;
    char *filename2 = (char*) vm_map(filename1, 0);
    char *filename3 = (char*) vm_map(filename1, 0); 
    cout << *filename3 << endl;
    int pid = fork();
    if(!pid){
        int pid2 = fork();
        if(!pid2){
            cout << *filename1 << endl;
        }else{
            cout << *filename1 << endl;
            cout << "dsfsd" << endl;
            strcpy(filename1, "data1.bin");
            char *filename4 = (char*) vm_map(filename1, 0);
            char *filename5 = (char*) vm_map(filename1, 0);
            cout << *filename4 << endl; 
            //strcpy(filename2, "shakespeare.txt");
            // filename2 = (char*) vm_map(nullptr, 0);
            // strcpy(filename2, "shakespeare.txt");
            // filename2 = (char*) vm_map(nullptr, 0);
            // strcpy(filename2, "shakespeare.txt");
        }
    }else{
        *filename1 = '9';
    }
    return 0;
}
