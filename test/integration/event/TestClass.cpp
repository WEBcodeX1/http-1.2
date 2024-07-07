#include "TestClass.hpp"

using namespace std;

Callback::Callback(){
    this->registerEvent(
        &Callback::check_function,
        &Callback::callback_function
    );
};

const bool Callback::check_function()
{
    return false;
}

void Callback::callback_function()
{
    cout << "Callback!" << endl;
}
