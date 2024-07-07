#define BOOST_TEST_MAIN
#include <boost/test/unit_test.hpp>

#include "../../../Vector.hpp"


BOOST_AUTO_TEST_CASE( test_vector_erase )
{
    cout << "Check Vector.erase(const vector<uint>&)" << endl;

    Vector<uint> v1;

    uint ints1[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13 };
    v1.assign(ints1, ints1+13);

    vector<uint> v2;

    uint ints2[] = { 1, 3, 7, 12 };
    v2.assign(ints2, ints2+4);

    v1.erase(v2);

    uint* ints_compare1 = v1.data();
    uint ints_compare2[] = { 1, 3, 5, 6, 7, 9, 10, 11, 12 };

    for (uint i=0; i<9; ++i) {
        const uint c1 = ints_compare1[i];
        const uint c2 = ints_compare2[i];
        cout << "Comp1:" << c1 << " Comp2:" << c2 << endl;
        BOOST_CHECK(c1 == c2);
    }

}
