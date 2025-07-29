#ifndef Vector_hpp
#define Vector_hpp

#include <vector>
#include <iterator>

#include "Debug.cpp"


using namespace std;

//- overloaded vector class providing multi element (non-continous) erase functionality
template <class T>
class Vector: public vector<T> {

typedef const vector<uint> EraseElements_t;


public:

    Vector() {
    }

    ~Vector() {
    }

    void erase(EraseElements_t& EraseElements) {
        uint erase_count = 0;
        for (auto it = EraseElements.begin(); it != EraseElements.end(); ++it) {
            DBG(120, "Erase Element at index:" << *it);
            if (this->begin()+*it <= this->end()) {
                vector<T>::erase(this->begin()-erase_count+*it);
                ++erase_count;
            }
        }
    }

};

#endif
