#include <iostream>
#include <string>

#include "exceptions.hpp"

#include "vector.hpp"
#include "priority_queue.hpp"
#include "deque.hpp"
#include "segment_tree.hpp"
// #include "map.hpp"

#include "PTF.hpp"

using namespace sjtu;
using namespace PTF;
using namespace PTL;

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

void priority_queueTest() {
    priority_queue<int, std::less<int> > tpq;
    std::cout << tpq.size() << std::endl;
    std::cout << "===cp1===" << std::endl;

    tpq.push(1);
    tpq.push(2);
    std::cout << "===cp2===" << std::endl;

    std::cout << tpq.size() << std::endl;
    std::cout << tpq.empty() << std::endl;
    std::cout << "===cp3===" << std::endl;

    std::cout << tpq.top() << std::endl;
    std::cout << "===cp4===" << std::endl;

    tpq.push(0);
    std::cout << tpq.top() << std::endl;
    tpq.pop();
    std::cout << tpq.top() << std::endl;
    tpq.pop();
    std::cout << tpq.top() << std::endl;
    std::cout << "===cp5===" << std::endl;

    tpq.push(1);
    priority_queue<int, std::less<int> > ttpq1(tpq);
    while (!ttpq1.empty()) {
        std::cout << ttpq1.top();
        ttpq1.pop();
    }
    std::cout << std::endl;

    tpq.push(5);
    ttpq1 = tpq;
    while (!ttpq1.empty()) {
        std::cout << ttpq1.top();
        ttpq1.pop();
    }
    std::cout << std::endl;

    tpq.push(4);
    ttpq1 = tpq;
    while (!ttpq1.empty()) {
        std::cout << ttpq1.top();
        ttpq1.pop();
    }
    std::cout << std::endl;

    tpq.push(3);
    ttpq1 = tpq;
    while (!ttpq1.empty()) {
        std::cout << ttpq1.top();
        ttpq1.pop();
    }
    std::cout << std::endl;
    std::cout << "===cp6===" << std::endl;

    priority_queue<int, std::less<int> > ttpq2, ttpq3;
    for (int i = 0; i < 5; ++i)ttpq2.push(i * 10);
    for (int i = 0; i < 5; ++i)ttpq3.push(i * 13 - 6);
    ttpq2.merge(ttpq3);
    std::cout << "===cp7===" << std::endl;

    std::cout << ttpq2.size() << std::endl;
    while (!ttpq2.empty()) {
        std::cout << ttpq2.top() << " ";
        ttpq2.pop();
    }
    std::cout << std::endl;
    std::cout << "===cp8===" << std::endl;
}

/*
void dequeTest() {
    deque<int, 5, 2> dq;
    for (int i = 0; i < 10; ++i)dq.push_back(i);
    dq.debugPrint();
    auto it = dq.end();
    --it;
    std::cout << *it << std::endl;
    it = dq.erase(it);
    dq.debugPrint();
    std::cout << *it << std::endl;
    it = dq.erase(it);
    dq.debugPrint();
    std::cout << *it << std::endl;
    it = dq.erase(it);
    std::cout << *it << std::endl;
    dq.debugPrint();
    it = dq.erase(it);
    std::cout << *it << std::endl;
    dq.debugPrint();
    it = dq.erase(it);
    std::cout << *it << std::endl;
    dq.debugPrint();
    it = dq.erase(it);
    std::cout << *it << std::endl;
    dq.debugPrint();
    it = dq.erase(it);
    std::cout << *it << std::endl;
    dq.debugPrint();
}
*/

void segment_treeTest() {
    int a[7];
    for (int i = 0; i < 7; ++i)
        a[i] = i + 1;
    //std::cin >> item;
    segment_tree<int> tree(7, a);
    for (int i = 0; i < 7; ++i)
        std::cout << tree.query(i, i + 1) << ", ";
    std::cout << std::endl;
    tree.update(1, -5);
    tree.update(3, 6, 3);
    for (int i = 0; i < 7; ++i)
        std::cout << tree.query(i, i + 1) << ", ";
    std::cout << std::endl;
}

void mapTest() {

}

int main() {
    /*
    printf("\033[1mThis is RED.\n\033[0m");
    printf("\033[2mThis is RED.\n\033[0m");
    printf("\033[3mThis is RED.\n\033[0m");
    printf("\033[4mThis is RED.\n\033[0m");
    printf("\033[5mThis is RED.\n\033[0m");
    printf("\033[6mThis is RED.\n\033[0m");
    printf("\033[7mThis is RED.\n\033[0m");
    printf("\033[8mThis is RED.\n\033[0m");
    printf("\033[9mThis is RED.\n\033[0m");
    printf("\033[31m\033[5AThis is NOT RED.\n\033[0m");
    printf("\033[2J");
    return 0;*/
    /*
    try {
        //vectorTest();
        //priority_queueTest();
        //dequeTest();
        mapTest();
    }
    catch (sjtu::exception xept) {
        std::cout << xept.what() << std::endl;
    }*/
    char a = '=';
    int b = 123;
    unsigned long long int c = 987;
    qWrite<' '>(a, b, c);
    return 0;
}
