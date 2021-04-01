#ifndef PTL_PTF_H
#define PTL_PTF_H

#include <cstdio>

namespace PTF {
// Here is PaperL's Template Function
// Version: 0.42
// Update Time: 2021.4.1

    template<typename T1, typename T2>
    struct sameType {
        operator bool() {
            return false;
        }
    };

    template<typename T>
    struct sameType<T, T> {
        operator bool() {
            return true;
        }
    };

    template<typename T>
    inline T qRead() {
        char _c = getchar();
        bool _sign = false;
        T _k = 0;
        while (_c < '0' || _c > '9') {
            if (_c == '-') _sign = true;
            _c = getchar();
        }
        while (_c >= '0' && _c <= '9') {
            _k = (_k << 3) + (_k << 1) - 48 + _c;
            _c = getchar();
        }
        if (_sign) _k = -_k;
        return _k;
    }

    template<typename T>
    inline void qRead(T &_k) {
        char _c = getchar();
        bool _sign = false;
        _k = 0;
        while (_c < '0' || _c > '9') {
            if (_c == '-') _sign = true;
            _c = getchar();
        }
        while (_c >= '0' && _c <= '9') {
            _k = (_k << 3) + (_k << 1) - 48 + _c;
            _c = getchar();
        }
        if (_sign) _k = -_k;
    }

    template<typename T, typename... argL>
    inline void qRead(T &_k, argL &... _argList) {
        qRead(_k);
        qRead(_argList...);
    }

    template<typename T>
    inline void qWrite(T _k) {
        if (_k != 0) {
            int _p = 0;
            char _c[24];
            if (_k < 0)putchar('-'), _k = -_k;
            while (_k) _c[_p++] = _k % 10 + 48, _k /= 10;
            while (_p--)putchar(_c[_p]);
        } else putchar('0');
    }

    template<typename T, typename... argL>
    inline void qWrite(T _k, argL ... _argList) {
        qWrite(_k);
        qWrite(_argList...);
    }

    template<typename T, typename... argL>
    inline void qWrite(const char _s, T _k, argL... _argList) {
        qWrite(_k);
        putchar(_s);
        if (sizeof...(_argList) != 0)
            qWrite(_s, _argList...);
    }

    template<typename T>
    inline T maxN(const T &_x, const T &_y) { return (_x > _y) ? _x : _y; }

    template<typename T, typename... argL>
    inline T maxN(const T &_x, const T &_y, const argL &... _argList) {
        return ((_x > _y) ? maxN(_x, _argList...) : maxN(_y, _argList...));
    }
}

#endif //PTL_PTF_H
