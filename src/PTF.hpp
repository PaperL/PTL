#pragma region PaperL_Header
#ifndef PTL_PTF_H
#define PTL_PTF_H

#include <cstdio>

namespace PTF {
#pragma region PTF_DESCRIPTION
/*
 * "PaperL's Template Function"
 *
 * Version: 0.8 todo 完成中
 * Last Update Time: 2021.4.12
 * Last Update Content:
 *      修改带分隔符的 qWrite 函数分隔符传参方式(参数->形参)
 *      扩展 qWrite 函数功能(支持行末符, 分隔字符串)
 *      采用简写函数模板(c++20)重写部分函数
 *      采用 pragma region 整理代码
 *      用 fold expression 及制约函数(c++20) 重写部分函数
 *          0.8 版本之前的 IO 函数会传参 n^2 数量级次, 极大影响效率
 */
#pragma endregion PTF_DESCRIPTION

#pragma region PTF_TYPE

    template<typename T1, typename T2>
    struct sameType {
        constexpr operator bool() { return false; }
    };

    template<typename T>
    struct sameType<T, T> {
        constexpr operator bool() { return true; }
    };

#pragma endregion PTF_TYPE

#pragma region PTF_MEMORY

    template<typename T>
    inline void swapT(T &_x, T &_y) {
        T _temp(_x);
        _x = _y;
        _y = _temp;
    }

#pragma endregion PTF_MEMORY

#pragma region PTF_MATH

    template<typename T>
    inline T maxN(const T &_x, const T &_y) { return (_x > _y) ? _x : _y; }

    //todo 利用制约函数和简写函数模板语法修改
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

#pragma endregion PTF_MATH

#pragma region PTF_IO

    inline void qRead(auto &_k) {
        char _c = getchar();
        bool _sign = false;
        _k = 0;
        while (_c < '0' || _c > '9') {
            if (_c == '-') _sign = true;
            _c = getchar();
        }
        while (_c >= '0' && _c <= '9') {
            _k = _k * 10 - 48 + _c; // 此处位运算替代 *10 可能不一定更快
            _c = getchar();
        }
        if (_sign) _k = -_k;
    }

    template<typename T>
    inline T qRead() {
        T _k;
        qRead(_k);
        return _k;
    }

    // c++20 fold expression
    inline void qRead(auto &... _argList) { (qRead(_argList), ...); }

    template<typename T>
    inline void qWrite(T _k) {
        if constexpr(sameType<T, char>()) putchar(_k);
        else if constexpr(sameType<T, const char *>() || sameType<T, char *>()) printf("%s", _k);
        else if (_k != 0) {
            int _p = 0;
            char _c[24];
            if (_k < 0)putchar('-'), _k = -_k;
            while (_k) _c[_p++] = _k % 10 + 48, _k /= 10;
            while (_p--)putchar(_c[_p]);
        } else putchar('0');
    }

    inline void qWrite(auto ... _argList) { (qWrite(_argList), ...); }

    // _s for split character
    template<char _s>
    inline void qWrite(auto _k) { qWrite(_k), putchar(_s); }

    template<char _s>
    inline void qWrite(auto... _argList) { (qWrite<_s>(_argList), ...); }

    // _eol for end of line character
    template<char _s, char _eol>
    inline void qWrite(auto... _argList) { (qWrite<_s, _eol>(_argList), ...), putchar(_eol); }

    // _ss for split string
    template<char *_ss>
    inline void qWrite(auto _k) { qWrite(_k), printf("%s", _ss); }

    template<char *_ss>
    inline void qWrite(auto... _argList) { (qWrite<_ss>(_argList), ...); }

    template<char *_ss, char _eol>
    inline void qWrite(auto... _argList) { qWrite<_ss, _eol>(_argList...), putchar(_eol); }

#pragma endregion PTF_IO
}

#endif //PTL_PTF_H
#pragma endregion