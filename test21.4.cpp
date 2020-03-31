#include <iostream>
#include <cstring>
#include <unistd.h>
#include "vm_app.h"
using namespace std;

int main()
{
    int *p1 = (int *)vm_map(nullptr, 0);
    int *p2 = (int *)vm_map(nullptr, 0);
    int *p3 = (int *)vm_map(nullptr, 0);
    int *p4 = (int *)vm_map(nullptr, 0);
    int *p5 = (int *)vm_map(nullptr, 0);
    *p2 = 2;
    *p3 = 3;
    *p4 = 4;
    *p5 = 5;
    int cpid = fork();
    if(cpid == 1){
        vm_yield();
        int ccpid = fork();
        if(ccpid == 1){
            *p1 = 11;
            *p2 = 22;
            *p3 = 33;
            *p4 = 44;
            cout<<*p5<<endl;
            int cccpid = fork();
            if(cccpid == 1)
            {
                cout<<*p1;
                vm_yield();
                *p1 = 111;
                vm_yield();
            }
        }
        vm_yield();
    }
}
