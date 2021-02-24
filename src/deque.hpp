#ifndef SJTU_DEQUE_HPP
#define SJTU_DEQUE_HPP

#include "exceptions.hpp"

#include <cstddef>
#include <cstring> // memset

namespace sjtu {

    template<class T, size_t BlockElementNumber = 128, size_t MergeBoundary = 32>
    class deque {
    private:
        size_t memorySize, mergeBoundary;

        // Unrolled Linked List Node
        class ULLBlock {
        public:
            ULLBlock *preBlock, *nxtBlock;
            size_t elementNum;
            T **elementData;

            ULLBlock() : preBlock(nullptr), nxtBlock(nullptr), elementNum(0) {
                elementData = new T *[BlockElementNumber];
                memset(elementData, 0, sizeof(T *) * BlockElementNumber);
            }

            ULLBlock(const ULLBlock &other) : ULLBlock() {
                elementNum = other.elementNum;
                for (size_t i = 0; i < elementNum; ++i)
                    elementData[i] = new T(*(other.elementData[i]));
            }

            ~ULLBlock() {
                for (size_t i = 0; i < elementNum; ++i)
                    delete elementData[i];
                delete elementData;
            }

            ULLBlock &operator=(const ULLBlock &other) {
                for (size_t i = 0; i < elementNum; ++i)
                    delete elementData[i];
                preBlock = nullptr;
                nxtBlock = nullptr;
                elementNum = other.elementNum;
                memset(elementData, 0, sizeof(T *) * BlockElementNumber);
                for (size_t i = 0; i < elementNum; ++i)
                    elementData[i] = new T(*(other.elementData[i]));
            }

            T *blockHeadElement() { return elementData[0]; }

            void insertElement(const T &arg) {}

            void deleteElement(const T &arg) {}
        };

        ULLBlock *headBlock, *tailBlock;

    public:
        class const_iterator;

        class iterator {
        private:
            deque<T, BlockElementNumber, MergeBoundary> *subject;
            size_t index;
        public:
            /**
             * return a new iterator which pointer n-next elements
             *   if there are not enough elements, iterator becomes invalid
             * as well as operator-
             */
            iterator() : subject(nullptr), index(0) {}

            explicit iterator(deque<T, BlockElementNumber, MergeBoundary> *sub, const size_t &id) :
                    subject(sub), index(id) {}


            iterator operator+(const int &n) const { return iterator(subject, index + n); }

            iterator operator-(const int &n) const { return iterator(subject, index - n); }

            int operator-(const iterator &rhs) const {
                if (subject != rhs.subject)throw invalid_iterator();
                return int(index - rhs.index);
            }

            iterator &operator+=(const int &n) {
                index += n;
                return *this;
            }

            iterator &operator-=(const int &n) {
                index -= n;
                return *this;
            }

            iterator operator++(int) {
                iterator tempIt(*this);
                ++index;
                return tempIt;
            }// iter++

            iterator &operator++() {
                ++index;
                return *this;
            }// ++iter

            iterator operator--(int) {
                iterator tempIt(*this);
                --index;
                return tempIt;
            }// iter--

            iterator &operator--() {
                --index;
                return *this;
            }// --iter

            /**
             * TODO *it
             * 		throw if iterator is invalid
             */
            T &operator*() const {}

            /**
             * TODO it->field
             * 		throw if iterator is invalid
             */
            T *operator->() const noexcept { return &(operator*()); }

            /**
             * a operator to check whether two iterators are same (pointing to the same memory).
             */
            bool operator==(const iterator &rhs) const {

            }

            bool operator==(const const_iterator &rhs) const {}

            /**
             * some other operator for iterator.
             */
            bool operator!=(const iterator &rhs) const {}

            bool operator!=(const const_iterator &rhs) const {}
        };

        class const_iterator {
            // it should has similar member method as iterator.
            //  and it should be able to construct from an iterator.
        private:
            // data members.
        public:
            const_iterator() {
                // TODO
            }

            const_iterator(const const_iterator &other) {
                // TODO
            }

            const_iterator(const iterator &other) {
                // TODO
            }
            // And other methods in iterator.
            // And other methods in iterator.
            // And other methods in iterator.
        };


        deque() : headBlock(nullptr), tailBlock(nullptr) {}

        deque(const deque &other) {
            clear();
            if (other.headBlock != nullptr) {
                ULLBlock *otherBlock = other.headBlock;
                ULLBlock *newBlock = new ULLBlock;
                headBlock = newBlock;
                while (true) {
                    *newBlock = *otherBlock;

                }
            }
        }

        ~deque() {}


        deque &operator=(const deque &other) {}

        /**
         * access specified element with bounds checking
         * throw index_out_of_bound if out of bound.
         */
        T &at(const size_t &pos) {}

        const T &at(const size_t &pos) const {}

        T &operator[](const size_t &pos) {}

        const T &operator[](const size_t &pos) const {}

        /**
         * access the first element
         * throw container_is_empty when the container is empty.
         */
        const T &front() const {}

        /**
         * access the last element
         * throw container_is_empty when the container is empty.
         */
        const T &back() const {}

        /**
         * returns an iterator to the beginning.
         */
        iterator begin() {}

        const_iterator cbegin() const {}

        /**
         * returns an iterator to the end.
         */
        iterator end() {}

        const_iterator cend() const {}

        /**
         * checks whether the container is empty.
         */
        bool empty() const {}

        /**
         * returns the number of elements
         */
        size_t size() const {}

        /**
         * clears the contents
         */
        void clear() {}

        /**
         * inserts elements at the specified locat on in the container.
         * inserts value before pos
         * returns an iterator pointing to the inserted value
         *     throw if the iterator is invalid or it point to a wrong place.
         */
        iterator insert(iterator pos, const T &value) {}

        /**
         * removes specified element at pos.
         * removes the element at pos.
         * returns an iterator pointing to the following element, if pos pointing to the last element, end() will be returned.
         * throw if the container is empty, the iterator is invalid or it points to a wrong place.
         */
        iterator erase(iterator pos) {}

        /**
         * adds an element to the end
         */
        void push_back(const T &value) {}

        /**
         * removes the last element
         *     throw when the container is empty.
         */
        void pop_back() {}

        /**
         * inserts an element to the beginning.
         */
        void push_front(const T &value) {}

        /**
         * removes the first element.
         *     throw when the container is empty.
         */
        void pop_front() {}
    };

}

#endif
