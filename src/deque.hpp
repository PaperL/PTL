#ifndef SJTU_DEQUE_HPP
#define SJTU_DEQUE_HPP

#include "exceptions.hpp"

#include <cstddef>
#include <cstring> // memset

namespace sjtu {

    template<class T, size_t BLOCK_ELEMENT_NUMBER = 128, size_t BLOCK_MERGE_BOUND = 32>
    class deque {
    private:
        size_t totElementNumber;

        // Unrolled Linked List Node
        class ULLBlock {
            friend class deque;

        private:
            const size_t sizePtr = sizeof(T *);

            ULLBlock *preBlock, *nxtBlock;
            size_t elementNum;
            T **elementData;

            void splitBlock() {
                const size_t leftNum = BLOCK_ELEMENT_NUMBER / 2;
                const size_t nxtNum = elementNum - BLOCK_ELEMENT_NUMBER / 2;
                ULLBlock *newBlock = new ULLBlock;

                newBlock->preBlock = this;
                newBlock->nxtBlock = nxtBlock;
                newBlock->elementNum = nxtNum;
                for (size_t i = leftNum; i < elementNum; ++i)
                    newBlock->elementData[i - leftNum] = elementData[i];

                if (nxtBlock != nullptr) nxtBlock->preBlock = newBlock;
                nxtBlock = newBlock;

                memset(elementData[leftNum], 0, sizePtr * nxtNum);
                elementNum = leftNum;
            }

            void mergeBlock() {
                for (size_t i = 0; i < nxtBlock->elementNum; ++i)
                    elementData[elementNum + i] = nxtBlock->elementData[i];
                elementNum += nxtBlock->elementNum;

                nxtBlock = nxtBlock->nxtBlock;
                if (nxtBlock != nullptr) {
                    delete nxtBlock->preBlock;
                    nxtBlock->preBlock = this;
                }
            }

        public:
            ULLBlock() : preBlock(nullptr), nxtBlock(nullptr), elementNum(0) {
                elementData = new T *[BLOCK_ELEMENT_NUMBER];
                memset(elementData, 0, sizePtr * BLOCK_ELEMENT_NUMBER);
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
                memset(elementData, 0, sizePtr * BLOCK_ELEMENT_NUMBER);
                for (size_t i = 0; i < elementNum; ++i)
                    elementData[i] = new T(*(other.elementData[i]));
            }

            T *getElementPtr(size_t id) {
                if (id >= elementNum) {
                    if (nxtBlock == nullptr)throw index_out_of_bound();
                    else return nxtBlock->getElementPtr(id - elementNum);
                } else return elementData[id];
            }


            void insertElement(const T &arg, size_t id) {
                if (elementNum == BLOCK_ELEMENT_NUMBER)splitBlock();
                if (id > elementNum) {
                    if (nxtBlock == nullptr)throw index_out_of_bound();
                    else nxtBlock->insertElement(arg, id - elementNum);
                } else {
                    for (size_t i = elementNum; i > id; --i)elementData[i] = elementData[i - 1];
                    elementData[id] = new T(arg);
                }
            }

            void deleteElement(const T &arg, size_t id) {
                if (id >= elementNum) {
                    if (nxtBlock == nullptr)throw index_out_of_bound();
                    else nxtBlock->deleteElement(arg, id - elementNum);
                } else {
                    delete (elementData + id);
                    for (size_t i = id; i < elementNum - 1; ++i)elementData[i] = elementData[i + 1];
                    elementData[--elementNum] = nullptr;
                    if (nxtBlock != nullptr && (elementNum + nxtBlock->elementNum < BLOCK_MERGE_BOUND))
                        mergeBlock();
                }
            }
        };

        ULLBlock *headBlock, *tailBlock;

    public:
        class const_iterator;

        class iterator {
        private:
            deque<T, BLOCK_ELEMENT_NUMBER, BLOCK_MERGE_BOUND> *subject;
            size_t index;
        public:
            /**
             * return a new iterator which pointer n-next elements
             *   if there are not enough elements, iterator becomes invalid
             * as well as operator-
             */
            iterator() : subject(nullptr), index(0) {}

            explicit iterator(deque<T, BLOCK_ELEMENT_NUMBER, BLOCK_MERGE_BOUND> *sub, const size_t &id) :
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


        deque() : totElementNumber(0), headBlock(nullptr), tailBlock(nullptr) {}

        deque(const deque &other) {
            if (other.headBlock != nullptr) {
                headBlock = new ULLBlock(*(other.headBlock));
                ULLBlock *thisPtr = headBlock, *otherPtr = other.headBlock;
                while (otherPtr->nxtBlock != nullptr) {
                    //todo finish here
                }
            } else {
                totElementNumber = 0;
                headBlock = nullptr;
                tailBlock = nullptr;
            }
        }

        ~deque() { clear(); }


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
        void clear() {
            ULLBlock *blockPtr = headBlock;
            while (blockPtr->nxtBlock != nullptr) {
                blockPtr = blockPtr->nxtBlock;
                delete blockPtr->preBlock;
            }
            delete blockPtr;

            totElementNumber = 0;
            headBlock = nullptr;
            tailBlock = nullptr;
        }

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
