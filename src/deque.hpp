#ifndef SJTU_DEQUE_HPP
#define SJTU_DEQUE_HPP

#include "exceptions.hpp"

#include <cstddef>
#include <cstring> // memset

#define PAPERL_DEQUE_DEBUG


#ifdef PAPERL_DEQUE_DEBUG

#include <iostream>

#endif

namespace sjtu {

    template<class T, size_t BLOCK_ELEMENT_NUMBER = 128, size_t BLOCK_MERGE_BOUND = 32>
    class deque {
    public:
        class iterator;

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
                delete[] elementData;
            }

            ULLBlock &operator=(const ULLBlock &other) {
                if (this == &other)return *this;
                for (size_t i = 0; i < elementNum; ++i)
                    delete elementData[i];
                memset(elementData, 0, sizePtr * BLOCK_ELEMENT_NUMBER);
                preBlock = nullptr;
                nxtBlock = nullptr;
                elementNum = other.elementNum;
                for (size_t i = 0; i < elementNum; ++i)
                    elementData[i] = new T(*(other.elementData[i]));
            }

            T *getElementPtr(size_t id) {
                if (id >= elementNum) {
                    if (nxtBlock == nullptr)throw index_out_of_bound();
                    else return nxtBlock->getElementPtr(id - elementNum);
                } else return elementData[id];
            }

            T *getFrontElementPtr() {
                if (elementNum == 0)throw runtime_error();
                return elementData[0];
            }

            T *getBackElementPtr() {
                if (elementNum == 0)throw runtime_error();
                return elementData[elementNum - 1];
            }


            iterator insertElement(const T &arg, size_t id) {
                /*if (id > elementNum) {
                    if (nxtBlock == nullptr)throw index_out_of_bound();
                    else nxtBlock->insertElement(arg, id - elementNum);
                } else {*/
                if (id > elementNum)throw index_out_of_bound();
                // todo 需要真正实现迭代器的 invalid 功能
                if (elementNum == BLOCK_ELEMENT_NUMBER) {
                    splitBlock();
                    if (id > elementNum)return insertElement(arg, id - elementNum);
                }
                for (size_t i = elementNum; i > id; --i)elementData[i] = elementData[i - 1];
                elementData[id] = new T(arg);
                return iterator(this, id);
            }

            iterator deleteElement(const T &arg, size_t id) {
                /*if (id >= elementNum) {
                    if (nxtBlock == nullptr)throw index_out_of_bound();
                    else nxtBlock->deleteElement(arg, id - elementNum);
                } else {*/
                if (id >= elementNum)throw index_out_of_bound();
                delete (elementData + id);
                --elementNum;
                for (size_t i = id; i < elementNum; ++i)elementData[i] = elementData[i + 1];
                elementData[elementNum] = nullptr;
                if (nxtBlock != nullptr && (elementNum + nxtBlock->elementNum < BLOCK_MERGE_BOUND))
                    mergeBlock();
                return iterator(this, id);
            }
        };

        ULLBlock *headBlock, *tailBlock;

    public:
        class const_iterator;

        class iterator {
            friend class deque;

        private:
            deque<T, BLOCK_ELEMENT_NUMBER, BLOCK_MERGE_BOUND> *subject;
            size_t indexInDeque;
            ULLBlock *blockPtr;
            size_t indexInBlock;
        public:
            iterator() : subject(nullptr), blockPtr(subject->headBlock), indexInDeque(0), indexInBlock(0) {}

            explicit iterator(deque<T, BLOCK_ELEMENT_NUMBER, BLOCK_MERGE_BOUND> *sub,
                              const size_t &id, bool tailConstruct = false) :
                    subject(sub), blockPtr(subject->headBlock), indexInDeque(id), indexInBlock(id) {
                if (tailConstruct) {
                    indexInDeque = subject->totElementNumber;
                    blockPtr = subject->tailBlock;
                    indexInBlock = blockPtr->elementNum;
                } else {
                    while (blockPtr != nullptr && indexInBlock >= blockPtr->elementNum) {
                        indexInBlock -= blockPtr->elementNum;
                        blockPtr = blockPtr->nxtBlock;
                    }
                }
            }

            explicit iterator(const iterator &it, int offset) :
                    subject(it.subject), blockPtr(it.blockPtr),
                    indexInDeque(it.indexInDeque + offset), indexInBlock(it.indexInBlock) {
                if (offset > 0) {
                    indexInBlock += offset;
                    while (indexInBlock >= blockPtr->elementNum) {
                        indexInBlock = indexInBlock - blockPtr->elementNum;
                        blockPtr = blockPtr->nxtBlock;
                    }
                } else if (offset < 0) {
                    offset = -offset;
                    if (offset > indexInBlock) {
                        offset -= indexInBlock + 1;
                        blockPtr = blockPtr->preBlock;
                        while (offset >= blockPtr->elementNum) {
                            offset -= blockPtr->elementNum;
                            blockPtr = blockPtr->preBlock;
                        }
                        indexInBlock = blockPtr->elementNum - offset - 1;
                    }
                }
            }

            explicit iterator(ULLBlock *blockP, size_t id) :
                    subject(nullptr), blockPtr(blockP), indexInDeque(0), indexInBlock(id) {}

            /*无符号类型与有符号类型变量同时出现在运算中时默认转为无符号
if ((n > 0 && n >= -index) || (n < 0 && -n > index))// n>0 处判断是否溢出size_t范围
    throw invalid_iterator("Index out of Range of size_t (operator+)");*/

            /*if ((n > 0 && n > index) || (n < 0 && -n >= -index))
                throw invalid_iterator("Index out of Range of size_t (operator-)");*/

            /**
             * return a new iterator which pointer n-next elements
             *   if there are not enough elements, iterator becomes invalid
             * as well as operator-
             */
            iterator operator+(const int &n) const { return iterator(*this, n); }

            iterator operator-(const int &n) const { return iterator(*this, -n); }

            int operator-(const iterator &rhs) const {
                if (subject != rhs.subject)throw invalid_iterator();
                return int(indexInDeque - rhs.indexInDeque);
            }

            iterator &operator+=(const int &n) {
                *this = iterator(*this, n);
                return *this;
            }

            iterator &operator-=(const int &n) {
                *this = iterator(*this, -n);
                return *this;
            }

            iterator operator++(int) {
                iterator tempIt(*this);
                *this = iterator(*this, 1);
                return tempIt;
            }// iter++

            iterator &operator++() {
                *this = iterator(*this, 1);
                return *this;
            }// ++iter

            iterator operator--(int) {
                iterator tempIt(*this);
                *this = iterator(*this, -1);
                return tempIt;
            }// iter--

            iterator &operator--() {
                *this = iterator(*this, -1);
                return *this;
            }// --iter

            /**
             * TODO *it
             * 		throw if iterator is invalid
             */
            T &operator*() const {
                if (indexInBlock < 0 || indexInBlock >= blockPtr->elementNum)
                    throw invalid_iterator();
                else return *(blockPtr->getElementPtr(indexInBlock));
            }

            /**
             * TODO it->field
             * 		throw if iterator is invalid
             */
            T *operator->() const noexcept { return &(operator*()); }

            /**
             * a operator to check whether two iterators are same (pointing to the same memory).
             */
            bool operator==(const iterator &rhs) const {
                return (indexInDeque == rhs.indexInDeque && subject == rhs.subject);
            }

            bool operator==(const const_iterator &rhs) const {
                return (indexInDeque == rhs.indexInDeque && subject == rhs.subject);
            }

            /**
             * some other operator for iterator.
             */
            bool operator!=(const iterator &rhs) const { return !(*this == rhs); }

            bool operator!=(const const_iterator &rhs) const { return !(*this == rhs); }
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

        deque(const deque &other) : totElementNumber(other.totElementNumber) {
            if (other.headBlock != nullptr) {
                headBlock = new ULLBlock(*(other.headBlock));
                ULLBlock *thisPtr = headBlock, *otherPtr = other.headBlock;
                while (otherPtr->nxtBlock != nullptr) {
                    otherPtr = otherPtr->nxtBlock;
                    thisPtr->nxtBlock = new ULLBlock(*otherPtr);
                    thisPtr->nxtBlock->prePtr = thisPtr;
                    thisPtr = thisPtr->nxtBlock;
                }
                tailBlock = thisPtr;
            } else {
                headBlock = nullptr;
                tailBlock = nullptr;
            }
        }

        ~deque() { clear(); }


        deque &operator=(const deque &other) {
            if (this == &other)return *this;
            clear();
            if (other.headBlock != nullptr) {
                headBlock = new ULLBlock(*(other.headBlock));
                ULLBlock *thisPtr = headBlock, *otherPtr = other.headBlock;
                while (otherPtr->nxtBlock != nullptr) {
                    otherPtr = otherPtr->nxtBlock;
                    thisPtr->nxtBlock = new ULLBlock(*otherPtr);
                    thisPtr->nxtBlock->prePtr = thisPtr;
                    thisPtr = thisPtr->nxtBlock;
                }
                tailBlock = thisPtr;
            } else {
                headBlock = nullptr;
                tailBlock = nullptr;
            }
            return *this;
        }


        T &at(const size_t &pos) {
            if (totElementNumber == 0)throw index_out_of_bound();
            else return *(headBlock->getElementPtr(pos));
        }

        const T &at(const size_t &pos) const { return at(pos); }

        T &operator[](const size_t &pos) { return *(headBlock->getElementPtr(pos)); }

        const T &operator[](const size_t &pos) const { return operator[](pos); }


        const T &front() const {
            if (totElementNumber == 0)throw container_is_empty();
            return *(headBlock->getFrontElementPtr());
        }

        const T &back() const {
            if (totElementNumber == 0)throw container_is_empty();
            return *(tailBlock->getBackElementPtr());
        }

        /**
         * returns an iterator to the beginning.
         */
        iterator begin() { return iterator(this, 0); }

        const_iterator cbegin() const {}

        /**
         * returns an iterator to the end.
         */
        iterator end() { return iterator(this, 0, true); }

        const_iterator cend() const {}

        /**
         * checks whether the container is empty.
         */
        bool empty() const { return (totElementNumber == 0); }

        /**
         * returns the number of elements
         */
        size_t size() const { return totElementNumber; }

        /**
         * clears the contents
         */
        void clear() {
            if (headBlock != nullptr) {
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
        }

        /**
         * inserts elements at the specified locat on in the container.
         * inserts value before pos
         * returns an iterator pointing to the inserted value
         *     throw if the iterator is invalid or it point to a wrong place.
         */
        iterator insert(iterator pos, const T &value) {
            if (totElementNumber++ == 0) {
                headBlock = new ULLBlock;
                std::cout << pos.indexInBlock << ", " << pos.indexInDeque << std::endl;
                if (pos.indexInDeque != 0)throw index_out_of_bound();
                headBlock->insertElement(value, 0);
                std::cout << "cp2" << std::endl;
                return iterator(this, 0, false);
            } else {
                iterator tempIt = pos.blockPtr->insertElement(value, pos.indexInBlock);
                tempIt.indexInDeque = pos.indexInDeque;
                tempIt.subject = pos.subject;
                return tempIt;
            }
        }

        /**
         * removes specified element at pos.
         * removes the element at pos.
         * returns an iterator pointing to the following element, if pos pointing to the last element, end() will be returned.
         * throw if the container is empty, the iterator is invalid or it points to a wrong place.
         */
        iterator erase(iterator pos) {
            if (totElementNumber == 0)throw container_is_empty();
            if (totElementNumber-- == 1) {
                if (pos.indexInDeque != 0)throw index_out_of_bound();
                delete headBlock;
                headBlock = nullptr;
                return iterator(this, 0, false);
            } else {
                iterator tempIt = pos.blockPtr->deleteElemnt(pos.indexInBlock);
                tempIt.indexInDeque = pos.indexInDeque;
                tempIt.subject = pos.subject;
                return tempIt;
            }
        }

        /**
         * adds an element to the end
         */
        void push_back(const T &value) { insert(--end(), value); }

        /**
         * removes the last element
         *     throw when the container is empty.
         */
        void pop_back() {
            if (totElementNumber == 0)throw container_is_empty();
            erase(--end());
        }

        /**
         * inserts an element to the beginning.
         */
        void push_front(const T &value) {
            std::cout << "cpPF1" << std::endl;
            insert(begin(), value);
            std::cout << "cpPF2" << std::endl;
        }

        /**
         * removes the first element.
         *     throw when the container is empty.
         */
        void pop_front() { erase(begin()); }

#ifdef PAPERL_DEQUE_DEBUG

        void debugPrint() {
            ULLBlock *_p = headBlock;
            std::cout << "num: " << totElementNumber << ", head: "
                      << headBlock << ", tail: " << tailBlock << std::endl << std::endl;
            while (_p != nullptr) {
                std::cout << "nxt: " << _p->nxtBlock << ", pre: " << _p->preBlock << std::endl;
                std::cout << "num: " << _p->elementNum;
                for (int _i = 0; _i < BLOCK_ELEMENT_NUMBER; ++_i) {
                    std::cout << *(_p->elementData[_i]) << "\t";
                    if (_i % 5 == 0)std::cout << std::endl;
                }
                std::cout << std::endl;
                _p = _p->nxtBlock;
            }
        }

#endif

    };

}

#endif
