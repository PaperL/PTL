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

        class lNode {
        public:
            T *data;
            lNode *lChild, *rChild;

            lNode() : data(nullptr), lChild(nullptr), rChild(nullptr) {}

            explicit lNode(const T &arg) : lChild(nullptr), rChild(nullptr) { data = new T(arg); }

            lNode(const lNode &other) : lChild(nullptr), rChild(nullptr) { data = new T(*(other.data)); }

            ~lNode() {
                delete data;
                delete lChild;
                delete rChild;
            }
        } *root;

        Compare cmp;

        void dfsCopy(lNode *myself, lNode *other) {
            // 本函数也可由 lNode::operator= 实现
            if (other->data != nullptr)
                myself->data = new T(*(other->data));
            if (other->lChild != nullptr) {
                myself->lChild = new lNode;
                dfsCopy(myself->lChild, other->lChild);
            }
            if (other->rChild != nullptr) {
                myself->rChild = new lNode;
                dfsCopy(myself->rChild, other->rChild);
            }
        }

        lNode *dfsMerge(lNode *H1, lNode *H2) {
            if (H1 == nullptr)return H2;
            if (H2 == nullptr)return H1;
            if (cmp(*(H1->data), *(H2->data))) {
                lNode *tempPtr = H1;
                H1 = H2;
                H2 = tempPtr;
            }
            if (H1->lChild == nullptr)H1->lChild = H2;
            else {
                H1->rChild = dfsMerge(H1->rChild, H2);
                // 斜堆
                lNode *tempPtr = H1->lChild;
                H1->lChild = H1->rChild;
                H1->rChild = tempPtr;
            }
            return H1;
        }

    public:

        priority_queue() : elementNum(0), root(nullptr) {}

        priority_queue(const priority_queue &other) : elementNum(0), root(nullptr) {
            if (other.root == nullptr) root = nullptr;
            else {
                root = new lNode;
                dfsCopy(root, other.root);
            }
            elementNum = other.elementNum;
        }

        ~priority_queue() { delete root; }


        priority_queue &operator=(const priority_queue &other) {
            if (this == &other)return *this;
            delete root;
            if (other.root == nullptr) root = nullptr;
            else {
                root = new lNode;
                dfsCopy(root, other.root);
            }
            elementNum = other.elementNum;
            return *this;
        }


        const T &top() const {
            if (elementNum == 0)throw container_is_empty();
            return *(root->data);
        }

        void push(const T &arg) {
            lNode *newNode = new lNode;
            newNode->data = new T(arg);
            newNode->lChild = nullptr;
            newNode->rChild = nullptr;
            root = dfsMerge(newNode, root);
            ++elementNum;
        }

        void pop() {
            if (elementNum == 0)throw container_is_empty();
            lNode *lH = root->lChild, *rH = root->rChild;
            root->lChild = root->rChild = nullptr;// 防止递归删除整棵树
            delete root;
            root = dfsMerge(lH, rH);
            --elementNum;
        }

        size_t size() const { return elementNum; }

        bool empty() const { return (elementNum == 0); }

        void merge(priority_queue &other) {
            if (this == &other)return;
            lNode *tempHeap;
            if (other.root == nullptr) tempHeap = nullptr;
            else {
                tempHeap = new lNode;
                dfsCopy(tempHeap, other.root);
            }
            root = dfsMerge(root, tempHeap);
            elementNum += other.elementNum;

            delete other.root;
            other.root = nullptr;
            other.elementNum = 0;
        }
    };

}

#endif