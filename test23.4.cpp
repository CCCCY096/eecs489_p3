#include <iostream>
#include <cstring>
#include <unistd.h>
#include "vm_app.h"
using namespace std;
int main(){
    char* b1 = (char* )vm_map(nullptr, 0);
    char* b2 = (char* )vm_map(nullptr, 0);
    strcpy(b1, "pikachu");
    int pid = fork();
    if(!pid){
        strcpy(b1, "pikachu");
        fork();
        vm_yield();
        char* b3 = (char* )vm_map(nullptr, 0);
        strcpy(b1, "pikachu");
        char* b4 = (char* )vm_map(nullptr, 0);
        strcpy(b3 + 200, "rachu");
        cout << *b4 << endl;
        char* b5 = (char* )vm_map(nullptr, 0);
        char* b6 = (char* )vm_map(nullptr, 0);
        char* b7 = (char* )vm_map(nullptr, 0);
        char* b8 = (char* )vm_map(nullptr, 0);
        char* b9 = (char* )vm_map(nullptr, 0);
        char* b10 = (char* )vm_map(nullptr, 0);
        strcpy(b6, "pikachu");
        strcpy(b9, "kono dio da!");
        cout << b1[50] << endl;
        cout << b2[50] << endl;
        cout << b3[250] << endl;
        strcpy(b10, "pikachu");
        cout << b5[50] << endl;
        cout << b6[0] << endl;
        vm_yield();
        strcpy(b8, "kono dio da!");
        cout << b1[50] << endl;
        strcpy(b6, "pikachu");
        cout << b3[250] << endl;
        strcpy(b3 + 800, "rachu");
        cout << b5[50] << endl;
        strcpy(b7, "kono dio da!");
    }
    else{
        vm_yield();
        vm_map(nullptr, 0);
        char* b3 = (char* )vm_map(nullptr, 0);
        strcpy(b1, "pikachu");
        char* b4 = (char* )vm_map(nullptr, 0);
        strcpy(b3 + 200, "rachu");
        cout << *b4 << endl;
        char* b5 = (char* )vm_map(nullptr, 0);
        char* b6 = (char* )vm_map(nullptr, 0);
        strcpy(b6, "pikachu");
        cout << b1[50] << endl;
        cout << b2[50] << endl;
        cout << b3[250] << endl;
        cout << b4[5050] << endl;
        cout << b5[50] << endl;
        cout << b6[0] << endl;
        vm_yield();
        cout << b1[50] << endl;
        strcpy(b6, "pikachu");
        cout << b3[250] << endl;
        strcpy(b3 + 800, "rachu");
        cout << b5[50] << endl;
        cout << b6[0] << endl;
        
    }
    return 0;
}