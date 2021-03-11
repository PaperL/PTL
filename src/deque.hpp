#ifndef SJTU_DEQUE_HPP
#define SJTU_DEQUE_HPP

#include "exceptions.hpp"

#include <cstddef>
#include <cstring> // memset

//#define PAPERL_DEQUE_DEBUG


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
                const size_t nxtNum = elementNum - leftNum;
                ULLBlock *newBlock = new ULLBlock;

                newBlock->preBlock = this;
                newBlock->nxtBlock = nxtBlock;
                newBlock->elementNum = nxtNum;
                for (size_t i = leftNum; i < elementNum; ++i) {
                    newBlock->elementData[i - leftNum] = elementData[i];
                }

                if (nxtBlock != nullptr) nxtBlock->preBlock = newBlock;
                nxtBlock = newBlock;

                memset(elementData + leftNum, 0, sizePtr * nxtNum);
                elementNum = leftNum;
            }

            void mergeBlock() {
                for (size_t i = 0; i < nxtBlock->elementNum; ++i) {
                    elementData[elementNum + i] = nxtBlock->elementData[i];
                    nxtBlock->elementData[i] = nullptr;
                }
                elementNum += nxtBlock->elementNum;
                if (nxtBlock->nxtBlock != nullptr) {
                    nxtBlock = nxtBlock->nxtBlock;
                    delete nxtBlock->preBlock;
                    nxtBlock->preBlock = this;
                } else {
                    delete nxtBlock;
                    nxtBlock = nullptr;
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


            iterator insertElement(const T &arg, size_t id,
                                   deque<T, BLOCK_ELEMENT_NUMBER, BLOCK_MERGE_BOUND> *sub,
                                   size_t idInDeque, ULLBlock *&tbp) { // tail block ptr
                /*if (id > elementNum) {
                    if (nxtBlock == nullptr)throw index_out_of_bound();
                    else nxtBlock->insertElement(arg, id - elementNum);
                } else {*/
                if (id > elementNum)throw index_out_of_bound();
                // todo 需要真正实现迭代器的 invalid 功能
                if (elementNum == BLOCK_ELEMENT_NUMBER) {
                    splitBlock();
                    if (nxtBlock->nxtBlock == nullptr) tbp = nxtBlock;
                    if (id > elementNum)
                        return nxtBlock->insertElement(arg, id - elementNum,
                                                       sub, idInDeque, tbp);
                }
                for (size_t i = elementNum; i > id; --i)elementData[i] = elementData[i - 1];
                elementData[id] = new T(arg);
                ++elementNum;
                return iterator(sub, this, idInDeque, id);
            }

            iterator deleteElement(size_t id,
                                   deque<T, BLOCK_ELEMENT_NUMBER, BLOCK_MERGE_BOUND> *sub,
                                   size_t idInDeque, ULLBlock *&tbp) {
                /*if (id >= elementNum) {
                    if (nxtBlock == nullptr)throw index_out_of_bound();
                    else nxtBlock->deleteElement(arg, id - elementNum);
                } else {*/
                if (id >= elementNum)throw index_out_of_bound();
                delete elementData[id];
                --elementNum;
                for (size_t i = id; i < elementNum; ++i)elementData[i] = elementData[i + 1];
                elementData[elementNum] = nullptr;
                if (nxtBlock != nullptr &&
                    (elementNum == 0 || elementNum + nxtBlock->elementNum < BLOCK_MERGE_BOUND)) {
                    if (nxtBlock == tbp)tbp = this;
                    mergeBlock();
                }
                if (id != 0 && id == elementNum) { // 特判(首)块末/删至空块
                    if (this->nxtBlock != nullptr)
                        return iterator(sub, this->nxtBlock, idInDeque, 0);
                }
                return iterator(sub, this, idInDeque, id);
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
            iterator() : subject(nullptr), blockPtr(nullptr), indexInDeque(0), indexInBlock(0) {}

            explicit iterator(deque<T, BLOCK_ELEMENT_NUMBER, BLOCK_MERGE_BOUND> *sub,
                              const size_t &id, bool tailConstruct = false) :
                    subject(sub), blockPtr(subject->headBlock), indexInDeque(id), indexInBlock(id) {
                if (tailConstruct) {
                    indexInDeque = subject->totElementNumber;
                    blockPtr = subject->tailBlock;
                    indexInBlock = (blockPtr != nullptr) ? (blockPtr->elementNum) : 0;
                } else {
                    if (blockPtr != nullptr) {
                        while (blockPtr->nxtBlock != nullptr && indexInBlock >= blockPtr->elementNum) {
                            indexInBlock -= blockPtr->elementNum;
                            blockPtr = blockPtr->nxtBlock;
                        }
                    }
                }
            }

            explicit iterator(const iterator &it, int offset) :
                    subject(it.subject), blockPtr(it.blockPtr),
                    indexInDeque(it.indexInDeque + offset), indexInBlock(it.indexInBlock) {
                if (offset > 0) {
                    indexInBlock += offset;
                    while (blockPtr->nxtBlock != nullptr && indexInBlock >= blockPtr->elementNum) {
                        indexInBlock = indexInBlock - blockPtr->elementNum;
                        blockPtr = blockPtr->nxtBlock;
                    }
                } else if (offset < 0) {
                    offset = -offset;
                    if (offset > indexInBlock) {
                        offset -= indexInBlock + 1;
                        blockPtr = blockPtr->preBlock;
                        if (blockPtr == nullptr) throw invalid_iterator();
                        while (offset >= blockPtr->elementNum) {
                            offset -= blockPtr->elementNum;
                            blockPtr = blockPtr->preBlock;
                            if (blockPtr == nullptr) throw invalid_iterator();
                        }
                        indexInBlock = blockPtr->elementNum - 1 - offset;
                    } else indexInBlock -= offset;
                }
            }

            explicit iterator(deque<T, BLOCK_ELEMENT_NUMBER, BLOCK_MERGE_BOUND> *sub,
                              ULLBlock *blockP, size_t idInDeque, size_t idInBlock) :
                    subject(sub), blockPtr(blockP), indexInDeque(idInDeque), indexInBlock(idInBlock) {}

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
                if (indexInBlock >= blockPtr->elementNum) {
                    throw invalid_iterator();
                } else return *(blockPtr->getElementPtr(indexInBlock));
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
            friend class deque;

        private:
            const deque<T, BLOCK_ELEMENT_NUMBER, BLOCK_MERGE_BOUND> *subject;
            size_t indexInDeque;
            ULLBlock *blockPtr;
            size_t indexInBlock;
        public:
            const_iterator() : subject(nullptr), blockPtr(nullptr), indexInDeque(0), indexInBlock(0) {}

            explicit const_iterator(const deque<T, BLOCK_ELEMENT_NUMBER, BLOCK_MERGE_BOUND> *sub,
                                    const size_t &id, bool tailConstruct = false) :
                    subject(sub), blockPtr(subject->headBlock), indexInDeque(id), indexInBlock(id) {
                if (tailConstruct) {
                    indexInDeque = subject->totElementNumber;
                    blockPtr = subject->tailBlock;
                    indexInBlock = (blockPtr != nullptr) ? (blockPtr->elementNum) : 0;
                } else {
                    if (blockPtr != nullptr) {
                        while (blockPtr->nxtBlock != nullptr && indexInBlock >= blockPtr->elementNum) {
                            indexInBlock -= blockPtr->elementNum;
                            blockPtr = blockPtr->nxtBlock;
                        }
                    }
                }
            }

            explicit const_iterator(const const_iterator &it, int offset) :
                    subject(it.subject), blockPtr(it.blockPtr),
                    indexInDeque(it.indexInDeque + offset), indexInBlock(it.indexInBlock) {
                if (offset > 0) {
                    indexInBlock += offset;
                    while (blockPtr->nxtBlock != nullptr && indexInBlock >= blockPtr->elementNum) {
                        indexInBlock = indexInBlock - blockPtr->elementNum;
                        blockPtr = blockPtr->nxtBlock;
                    }
                } else if (offset < 0) {
                    offset = -offset;
                    if (offset > indexInBlock) {
                        offset -= indexInBlock + 1;
                        blockPtr = blockPtr->preBlock;
                        if (blockPtr == nullptr) throw invalid_iterator();
                        while (offset >= blockPtr->elementNum) {
                            offset -= blockPtr->elementNum;
                            blockPtr = blockPtr->preBlock;
                        }
                        indexInBlock = blockPtr->elementNum - offset - 1;
                    } else indexInBlock -= offset;
                }
            }

            explicit const_iterator(deque<T, BLOCK_ELEMENT_NUMBER, BLOCK_MERGE_BOUND> *sub,
                                    ULLBlock *blockP, size_t idInDeque, size_t idInBlock) :
                    subject(sub), blockPtr(blockP), indexInDeque(idInDeque), indexInBlock(idInBlock) {}

            const_iterator operator+(const int &n) const { return const_iterator(*this, n); }

            const_iterator operator-(const int &n) const { return const_iterator(*this, -n); }

            int operator-(const const_iterator &rhs) const {
                if (subject != rhs.subject)throw invalid_iterator();
                return int(indexInDeque - rhs.indexInDeque);
            }

            const_iterator &operator+=(const int &n) {
                *this = const_iterator(*this, n);
                return *this;
            }

            const_iterator &operator-=(const int &n) {
                *this = const_iterator(*this, -n);
                return *this;
            }

            const_iterator operator++(int) {
                const_iterator tempIt(*this);
                *this = const_iterator(*this, 1);
                return tempIt;
            }// iter++

            const_iterator &operator++() {
                *this = const_iterator(*this, 1);
                return *this;
            }// ++iter

            const_iterator operator--(int) {
                const_iterator tempIt(*this);
                *this = const_iterator(*this, -1);
                return tempIt;
            }// iter--

            const_iterator &operator--() {
                *this = const_iterator(*this, -1);
                return *this;
            }// --iter

            const T &operator*() const {
                if (indexInBlock >= blockPtr->elementNum)
                    throw invalid_iterator();
                else return *(blockPtr->getElementPtr(indexInBlock));
            }

            const T *operator->() const noexcept { return &(operator*()); }

            bool operator==(const iterator &rhs) const {
                return (indexInDeque == rhs.indexInDeque && subject == rhs.subject);
            }

            bool operator==(const const_iterator &rhs) const {
                return (indexInDeque == rhs.indexInDeque && subject == rhs.subject);
            }

            bool operator!=(const iterator &rhs) const { return !(*this == rhs); }

            bool operator!=(const const_iterator &rhs) const { return !(*this == rhs); }
        };

        deque() : totElementNumber(0), headBlock(nullptr), tailBlock(nullptr) {}

        deque(const deque &other) : totElementNumber(other.totElementNumber) {
            if (other.headBlock != nullptr) {
                headBlock = new ULLBlock(*(other.headBlock));
                ULLBlock *thisPtr = headBlock, *otherPtr = other.headBlock;
                while (otherPtr->nxtBlock != nullptr) {
                    otherPtr = otherPtr->nxtBlock;
                    thisPtr->nxtBlock = new ULLBlock(*otherPtr);
                    thisPtr->nxtBlock->preBlock = thisPtr;
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
            totElementNumber = other.totElementNumber;
            if (other.headBlock != nullptr) {
                headBlock = new ULLBlock(*(other.headBlock));
                ULLBlock *thisPtr = headBlock, *otherPtr = other.headBlock;
                while (otherPtr->nxtBlock != nullptr) {
                    otherPtr = otherPtr->nxtBlock;
                    thisPtr->nxtBlock = new ULLBlock(*otherPtr);
                    thisPtr->nxtBlock->preBlock = thisPtr;
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

        const T &at(const size_t &pos) const {
            if (totElementNumber == 0)throw index_out_of_bound();
            else return *(headBlock->getElementPtr(pos));
        }

        T &operator[](const size_t &pos) { return *(headBlock->getElementPtr(pos)); }

        const T &operator[](const size_t &pos) const { return *(headBlock->getElementPtr(pos)); }


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

        const_iterator cbegin() const { return const_iterator(this, 0); }

        /**
         * returns an iterator to the end.
         */
        iterator end() { return iterator(this, 0, true); }

        const_iterator cend() const { return const_iterator(this, 0, true); }

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
            //std::cout << "insert: " << pos.indexInDeque << ", " << pos.indexInBlock << ", "
            //          << pos.blockPtr << std::endl;
            if (this != pos.subject)throw invalid_iterator();
            if (totElementNumber++ == 0) {
                headBlock = new ULLBlock; // headBlock 仅在首块生成/消亡时变动
                tailBlock = headBlock;
                if (pos.indexInDeque != 0)throw index_out_of_bound();
                headBlock->insertElement(value, 0,
                                         this, pos.indexInDeque, tailBlock);
                return iterator(this, 0, false);
            } else
                return pos.blockPtr->insertElement(value, pos.indexInBlock,
                                                   pos.subject, pos.indexInDeque, tailBlock);
        }

        /**
         * removes specified element at pos.
         * removes the element at pos.
         * returns an iterator pointing to the following element, if pos pointing to the last element, end() will be returned.
         * throw if the container is empty, the iterator is invalid or it points to a wrong place.
         */
        iterator erase(iterator pos) {
            if (totElementNumber == 0)throw container_is_empty();
            //std::cout << "erase: " << pos.indexInDeque << ", " << pos.indexInBlock << ", "
            //          << pos.blockPtr << std::endl;
            iterator tempIt = pos.blockPtr->deleteElement(pos.indexInBlock,
                                                          pos.subject, pos.indexInDeque, tailBlock);
            if (tempIt.blockPtr->elementNum == 0) {
                if (tempIt.blockPtr->preBlock != nullptr) {// 删除末块
                    tempIt.blockPtr = tempIt.blockPtr->preBlock;
                    delete tempIt.blockPtr->nxtBlock;
                    tempIt.blockPtr->nxtBlock = nullptr;
                    tailBlock = tempIt.blockPtr;
                    tempIt.indexInBlock = tempIt.blockPtr->elementNum - 1;
                } else {
                    headBlock = nullptr;
                    tailBlock = nullptr;
                    delete tempIt.blockPtr;
                    tempIt = iterator(pos.subject, nullptr, 0, 0);
                }
            }
            --totElementNumber;
            return tempIt;
        }

        void push_back(const T &value) { insert(end(), value); }

        void pop_back() {
            if (totElementNumber == 0)throw container_is_empty(); // --end()会出错
            else erase(--end());
        }

        void push_front(const T &value) { insert(begin(), value); }

        void pop_front() { erase(begin()); }

#ifdef PAPERL_DEQUE_DEBUG

        void debugPrint() {
            std::cout << "================begin" << std::endl;
            ULLBlock *_p = headBlock;
            std::cout << "num: " << totElementNumber << ", head: "
                      << headBlock << ", tail: " << tailBlock << std::endl << std::endl;
            while (_p != nullptr) {
                std::cout << "=== num: " << _p->elementNum << ", address: " << _p << std::endl;
                std::cout << "nxt: " << _p->nxtBlock << ", pre: " << _p->preBlock << std::endl;
                for (int _i = 0; _i < BLOCK_ELEMENT_NUMBER; ++_i) {
                    if (_p->elementData[_i] == nullptr)
                        std::cout << "NULL" << "\t";
                    else std::cout << *(_p->elementData[_i]) << "\t";
                    if (_i % 5 == 4)std::cout << std::endl;
                }
                _p = _p->nxtBlock;
            }
            std::cout << "------------------end" << std::endl;
            std::cout << std::endl;
        }

#endif

    };

}

#endif
