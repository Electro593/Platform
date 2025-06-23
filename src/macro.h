/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
**                                                                         **
**  Author: Aria Seiler                                                    **
**                                                                         **
**  This program is in the public domain. There is no implied warranty,    **
**  so use it at your own risk.                                            **
**                                                                         **
\* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

// CREDIT https://gitlab.inria.fr/gustedt/p99/-/blob/master/p99/p99_for.h

#define MAC_EVAL(...) __VA_ARGS__

#define MAC_PACKAGE(...) (__VA_ARGS__)
#define _MAC_UNPACKAGE_(D) D
#define _MAC_UNPACKAGE(P) _MAC_UNPACKAGE_ P
#define MAC_UNPACKAGE(P) _MAC_UNPACKAGE(P)

#define _MAC_CONCAT(A, B) A ## B
#define MAC_CONCAT(A, B) _MAC_CONCAT(A, B)

#define _MAC_STRINGIFY(S) #S
#define MAC_STRINGIFY(S) _MAC_STRINGIFY(S)

#define _MAC_ARGCOUNT_(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, I, ...) I
#define _MAC_ARGCOUNT(...) _MAC_ARGCOUNT_(__VA_ARGS__, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0,)
#define MAC_ARGCOUNT(...) _MAC_ARGCOUNT(__VA_OPT__(,) __VA_ARGS__)

#define _MAC_PRED_0 (-1)
#define _MAC_PRED_1   0
#define _MAC_PRED_2   1
#define _MAC_PRED_3   2
#define _MAC_PRED_4   3
#define _MAC_PRED_5   4
#define _MAC_PRED_6   5
#define _MAC_PRED_7   6
#define _MAC_PRED_8   7
#define _MAC_PRED_9   8
#define _MAC_PRED_10   9
#define _MAC_PRED_11  10
#define _MAC_PRED_12  11
#define _MAC_PRED_13  12
#define _MAC_PRED_14  13
#define _MAC_PRED_15  14
#define MAC_PRED(N) MAC_CONCAT(_MAC_PRED_, N)

#define _MAC_SUCC_0  1
#define _MAC_SUCC_1  2
#define _MAC_SUCC_2  3
#define _MAC_SUCC_3  4
#define _MAC_SUCC_4  5
#define _MAC_SUCC_5  6
#define _MAC_SUCC_6  7
#define _MAC_SUCC_7  8
#define _MAC_SUCC_8  9
#define _MAC_SUCC_9 10
#define _MAC_SUCC_10 11
#define _MAC_SUCC_11 12
#define _MAC_SUCC_12 13
#define _MAC_SUCC_13 14
#define _MAC_SUCC_14 15
#define _MAC_SUCC_15 16
#define MAC_PRED(N) MAC_CONCAT(_MAC_PRED_, N)

#define _MAC_SKIP_0(...)                  __VA_ARGS__
#define _MAC_SKIP_1(_0, ...)              __VA_ARGS__
#define _MAC_SKIP_2(_0, ...)  _MAC_SKIP_1(__VA_ARGS__)
#define _MAC_SKIP_3(_0, ...)  _MAC_SKIP_2(__VA_ARGS__)
#define _MAC_SKIP_4(_0, ...)  _MAC_SKIP_3(__VA_ARGS__)
#define _MAC_SKIP_5(_0, ...)  _MAC_SKIP_4(__VA_ARGS__)
#define _MAC_SKIP_6(_0, ...)  _MAC_SKIP_5(__VA_ARGS__)
#define _MAC_SKIP_7(_0, ...)  _MAC_SKIP_6(__VA_ARGS__)
#define _MAC_SKIP_8(_0, ...)  _MAC_SKIP_7(__VA_ARGS__)
#define _MAC_SKIP_9(_0, ...)  _MAC_SKIP_8(__VA_ARGS__)
#define _MAC_SKIP_10(_0, ...)  _MAC_SKIP_9(__VA_ARGS__)
#define _MAC_SKIP_11(_0, ...) _MAC_SKIP_10(__VA_ARGS__)
#define _MAC_SKIP_12(_0, ...) _MAC_SKIP_11(__VA_ARGS__)
#define _MAC_SKIP_13(_0, ...) _MAC_SKIP_12(__VA_ARGS__)
#define _MAC_SKIP_14(_0, ...) _MAC_SKIP_13(__VA_ARGS__)
#define _MAC_SKIP_15(_0, ...) _MAC_SKIP_14(__VA_ARGS__)
#define MAC_SKIP(N, ...) MAC_CONCAT(_MAC_SKIP_, N)(__VA_ARGS__)

#define _MAC_TAKE_0(...)
#define _MAC_TAKE_1(_1, ...) _1
#define _MAC_TAKE_2(_1, ...) _1,  _MAC_TAKE_1(__VA_ARGS__)
#define _MAC_TAKE_3(_1, ...) _1,  _MAC_TAKE_2(__VA_ARGS__)
#define _MAC_TAKE_4(_1, ...) _1,  _MAC_TAKE_3(__VA_ARGS__)
#define _MAC_TAKE_5(_1, ...) _1,  _MAC_TAKE_4(__VA_ARGS__)
#define _MAC_TAKE_6(_1, ...) _1,  _MAC_TAKE_5(__VA_ARGS__)
#define _MAC_TAKE_7(_1, ...) _1,  _MAC_TAKE_6(__VA_ARGS__)
#define _MAC_TAKE_8(_1, ...) _1,  _MAC_TAKE_7(__VA_ARGS__)
#define _MAC_TAKE_9(_1, ...) _1,  _MAC_TAKE_8(__VA_ARGS__)
#define _MAC_TAKE_10(_1, ...) _1,  _MAC_TAKE_9(__VA_ARGS__)
#define _MAC_TAKE_11(_1, ...) _1, _MAC_TAKE_10(__VA_ARGS__)
#define _MAC_TAKE_12(_1, ...) _1, _MAC_TAKE_11(__VA_ARGS__)
#define _MAC_TAKE_13(_1, ...) _1, _MAC_TAKE_12(__VA_ARGS__)
#define _MAC_TAKE_14(_1, ...) _1, _MAC_TAKE_13(__VA_ARGS__)
#define _MAC_TAKE_15(_1, ...) _1, _MAC_TAKE_14(__VA_ARGS__)
#define MAC_TAKE(N, ...) MAC_CONCAT(_MAC_TAKE_, N)(__VA_ARGS__,)

#define _MAC_CHOOSE_(X, ...) X
#define _MAC_CHOOSE(...) _MAC_CHOOSE_(__VA_ARGS__,)
#define MAC_CHOOSE(N, ...) _MAC_CHOOSE(MAC_SKIP(N, __VA_ARGS__,))

#define MAC_FIRST(...) MAC_CHOOSE(0, __VA_ARGS__,)
#define MAC_ALLBUTFIRST(...) MAC_SKIP(1, __VA_ARGS__,)
#define MAC_LAST(...) MAC_CHOOSE(MAC_PRED(MAC_ARGCOUNT(__VA_ARGS__)), __VA_ARGS__,)
#define MAC_ALLBUTLAST(...) MAC_TAKE(MAC_PRED(MAC_ARGCOUNT(__VA_ARGS__)), __VA_ARGS__,)

#define _MAC_FOR_0(NAME, OP, FUNC, ...)
#define _MAC_FOR_1(NAME, OP, FUNC, ...) FUNC(NAME, MAC_FIRST(__VA_ARGS__), 0)
#define _MAC_FOR_2(NAME, OP, FUNC, ...) OP(NAME,  1,  _MAC_FOR_1(NAME, OP, FUNC, MAC_ALLBUTLAST(__VA_ARGS__)), FUNC(NAME, MAC_LAST(__VA_ARGS__),  1))
#define _MAC_FOR_3(NAME, OP, FUNC, ...) OP(NAME,  2,  _MAC_FOR_2(NAME, OP, FUNC, MAC_ALLBUTLAST(__VA_ARGS__)), FUNC(NAME, MAC_LAST(__VA_ARGS__),  2))
#define _MAC_FOR_4(NAME, OP, FUNC, ...) OP(NAME,  3,  _MAC_FOR_3(NAME, OP, FUNC, MAC_ALLBUTLAST(__VA_ARGS__)), FUNC(NAME, MAC_LAST(__VA_ARGS__),  3))
#define _MAC_FOR_5(NAME, OP, FUNC, ...) OP(NAME,  4,  _MAC_FOR_4(NAME, OP, FUNC, MAC_ALLBUTLAST(__VA_ARGS__)), FUNC(NAME, MAC_LAST(__VA_ARGS__),  4))
#define _MAC_FOR_6(NAME, OP, FUNC, ...) OP(NAME,  5,  _MAC_FOR_5(NAME, OP, FUNC, MAC_ALLBUTLAST(__VA_ARGS__)), FUNC(NAME, MAC_LAST(__VA_ARGS__),  5))
#define _MAC_FOR_7(NAME, OP, FUNC, ...) OP(NAME,  6,  _MAC_FOR_6(NAME, OP, FUNC, MAC_ALLBUTLAST(__VA_ARGS__)), FUNC(NAME, MAC_LAST(__VA_ARGS__),  6))
#define _MAC_FOR_8(NAME, OP, FUNC, ...) OP(NAME,  7,  _MAC_FOR_7(NAME, OP, FUNC, MAC_ALLBUTLAST(__VA_ARGS__)), FUNC(NAME, MAC_LAST(__VA_ARGS__),  7))
#define _MAC_FOR_9(NAME, OP, FUNC, ...) OP(NAME,  8,  _MAC_FOR_8(NAME, OP, FUNC, MAC_ALLBUTLAST(__VA_ARGS__)), FUNC(NAME, MAC_LAST(__VA_ARGS__),  8))
#define _MAC_FOR_10(NAME, OP, FUNC, ...) OP(NAME,  9,  _MAC_FOR_9(NAME, OP, FUNC, MAC_ALLBUTLAST(__VA_ARGS__)), FUNC(NAME, MAC_LAST(__VA_ARGS__),  9))
#define _MAC_FOR_11(NAME, OP, FUNC, ...) OP(NAME, 10, _MAC_FOR_10(NAME, OP, FUNC, MAC_ALLBUTLAST(__VA_ARGS__)), FUNC(NAME, MAC_LAST(__VA_ARGS__), 10))
#define _MAC_FOR_12(NAME, OP, FUNC, ...) OP(NAME, 11, _MAC_FOR_11(NAME, OP, FUNC, MAC_ALLBUTLAST(__VA_ARGS__)), FUNC(NAME, MAC_LAST(__VA_ARGS__), 11))
#define _MAC_FOR_13(NAME, OP, FUNC, ...) OP(NAME, 12, _MAC_FOR_12(NAME, OP, FUNC, MAC_ALLBUTLAST(__VA_ARGS__)), FUNC(NAME, MAC_LAST(__VA_ARGS__), 12))
#define _MAC_FOR_14(NAME, OP, FUNC, ...) OP(NAME, 13, _MAC_FOR_13(NAME, OP, FUNC, MAC_ALLBUTLAST(__VA_ARGS__)), FUNC(NAME, MAC_LAST(__VA_ARGS__), 13))
#define _MAC_FOR_15(NAME, OP, FUNC, ...) OP(NAME, 14, _MAC_FOR_14(NAME, OP, FUNC, MAC_ALLBUTLAST(__VA_ARGS__)), FUNC(NAME, MAC_LAST(__VA_ARGS__), 14))
#define MAC_FOR(NAME, N, OP, FUNC, ...) MAC_CONCAT(_MAC_FOR_, N)(NAME, OP, FUNC, MAC_TAKE(N, __VA_ARGS__))
#define MAC_FOREACH(NAME, OP, FUNC, ...) MAC_FOR(NAME, MAC_ARGCOUNT(__VA_ARGS__), OP, FUNC, __VA_ARGS__)

#define MAC_FOR_OP_SPA(NAME, ITER, PREV, CURR) PREV CURR
#define MAC_FOR_OP_SEQ(NAME, ITER, PREV, CURR) PREV, CURR
#define MAC_FOR_OP_SEP(NAME, ITER, PREV, CURR) PREV; CURR
#define MAC_FOR_OP_NAME(NAME, ITER, PREV, CURR) PREV NAME CURR

#define MAC_FOR_FUNC_DECLVAR(NAME, ARG, ITER)  NAME ARG
#define MAC_FOR_FUNC_V_ASSIGN(NAME, ARG, ITER) Result.ARG = ARG
#define MAC_FOR_FUNC_V_OP(NAME, ARG, ITER)     Result.ARG = NAME V.ARG
#define MAC_FOR_FUNC_VV_OP(NAME, ARG, ITER)    Result.ARG = A.ARG NAME B.ARG
#define MAC_FOR_FUNC_VS_OP(NAME, ARG, ITER)    Result.ARG = V.ARG NAME S
#define MAC_FOR_FUNC_VV_EQ(NAME, ARG, ITER)    A.ARG == B.ARG
#define MAC_FOR_FUNC_VEC_DOT(NAME, ARG, ITER)  (B.ARG*A.ARG)
#define MAC_FOR_FUNC_CLAMP(NAME, ARG, ITER)    Result.ARG = NAME##_Clamp(V.ARG, S, E)
#define MAC_FOR_FUNC_LERP(NAME, ARG, ITER)     NAME##_Lerp(A.ARG, B.ARG, T)
