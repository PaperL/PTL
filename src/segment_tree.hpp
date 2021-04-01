//
// Created by PaperL on 2021/4/1.
//

#ifndef PTL_SEGMENT_TREE_H
#define PTL_SEGMENT_TREE_H

#include "exceptions.hpp"
#include <cstring> // memcpy

namespace PTL {

    template<typename T>
    class segment_tree {
    private:
        const size_t sizePtr = sizeof(T *);
        T **data;
        size_t elementNum, memorySize; // 此处 memroySize 单位为 sizeof(T)

        inline void initMem() {
            data = new T *[memorySize];
            memset(data, 0, sizePtr * memorySize);
        }

        inline void delMem() {
            for (size_t i = 0; i < elementNum; ++i)
                delete data[i];
            delete[] data;
        }

        inline void pushUp(const size_t &p) { *data[p] = *data[p << 1] + *data[p << 1 | 1]; }

        void buildTree(size_t p, size_t l, size_t r, T originData[]) {
            if (r - l == 1) *data[p] = originData[l];
            else {
                size_t mid = (l + r) >> 1;
                buildTree(p << 1, l, mid);
                buildTree(p << 1 | 1, mid, r);
                pushUp(p);
            }
        }

        void update(size_t p, size_t l, size_t r, size_t t, const T &k) {
            if (r - l == 1)*data[p] = *data[p] + k;
            else {
                size_t mid = (l + r) >> 1;
                if (t < mid) update(p << 1, l, mid, t, k);
                else update(p << 1 | 1, mid, r, t, k);
                pushUp(p);
            }
        }

    public:

        segment_tree(size_t elementN) : data(nullptr), elementNum(elementN), memorySize(elementN << 2) {
            initMem();
        }

        segment_tree(size_t elementN, T originData[]) : data(nullptr), elementNum(elementN), memorySize(elementN << 2) {
            initMem();
            buildTree(1, 0, elementN, originData);
        }

        segment_tree(const segment_tree &other) : elementNum(other.elementNum), memorySize(other.memorySize) {
            initMem();
            for (size_t i = 0; i < elementNum; ++i)data[i] = new T(*(other.data[i]));
        }

        ~segment_tree() { delMem(); }


        segment_tree &operator=(const segment_tree &other) {
            if (this == &other)return *this;
            delMem();
            elementNum = other.elementNum;
            memorySize = other.memorySize;
            initMem();
            for (size_t i = 0; i < elementNum; ++i)data[i] = new T(*(other.data[i]));
            return *this;
        }

        T &operator[](const size_t &pos) {
            if (pos >= elementNum)throw sjtu::index_out_of_bound();
            else return *(data[pos]);
        }

        const T &operator[](const size_t &pos) const {
            if (pos >= elementNum)throw sjtu::index_out_of_bound();
            else return *(data[pos]);
        }

        size_t size() const { return elementNum; }

        void clear() {
            delMem();
            elementNum = 0;
            memorySize = 0;
            initMem();
        }

    };
}


#endif //PTL_SEGMENT_TREE_H
