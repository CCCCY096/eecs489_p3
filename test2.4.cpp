#include <iostream>
#include <cstring>
#include <unistd.h>
#include "vm_app.h"

using namespace std;

int main(){
    char* filename1 = (char*) vm_map(nullptr, 0);
    char* filename2 = (char*) vm_map(nullptr, 0);
    char* filename3 = (char*) vm_map(nullptr, 0);
    char* filename4 = (char*) vm_map(nullptr, 0);
    char* filename5 = (char*) vm_map(nullptr, 0);
    char* filename6 = (char*) vm_map(nullptr, 0);
    cout << *filename1 << endl;
    strcpy(filename1, "shakespeare.txt");
    cout << *filename1 << endl;
    strcpy(filename2, "pikachu");
    strcpy(filename3, "charizard");
    strcpy(filename4, "squirtle");
    strcpy(filename5, "ash");
    cout << *filename5 << " read " << *filename1 << endl;
    cout << *filename1 << endl;
    strcpy(filename1, "shakespeare");
    int pid = fork();
    if ( !pid ){
       strcpy(filename2, "dead_pikachu");
       strcpy(filename3, "dead_charizard"); 
    }else{
       strcpy(filename2, "rachu");
       strcpy(filename3, "mega charizard");
    }
    return 0;
}