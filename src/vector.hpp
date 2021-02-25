#ifndef SJTU_VECTOR_HPP
#define SJTU_VECTOR_HPP

#include "exceptions.hpp"

#include <climits>
#include <cstddef>
#include <cstring> // memcpy

#define VECTOR_INITIAL_SIZE 8

namespace sjtu {

    template<typename T>
    class vector {
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

    public:
        class const_iterator;

        class iterator {
        private:
            friend class const_iterator;

            friend class vector<T>;

            vector<T> *subject;
            size_t index;

            /* 安全性是不存在的
            inline void checkUninitialized() {
                if (subject == nullptr) throw invalid_iterator("Try to Operate Uninitialized Iterator");
            }*/

        public:

            iterator() : subject(nullptr), index(0) {}

            iterator(vector<T> *sub, size_t id) : subject(sub), index(id) {}


            iterator operator+(const int &n) const { return iterator(subject, index + n); }

            /*无符号类型与有符号类型变量同时出现在运算中时默认转为无符号
            if ((n > 0 && n >= -index) || (n < 0 && -n > index))// n>0 处判断是否溢出size_t范围
                throw invalid_iterator("Index out of Range of size_t (operator+)");*/

            iterator operator-(const int &n) const { return iterator(subject, index - n); }

            /*if ((n > 0 && n > index) || (n < 0 && -n >= -index))
                throw invalid_iterator("Index out of Range of size_t (operator-)");*/


            int operator-(const iterator &rhs) const {
                if (subject != rhs.subject)throw invalid_iterator();
                    //"Try to Minus Two Iterators of Different Vector"
                else return int(index - rhs.index); // 此处也很危险
            }


            iterator &operator+=(const int &n) {
                index += n;
                return (*this);
            }

            iterator &operator-=(const int &n) {
                index -= n;
                return (*this);
            }


            iterator operator++(int) {
                ++index;
                return (*this);
            }// iter++

            iterator &operator++() {
                ++index;
                return (*this);
            }// ++iter

            iterator operator--(int) {
                --index;
                return (*this);
            }// iter--

            iterator &operator--() {
                --index;
                return (*this);
            }// --iter


            T &operator*() const { return *(subject->data[index]); }// *iter


            bool operator==(const iterator &rhs) const { return (index == rhs.index && subject == rhs.subject); }

            bool operator==(const const_iterator &rhs) const { return (index == rhs.index && subject == rhs.subject); }

            bool operator!=(const iterator &rhs) const { return (index != rhs.index || subject != rhs.subject); }

            bool operator!=(const const_iterator &rhs) const { return (index != rhs.index || subject != rhs.subject); }
        };

        class const_iterator {
        private:
            friend class iterator;

            friend class vector<T>;

            const vector<T> *subject;
            size_t index;

        public:

            const_iterator() : subject(nullptr), index(0) {}

            const_iterator(const vector<T> *sub, size_t id) : subject(sub), index(id) {}


            const_iterator operator+(const int &n) const { return iterator(subject, index + n); }

            const_iterator operator-(const int &n) const { return iterator(subject, index - n); }


            int operator-(const const_iterator &rhs) const {
                if (subject != rhs.subject) throw invalid_iterator();
                    //"Try to Minus Two Iterators of Different Vector (const_iterator)"
                else return (index - rhs.index);
            }


            const_iterator &operator+=(const int &n) {
                index += n;
                return (*this);
            }

            const_iterator &operator-=(const int &n) {
                index -= n;
                return (*this);
            }


            const_iterator operator++(int) {
                ++index;
                return (*this);
            }// iter++

            const_iterator &operator++() {
                ++index;
                return (*this);
            }// ++iter

            const_iterator operator--(int) {
                --index;
                return (*this);
            }// iter--

            const_iterator &operator--() {
                --index;
                return (*this);
            }// --iter


            const T &operator*() const { return *(subject->data[index]); }// *iter

            bool operator==(const iterator &rhs) const { return (index == rhs.index && subject == rhs.subject); }

            bool operator==(const const_iterator &rhs) const { return (index == rhs.index && subject == rhs.subject); }

            bool operator!=(const iterator &rhs) const { return (index != rhs.index || subject != rhs.subject); }

            bool operator!=(const const_iterator &rhs) const { return (index != rhs.index || subject != rhs.subject); }
        };


        vector() : data(nullptr), elementNum(0), memorySize(VECTOR_INITIAL_SIZE) { initMem(); }

        vector(const vector &other) : elementNum(other.elementNum), memorySize(other.memorySize) {
            initMem();
            for (size_t i = 0; i < elementNum; ++i)data[i] = new T(*(other.data[i]));
            // 如果直接 operator new() 返回类型为 void*
        }

        ~vector() { delMem(); }


        vector &operator=(const vector &other) {
            if (this == &other)return *this;
            delMem();
            elementNum = other.elementNum;
            memorySize = other.memorySize;
            initMem();
            for (size_t i = 0; i < elementNum; ++i)data[i] = new T(*(other.data[i]));
        }


        T &at(const size_t &pos) {
            if (pos >= elementNum)throw index_out_of_bound();
                //"Try to Get Vector Element Out of Range (at)"
            else return *(data[pos]);
        }

        const T &at(const size_t &pos) const {
            if (pos >= elementNum)throw index_out_of_bound();
                // "Try to Get Vector Element Out of Range (const at)"
            else return *(data[pos]);
        }

        T &operator[](const size_t &pos) {
            if (pos >= elementNum)throw index_out_of_bound();
                // "Try to Get Vector Element Out of Range (operator[])"
            else return *(data[pos]);
        }

        const T &operator[](const size_t &pos) const {
            if (pos >= elementNum)throw index_out_of_bound();
                //"Try to Get Vector Element Out of Range (const operator[])"
            else return *(data[pos]);
        }


        const T &front() const {
            if (elementNum == 0)throw container_is_empty();
                //"Try to Get Empty Vector's Front Element"
            else return *(data[0]);
        }

        const T &back() const {
            if (elementNum == 0)throw container_is_empty();
                //"Try to Get Empty Vector's Back Element"
            else return *(data[elementNum - 1]);
        }


        iterator begin() { return iterator(this, 0); }

        const_iterator cbegin() const { return const_iterator(this, 0); }

        iterator end() { return iterator(this, elementNum); }

        const_iterator cend() const { return const_iterator(this, elementNum); }


        bool empty() const { return (elementNum == 0); }

        size_t size() const { return elementNum; }

        void clear() {
            delMem();
            elementNum = 0;
            memorySize = VECTOR_INITIAL_SIZE;
            initMem();
        }


        iterator insert(iterator pos, const T &value) { return insert(pos.index, value); }

        iterator insert(const size_t &id, const T &value) {
            if (id > elementNum)throw index_out_of_bound();
            //"Try to Insert Element Out of Range of Vector"
            if (elementNum == memorySize) {
                T **originalData = data;
                memorySize <<= 1;
                initMem();
                memcpy(data, originalData, sizePtr * elementNum);
                delete[] originalData;
            }
            for (size_t i = elementNum; i > id; --i)data[i] = data[i - 1];
            data[id] = new T(value);
            ++elementNum;
            return iterator(this, id);
        }

        iterator erase(iterator pos) { return erase(pos.index); }

        iterator erase(const size_t &id) {
            if (id >= elementNum)throw index_out_of_bound();
            //"Try to Erase Element Out of Range of Vector"
            delete data[id];
            for (size_t i = id; i < elementNum - 1; ++i)data[i] = data[i + 1];
            --elementNum;
            return iterator(this, id);
        }


        void push_back(const T &value) {
            if (elementNum == memorySize) {
                T **originalData = data;
                memorySize <<= 1;
                initMem();
                memcpy(data, originalData, sizePtr * elementNum);
                delete[] originalData;
            }
            data[elementNum++] = new T(value);
        }


        void pop_back() {
            if (elementNum == 0)throw container_is_empty();
                //"Try to Pop Back Element from Empty Vector"
            else {
                delete data[--elementNum];
                data[elementNum] = nullptr;
            }
        }
    };

}

#endif