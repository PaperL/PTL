#pragma region PaperL_Header
#ifndef PTL_PTF_H
#define PTL_PTF_H

#include <cstdio>

namespace PTF {
// Here is PaperL's Template Function
// Version: 0.50
// Update Time: 2021.4.11

    template<typename T1, typename T2>
    struct sameType {
        constexpr operator bool() {
            return false;
        }
    };

    template<typename T>
    struct sameType<T, T> {
        constexpr operator bool() {
            return true;
        }
    };

    template<typename T>
    inline void swapT(T &_x, T &_y) {
        T _temp(_x);
        _x = _y;
        _y = _temp;
    }

    template<typename T>
    inline T maxN(const T &_x, const T &_y) { return (_x > _y) ? _x : _y; }

    template<typename T, typename... argL>
    inline T maxN(const T &_x, const T &_y, const argL &... _argList) {
        return ((_x > _y) ? maxN(_x, _argList...) : maxN(_y, _argList...));
    }

    template<typename T>
    inline T minN(const T &_x, const T &_y) { return (_x < _y) ? _x : _y; }

    template<typename T, typename... argL>
    inline T minN(const T &_x, const T &_y, const argL &... _argList) {
        return ((_x < _y) ? minN(_x, _argList...) : minN(_y, _argList...));
    }

    template<typename T>
    inline T qRead() {
        T _k;
        qRead(_k);
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
            _k = _k * 10 - 48 + _c;
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
        if constexpr(sameType<T, char>()) {
            putchar(_k);
            return;
        } else if constexpr(sameType<T, const char *>() || sameType<T, char *>()) {
            size_t _p = 0;
            while (_k[_p] != '\0')putchar(_k[_p++]);
            return;
        } else if (_k != 0) {
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
}

#endif //PTL_PTF_H
#pragma endregion