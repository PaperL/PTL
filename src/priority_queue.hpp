#ifndef SJTU_PRIORITY_QUEUE_HPP
#define SJTU_PRIORITY_QUEUE_HPP

#include <cstddef>
#include <functional>
#include "exceptions.hpp"

namespace sjtu {

    template<typename T, class Compare = std::less<T>>
    class priority_queue {
    private:
        size_t elementNum;
        struct lNode {
            T *data;
            lNode *lChild, *rChild;
        } *root;

        Compare cmp;

        void dfsDel(lNode *p) {
            if (p == nullptr)return;
            if (p->lChild != nullptr || p->rChild != nullptr) {
                dfsDel(p->lChild);
                dfsDel(p->rChild);
            } else delete p;
        }

        void dfsCopy(lNode *myself, lNode *right) {
            if (p == nullptr)return;
            if (p->lChild != nullptr || p->rChild != nullptr) {
                dfsDel(p->lChild);
                dfsDel(p->rChild);
            } else delete p;
        }

        lNode *dfsMerge(lNode *H1, lNode *H2) {
            if (H1 == nullptr)return H2;
            if (H2 == nullptr)return H1;
            if (!cmp(H1, H2)) {
                lNode *tempPtr = H1;
                H1 = H2;
                H2 = tempPtr;
            }
            if (H1->lChild == nullptr)H1->lChild = H2;
            else {
                H1->rChild = dfsMerge(H1->rChild, H2);
                lNode *tempPtr = H1->lChild;
                H1->lChild = H1->rChild;
                H1->rChild = tempPtr;
            }
            return H1;
        }

    public:

        priority_queue() : elementNum(0), root(nullptr) {}

        priority_queue(const priority_queue &other) : elementNum(0), root(nullptr) {}

        ~priority_queue() { dfsDel(root); }


        priority_queue &operator=(const priority_queue &other) {}


        const T &top() const {
            if (elementNum == 0)throw container_is_empty();
            return *(root->data);
        }

        void push(const T &arg) {
            lNode *newNode;
            newNode->data = new T(arg);
            newNode->lChild = nullptr;
            newNode->rChild = nullptr;
            root = dfsMerge(newNode, root);
            ++elementNum;
        }

        void pop() {
            if (elementNum == 0)throw container_is_empty();
            lNode *lH = root->lChild, *rH = root->rChild;
            delete root->data;
            delete root;
            root = dfsMerge(lH, rH);
            --elementNum;
        }

        size_t size() const { return elementNum; }

        bool empty() const { return elementNum == 0; }

        void merge(priority_queue &other) {

        }
    };

}

#endif
