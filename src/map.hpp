/**
 * implement a container like std::map
 */
#ifndef SJTU_MAP_HPP
#define SJTU_MAP_HPP

#include <functional> // std::less<T>
#include <cstddef>
#include "utility.hpp" // pair
#include "exceptions.hpp"

#include <iostream> // print debug info

namespace sjtu {

    template<class Key, class Value, class Compare = std::less<Key> >
    class map {

#pragma region DECLARATION
    public:
        typedef pair<const Key, Value> Element;

    private:
        enum nodeColorENUM {
            RED, BLACK
        };

        struct Node {
            Element *element;
            nodeColorENUM color;
            Node *lChild, *rChild, *parent;

            Node(Element *ele = nullptr, nodeColorENUM col = BLACK,
                 Node *lc = nullptr, Node *rc = nullptr, Node *par = nullptr)
                    : element(ele), color(col), lChild(lc), rChild(rc), parent(par) {}

            Node(const Node &other) : element(new Element(*other.element)), color(other.color),
                                      lChild(nullptr), rChild(nullptr), parent(nullptr) {}

            ~Node() { delete element; }
        } *NilPtr, *beginNodePtr;

        Compare compare;
        size_t elementNum;

        template<typename _T>
        void _swap(_T &_x, _T &_y) {
            _T _t(_x);
            _x = _y;
            _y = _t;
        }

#pragma endregion DECLARATION

#define ROOT_PTR (NilPtr->lChild)

#pragma region TREEOPERATION
    private:

        void rotateL(Node *x) {
            Node *y = x->parent;
            if (x->lChild != NilPtr) x->lChild->parent = y;
            y->rChild = x->lChild;
            if (y->parent->lChild == y) y->parent->lChild = x;
            else y->parent->rChild = x;
            x->parent = y->parent;
            y->parent = x;
            x->lChild = y;
        }

        void rotateR(Node *x) {
            Node *y = x->parent;
            if (x->rChild != NilPtr) x->rChild->parent = y;
            y->lChild = x->rChild;
            if (y->parent->lChild == y) y->parent->lChild = x;
            else y->parent->rChild = x;
            x->parent = y->parent;
            y->parent = x;
            x->rChild = y;
        }

        void transplant(Node *const x, Node *const y) {
            if (x == x->parent->lChild)
                x->parent->lChild = y;
            else x->parent->rChild = y;
            y->parent = x->parent;
        }

        void insertFixup(Node *x) {
            Node *y = nullptr;
            while (x->parent->color == RED) {
                if (x->parent->parent->lChild == x->parent) {
                    y = x->parent->parent->rChild;
                    if (y->color == RED) {
                        x->parent->color = y->color = BLACK;
                        y->parent->color = RED;
                        x = y->parent;
                    }
                    else {
                        y = x->parent;
                        if (y->rChild == x) rotateL(x), x = y, y = x->parent;
                        rotateR(y), _swap(y->color, y->rChild->color);
                    }
                }
                else {
                    y = x->parent->parent->lChild;
                    if (y->color == RED) {
                        x->parent->color = y->color = BLACK;
                        y->parent->color = RED;
                        x = y->parent;
                    }
                    else {
                        y = x->parent;
                        if (y->lChild == x) rotateR(x), x = y, y = x->parent;
                        rotateL(y), _swap(y->color, y->lChild->color);
                    }
                }
            }
            ROOT_PTR->color = BLACK;
        }

        void eraseFixup(Node *x) {
            for (Node *y; x->parent != NilPtr && x->color == BLACK;) {
                if (x->parent->lChild == x) {
                    y = x->parent->rChild;
                    if (y->color == RED) rotateL(y), _swap(y->color, y->lChild->color);
                    else if (y->lChild->color == BLACK && y->rChild->color == BLACK)
                        y->color == RED, x = x->parent;
                    else {
                        if (y->rChild->color == BLACK) {
                            rotateR(y->lChild), _swap(y->color, y->parent->color);
                            y = y->parent;
                        }
                        rotateL(y), _swap(y->color, y->lChild->color), y->rChild->color = BLACK;
                        x = ROOT_PTR;
                    }
                }
                else {
                    y = x->parent->lChild;
                    if (y->color == RED) rotateL(y), _swap(y->color, y->rChild->color);
                    else if (y->lChild->color == BLACK && y->rChild->color == BLACK)
                        y->color == RED, x = x->parent;
                    else {
                        if (y->lChild->color == BLACK) {
                            rotateL(y->rChild), _swap(y->color, y->parent->color);
                            y = y->parent;
                        }
                        rotateR(y), _swap(y->color, y->rChild->color), y->lChild->color = BLACK;
                        x = ROOT_PTR;
                    }
                }
            }
            x->color = BLACK;
        }

        Node *findMin(Node *p) const {
            while (p->lChild != NilPtr) p = p->lChild;
            return p;
        }

        Node *findMax(Node *p) const {
            while (p->rChild != NilPtr) p = p->rChild;
            return p;
        }

        Node *findPre(Node *p) const {
            if (p->lChild == NilPtr) {
                while (p != NilPtr && p->parent->lChild == p) p = p->parent;
                if (p == NilPtr) throw invalid_iterator();
                p = p->parent;
            }
            else p = findMax(p->lChild);
            return p;
        }

        Node *findSuc(Node *p) const {
            if (p->rChild == NilPtr) {
                while (p != NilPtr && p->parent->rChild == p) p = p->parent;
                if (p == NilPtr)throw invalid_iterator();
                p = p->parent;
            }
            else p = findMin(p->rChild);
            return p;
        }

        Node *_searchKey(const Key &key) const {
            Node *p = ROOT_PTR;
            while (p != NilPtr) {
                bool b1 = compare(key, p->element->first),
                        b2 = compare(p->element->first, key);
                if (b1 || b2) p = b1 ? p->lChild : p->rChild;
                else break;
            }
            return p;
        }

        Node *_insertEle(Element *elePtr) {
            Node *newNode = new Node(elePtr, RED, NilPtr, NilPtr);
            Node *x = ROOT_PTR, *y = NilPtr;
            while (x != NilPtr) {
                y = x;
                x = compare(elePtr->first, x->element->first)
                    ? x->lChild : x->rChild;
            }
            if (y == NilPtr || compare(elePtr->first, y->element->first))
                y->lChild = newNode;
            else y->rChild = newNode;
            newNode->parent = y;
            insertFixup(newNode);

            ++elementNum;
            beginNodePtr = findMin(NilPtr); // 此处可优化
            return newNode;
        }

        void _eraseNode(Node *x) {
            nodeColorENUM col = x->color;
            Node *y;
            if (x->lChild == NilPtr) y = x->rChild, transplant(x, x->rChild);
            else if (x->rChild == NilPtr) y = x->lChild, transplant(x, x->lChild);
            else {
                Node *z = findMin(x->rChild);
                col = z->color;
                y = z->rChild;
                if (z->parent != x) {  // what is Nil.parent?
                    transplant(z, z->rChild);
                    z->rChild = x->rChild;
                }
                z->rChild->parent = z;
                transplant(x, z);
                z->color = x->color, z->lChild = x->lChild;
                z->lChild->parent = z;
            }
            delete x;
            if (col == BLACK) eraseFixup(y);

            --elementNum;
            beginNodePtr = findMin(NilPtr); // 此处可优化
        }

        Node *copy(Node *ptr) {

        }

        void _destroy(const Node *p) {
            if (p == NilPtr)return;
            _destroy(p->lChild), _destroy(p->rChild);
            delete p;
        }

#pragma endregion TREEOPERATION

#pragma region ITERATOR
    public:
        class const_iterator;

        class iterator {
            // friend class map;

        private:
            const map *subject;

            Node *nodePtr;

        public:
            explicit iterator(const map *sub = nullptr, Node *ptr = nullptr)
                    : subject(sub), nodePtr(ptr) {}


            iterator(const iterator &other) : subject(other.subject), nodePtr(other.nodePtr) {}

            // it++
            iterator operator++(int) {
                iterator tempIt(*this);
                nodePtr = subject->findSuc(nodePtr);
                return tempIt;
            }

            // ++it
            iterator &operator++() {
                nodePtr = subject->findSuc(nodePtr);
                return *this;
            }

            // it--
            iterator operator--(int) {
                iterator tempIt(*this);
                nodePtr = subject->findPre(nodePtr);
                return tempIt;
            }

            // --it
            iterator &operator--() {
                nodePtr = subject->findPre(nodePtr);
                return *this;
            }

            Element &operator*() const { return *(nodePtr->element); }

            bool operator==(const iterator &rhs) const {
                return (subject == rhs.subject && nodePtr == rhs.nodePtr);
            }

            bool operator==(const const_iterator &rhs) const {
                return (subject == rhs.subject && nodePtr == rhs.nodePtr);
            }

            bool operator!=(const iterator &rhs) const {
                return (subject != rhs.subject || nodePtr != rhs.nodePtr);
            }

            bool operator!=(const const_iterator &rhs) const {
                return (subject != rhs.subject || nodePtr != rhs.nodePtr);
            }

            Element *operator->() const noexcept { return nodePtr->element; }
        };

        class const_iterator {
            friend class map;

        private:
            const map *const subject;

            Node *const nodePtr;
        public:
            explicit const_iterator(const map *sub = nullptr, Node *ptr = nullptr)
                    : subject(sub), nodePtr(ptr) {}

            const_iterator(const const_iterator &other) : subject(other.subject), nodePtr(other.nodePtr) {}

            const_iterator(const iterator &other) : subject(other.subject), nodePtr(other.nodePtr) {}

            // it++
            const_iterator operator++(int) {
                const_iterator tempIt(*this);
                nodePtr = subject->findSuc(nodePtr);
                return tempIt;
            }

            // ++it
            const_iterator &operator++() {
                nodePtr = subject->findSuc(nodePtr);
                return *this;
            }

            // it--
            const_iterator operator--(int) {
                const_iterator tempIt(*this);
                nodePtr = subject->findPre(nodePtr);
                return tempIt;
            }

            // --it
            const_iterator &operator--() {
                nodePtr = subject->findPre(nodePtr);
                return *this;
            }

            const Element &operator*() const { return *(nodePtr->element); }

            bool operator==(const iterator &rhs) const {
                return (subject == rhs.subject && nodePtr == rhs.nodePtr);
            }

            bool operator==(const const_iterator &rhs) const {
                return (subject == rhs.subject && nodePtr == rhs.nodePtr);
            }

            bool operator!=(const iterator &rhs) const {
                return (subject != rhs.subject || nodePtr != rhs.nodePtr);
            }

            bool operator!=(const const_iterator &rhs) const {
                return (subject != rhs.subject || nodePtr != rhs.nodePtr);
            }

            const Element *operator->() const noexcept { return nodePtr->element; }
        };

#pragma endregion ITERATOR

#pragma region BASICFUNCTION
    public:
        map() : elementNum(0), NilPtr(new Node()), beginNodePtr(NilPtr) {
            ROOT_PTR = NilPtr;
        }

        map(const map &other) : map() {
            ROOT_PTR = copy(other.NilPtr->lChild);
            elementNum = other.elementNum;
        }

        map &operator=(const map &other) {
            if (&other == this) return *this;
            _destroy(ROOT_PTR);
            ROOT_PTR = copy(other.NilPtr->lChild);
            elementNum = other.elementNum;
            beginNodePtr = findMin(NilPtr);
        }

        ~map() {
            _destroy(ROOT_PTR);
            delete NilPtr;
        }

#pragma endregion BASICFUNCTION

#pragma region USERFUNCTION
    public:
        Value &at(const Key &key) {
            Node *ptr = _searchKey(key);
            if (ptr == NilPtr) throw index_out_of_bound();
            return (ptr->element->second);
        }

        const Value &at(const Key &key) const {
            Node *ptr = _searchKey(key);
            if (ptr == NilPtr) throw index_out_of_bound();
            return (ptr->element->second);
        }

        Value &operator[](const Key &key) {
            Node *ptr = _searchKey(key);
            if (ptr == NilPtr) {
                auto *tmpEle = new Element(key, Value());
                ptr = _insertEle(tmpEle);
            }
            return (ptr->element->second);
        }

        const Value &operator[](const Key &key) const { return at(key); }

        iterator begin() { return iterator(this, beginNodePtr); }

        const_iterator cbegin() const { return const_iterator(this, beginNodePtr); }

        iterator end() { return iterator(this, NilPtr); }

        const_iterator cend() const { return const_iterator(this, NilPtr); }

        bool empty() const { return (elementNum == 0); }

        size_t size() const { return elementNum; }

        void clear() {
            elementNum = 0;
            _destroy(ROOT_PTR);
            ROOT_PTR = NilPtr;
        }

        pair<iterator, bool> insert(const Element &ele) {
            Node *ptr = _searchKey(ele.first);
            if (ptr == NilPtr) {
                auto *tmpEle = new Element(ele);
                ptr = _insertEle(tmpEle);
                return pair<iterator, bool>(iterator(this, ptr), true);
            }
            else return pair<iterator, bool>(iterator(this, ptr), false);
        }

        void erase(iterator pos) {
            if (pos.subject != this || pos == end()) throw runtime_error();
            _eraseNode(pos.nodePtr);
        }

        size_t count(const Key &key) const { return ((_searchKey(key) == NilPtr) ? 0 : 1); }

        iterator find(const Key &key) { return iterator(this, _searchKey(key)); } // NilPtr is end()

        const_iterator find(const Key &key) const { return const_iterator(this, _searchKey(key)); }

#pragma endregion USERFUNCTION

#undef ROOT_PTR
    };

}

#endif
