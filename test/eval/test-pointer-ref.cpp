#include <cassert>
#include <string>
#include <iostream>

using namespace std;

void testfunc_int(int* testvar) {
    *testvar += 1;
}

void testfunc_void_pointer(void* AddressP) {
    *static_cast<char*>(AddressP) += 1;
}

int main()
{
    int a = 1;
    testfunc_int(&a);
    cout << a << endl;

    void* TestAddress = 0;
    cout << TestAddress << endl;
    testfunc_void_pointer(&TestAddress);
    cout << TestAddress << endl;
}
