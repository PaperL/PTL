#include <iostream>

#include "exceptions.hpp"
#include "vector.hpp"
#include <string>

using namespace sjtu;

void vectorTest() {

    vector<int> a;
    std::cout << a.size() << std::endl;
    std::cout << "===cp1===" << std::endl;

    a.insert(0, 1);
    a.insert(0, 2);
    a.insert(1, 3);
    std::cout << "===cp2===" << std::endl;

    std::cout << (a.front()) << std::endl;
    std::cout << (*(a.begin())) << std::endl;
    std::cout << (a.back()) << std::endl;
    std::cout << (*(a.end() - 1)) << std::endl;
    std::cout << "===cp3===" << std::endl;

    std::cout << (a[0]) << std::endl;
    std::cout << (a[1]) << std::endl;
    std::cout << (a[2]) << std::endl;
    std::cout << "===cp4===" << std::endl;

    a.erase(1);
    std::cout << (a.size()) << std::endl;
    std::cout << (a.front()) << std::endl;
    std::cout << (a.back()) << std::endl;
    a.erase(1);
    std::cout << (a.size()) << std::endl;
    std::cout << (a.front()) << std::endl;
    std::cout << (a.back()) << std::endl;
    std::cout << "===cp5===" << std::endl;

    std::cout << (a.size()) << std::endl;
    for (int i = 1; i <= 100; ++i)
        a.push_back(i * 5);
    std::cout << "===cp6===" << std::endl;

    std::cout << (a.size()) << std::endl;
    std::cout << (a.front()) << std::endl;
    std::cout << (a.back()) << std::endl;
    std::cout << *(a.begin()) << std::endl;
    std::cout << *(a.end() - 1) << std::endl;
    std::cout << "===cp7===" << std::endl;

    std::cout << "size:" << a.size() << std::endl;
    for (auto i:a) {
        if (i >= 30 && i <= 470)continue;
        else if (i == 475)std::cout << "..." << std::endl;
        std::cout << i << std::endl;
    }
    std::cout << "===cp8===" << std::endl;

    a.pop_back();
    a.pop_back();
    a.erase(97);
    a.erase(3);
    a.insert(a.begin() + 1, -1);
    a.insert(a.end() - 2, -2);
    std::cout << "size:" << a.size() << std::endl;
    for (auto i:a) {
        if (i >= 30 && i <= 470)continue;
        else if (i == 475)std::cout << "..." << std::endl;
        std::cout << i << std::endl;
    }
    std::cout << "===cp9===" << std::endl;

    auto p = a.cbegin();
    vector<int>::iterator p2(a.begin());
    std::cout << (p == p2) << std::endl;
    std::cout << "===cp10===" << std::endl;
}

int main() {
    try {
        vectorTest();
    }
    catch (sjtu::exception xept) {
        std::cout << xept.what() << std::endl;
    }
    return 0;
}
