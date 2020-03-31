#include <iostream>
#include <cstring>
#include <unistd.h>
#include "vm_app.h"

using namespace std;

int main()
{
    int pid1 = fork();
    if (pid1)
    {
        vm_yield();
        int pid2 = fork();
        vm_yield();
        if (pid2)
        {
            vm_yield();
            int pid3 = fork();
            if (pid3)
            {
                char *vp0 = (char *) vm_map(nullptr, 0);
                char *vp1 = (char *) vm_map(nullptr, 0);
                char *vp2 = (char *) vm_map(nullptr, 0);
                char *vp3 = (char *) vm_map(nullptr, 0);
                char *vp4 = (char *) vm_map(nullptr, 0);
                char *vp5 = (char *) vm_map(nullptr, 0);
                vm_yield();
                *vp0 = 'c'; 
                *vp1 = 'c';
                *vp2 = 'c';
                *vp3 = 'c'; // Now vp0 is evicted to disk. vp1 & vp2's referenced bit is 0.
                cout << *vp1 << endl; // Now vp1's reference bit is 1
                *vp4 = 'c'; // Now vp2 is evicted to disk. vp1's referenced bit is 0.
                cout << *vp0 << endl; // now vp0 is at state 5. vp1 is evicted
                cout << *vp0 << endl; // nothing changes here
                *vp2 = 'c'; // now vp2 is brought to mem and is at state 2. vp3 is evicted and everyone's referenced bit is now 0
                cout << *vp0 << endl; // now vp0 is back to state 5
                *vp0 = 'c'; // vp0 is changed to state 2
                cout << *vp3 << endl; // vp3 is changed to state 5.
                cout << *vp1 << endl; // now vp1 is changed to state 5. Everyone except vp1 has referenced bit of 0 
                *vp2 = 'c'; // now vp2 is changed from state3 to state 2
                *vp0 = 'c'; // vp0 is changed from state 6 to 2. vp3 is evicted. vp2's ref bit is 0.
            vm_yield();
            }
        vm_yield();
        }
    vm_yield();
    }
}