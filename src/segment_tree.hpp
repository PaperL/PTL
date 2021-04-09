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
        T **data, **lazyTag;
        size_t elementNum, memorySize; // 此处 memroySize 单位为 sizeof(T)

        inline void initMem() {
            data = new T *[memorySize];
            memset(data, 0, sizePtr * memorySize);
            lazyTag = new T *[memorySize];
            memset(lazyTag, 0, sizePtr * memorySize);
        }

        inline void delMem() {
            for (size_t i = 0; i < elementNum; ++i) {
                delete data[i];
                delete lazyTag[i];
            }
            delete[] data;
            delete[] lazyTag;
        }


        inline void pushUp(const size_t &p) {
            if (data[p]) *data[p] = *data[p << 1] + *data[p << 1 | 1];
            else data[p] = new T(*data[p << 1] + *data[p << 1 | 1]);
        }

        inline void tag(const size_t &p, const size_t &l, const size_t &r, const T &k) {
            if (data[p]) *data[p] = *data[p] + (r - l) * k;
            else data[p] = new T((r - l) * k);
            if (lazyTag[p]) *lazyTag[p] = *lazyTag[p] + k;
            else lazyTag[p] = new T(k);
        }

        inline void pushDown(const size_t &p, const size_t &l, const size_t &r) {
            if (lazyTag[p]) {
                size_t mid = (l + r) >> 1;
                tag(p << 1, l, mid, *lazyTag[p]);
                tag(p << 1 | 1, mid, r, *lazyTag[p]);
                delete lazyTag[p];
                lazyTag[p] = nullptr;
            }
        }

        void buildTree(size_t p, size_t l, size_t r, const T &initT) {
            if (r - l == 1) data[p] = new T(initT);
            else {
                size_t mid = (l + r) >> 1;
                buildTree(p << 1, l, mid, initT);
                buildTree(p << 1 | 1, mid, r, initT);
                pushUp(p);
            }
        }

        void buildTree(size_t p, size_t l, size_t r, T originData[]) {
            if (r - l == 1) data[p] = new T(originData[l]);
            else {
                size_t mid = (l + r) >> 1;
                buildTree(p << 1, l, mid, originData);
                buildTree(p << 1 | 1, mid, r, originData);
                pushUp(p);
            }
        }

        void _update(size_t p, size_t l, size_t r, const size_t &t, const T &k) {
            if (r - l == 1) {
                if (data[p])*data[p] = *data[p] + k;
                else data[p] = new T(*data[p] + k);
            } else {
                pushDown(p, l, r);
                size_t mid = (l + r) >> 1;
                if (t < mid) _update(p << 1, l, mid, t, k);
                else _update(p << 1 | 1, mid, r, t, k);
                pushUp(p);
            }
        }

        void _update(size_t p, size_t l, size_t r, const size_t &tl, const size_t &tr, const T &k) {
            if (tl <= l && tr >= r) tag(p, l, r, k);
            else if (r - l > 1) {
                pushDown(p, l, r);
                size_t mid = (l + r) >> 1;
                if (tl <= mid) _update(p << 1, l, mid, tl, tr, k);
                if (tr >= mid) _update(p << 1 | 1, mid, r, tl, tr, k);
                pushUp(p);
            }
        }

        T _query(size_t p, size_t l, size_t r, const size_t &tl, const size_t &tr) {
            if (r - l > 1)pushDown(p, l, r);
            if (tl <= l && tr >= r)return *data[p]; // 此处若无初始化建树导致解引用 nullptr, 为未定义行为
            size_t mid = (l + r) >> 1;
            if (tr <= mid) return _query(p << 1, l, mid, tl, tr);
            if (tl >= mid) return _query(p << 1 | 1, mid, r, tl, tr);
            return _query(p << 1, l, mid, tl, tr) + _query(p << 1 | 1, mid, r, tl, tr);
        }

    public:

        explicit segment_tree(size_t elementN, T initT)
                : data(nullptr), elementNum(elementN), memorySize(elementN << 2) {
            initMem();
            buildTree(1, 0, elementN, initT);
        }

        explicit segment_tree(size_t elementN, T originData[])
                : data(nullptr), elementNum(elementN), memorySize(elementN << 2) {
            initMem();
            buildTree(1, 0, elementN, originData);
        }

        segment_tree(const segment_tree &other) : elementNum(other.elementNum), memorySize(other.memorySize) {
            initMem();
            for (size_t i = 0; i < memorySize; ++i) {
                data[i] = (other.data[i]) ? (new T(*other.data[i])) : (nullptr);
                lazyTag[i] = (other.lazyTag[i]) ? (new T(*other.lazyTag[i])) : (nullptr);
            }
        }

        ~segment_tree() { delMem(); }


        segment_tree &operator=(const segment_tree &other) {
            if (this == &other)return *this;
            delMem();
            elementNum = other.elementNum;
            memorySize = other.memorySize;
            initMem();
            for (size_t i = 0; i < memorySize; ++i) {
                data[i] = (other.data[i]) ? (new T(*other.data[i])) : (nullptr);
                lazyTag[i] = (other.lazyTag[i]) ? (new T(*other.lazyTag[i])) : (nullptr);
            }
            return *this;
        }

        size_t size() const { return elementNum; }

        void clear() {
            delMem();
            elementNum = 0;
            memorySize = 0;
            data = nullptr;
            lazyTag = nullptr;
        }


        void update(const size_t &t, const T &k) { _update(1, 0, elementNum, t, k); }

        void update(const size_t &l, const size_t &r, const T &k) { _update(1, 0, elementNum, l, r, k); }

        T query(const size_t &l, const size_t &r) { return _query(1, 0, elementNum, l, r); }

    };
}


#endif //PTL_SEGMENT_TREE_H