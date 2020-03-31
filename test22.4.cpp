#include <iostream>
#include <cstring>
#include <unistd.h>
#include "vm_app.h"
using namespace std;

int main()
{
    char *s0 = (char *)vm_map(nullptr, 0);
    char *s99 = (char *)vm_map(nullptr, 0);
    char *s98 = (char *)vm_map(nullptr, 0);
    char *s97 = (char *)vm_map(nullptr, 0);
    char *s96 = (char *)vm_map(nullptr, 0);
    char *s95 = (char *)vm_map(nullptr, 0);
    *s0 = 'c';
    int pid = fork();
    if (!pid)
    {
        *s0 = 'c';
        vm_yield();
        char *s1 = (char *)vm_map(nullptr, 0);
        char *s2 = (char *)vm_map(nullptr, 0);
        char *s3 = (char *)vm_map(nullptr, 0);
        char *s4 = (char *)vm_map(nullptr, 0);
        char *s5 = (char *)vm_map(nullptr, 0);
        char *s6 = (char *)vm_map(nullptr, 0);
        char *s7 = (char *)vm_map(nullptr, 0);
        char *s8 = (char *)vm_map(nullptr, 0);
        cout << *s99 << endl;
        *s99 = 'c';
        *s98 = 'c';
        cout << *s97 << endl;
        vm_yield();
        *s96 = 'c';
        cout << *s0 << endl;
        cout << *s99 << endl;
        *s97 = 'c';
        *s96 = 'c';
    }
    else
    {
        fork();
        vm_yield();
        char *s1 = (char *)vm_map(nullptr, 0);
        char *s2 = (char *)vm_map(nullptr, 0);
        char *s3 = (char *)vm_map(nullptr, 0);
        char *s4 = (char *)vm_map(nullptr, 0);
        char *s5 = (char *)vm_map(nullptr, 0);
        char *s6 = (char *)vm_map(nullptr, 0);
        char *s7 = (char *)vm_map(nullptr, 0);
        char *s8 = (char *)vm_map(nullptr, 0);
        *s1 = 'c';
        *s2 = 'c';
        strcpy(s3, "");
        *s4 = 'c';
        *s5 = 'c';
        cout << *s1 << endl;
        cout << *s2 << endl;
        cout << *s3 << endl;
        cout << *s8 << endl;
        *s8 = 'c';
        cout << *s4 << endl;
        cout << *s5 << endl;
        *s7 = 'c';
        cout << *s3 << endl;
        vm_yield();
        int pid0 = fork();
        if (pid0)
        {
            *s0 = 'c';
            *s1 = 'c';
            *s2 = 'c';
            cout << *s8 << endl;
        }
        else
        {
            *s0 = 'c';
            cout << *s1 << endl;
            cout << *s2 << endl;
            cout << *s3 << endl;
        }
        vm_yield();
        *s8 = 'c';
        *s7 = 'c';
        cout << *s0 << endl;
    }
}