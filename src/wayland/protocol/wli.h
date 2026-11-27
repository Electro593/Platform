/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
*                                                                             *
*  Author: Aria Seiler                                                        *
*                                                                             *
*  This program is in the public domain. There is no implied warranty, so use *
*  it at your own risk.                                                       *
*                                                                             *
\* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#pragma once

// Helpers

#define WLI_SPACE(...)
#define WLI_COMMA(...) ,

// clang-format off
#define _WLI_COUNT_(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, I, ...) I
#define _WLI_COUNT(...) _WLI_COUNT_(__VA_ARGS__, 31, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, )
#define WLI_IS_LIST(...) _WLI_COUNT_(__VA_ARGS__, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, )
#define WLI_PAD(...)   __VA_ARGS__, , , , , , , , , , , , , , , , , , , , , , , , , , , , , , ,
// clang-format on

#define _WLI_IS_EMPTY_CHECK_0001 ,
#define _WLI_IS_EMPTY_CHECK_(_0, _1, _2, _3)          \
	WLI_IS_LIST(_WLI_IS_EMPTY_CHECK_##_0##_1##_2##_3)
#define _WLI_IS_EMPTY_CHECK(_0, _1, _2, _3) _WLI_IS_EMPTY_CHECK_(_0, _1, _2, _3)
#define WLI_IS_EMPTY(...)                    \
	_WLI_IS_EMPTY_CHECK(                     \
		WLI_IS_LIST(__VA_ARGS__),            \
		WLI_IS_LIST(WLI_COMMA __VA_ARGS__),  \
		WLI_IS_LIST(__VA_ARGS__()),          \
		WLI_IS_LIST(WLI_COMMA __VA_ARGS__()) \
	)

#define _WLI_IS_WRAPPED_CHECK_(_, X, ...) X
#define _WLI_IS_WRAPPED_CHECK(...) _WLI_IS_WRAPPED_CHECK_(__VA_ARGS__, 0, )
#define _WLI_IS_WRAPPED_PROBE(...) ~, 1,
#define WLI_IS_WRAPPED(Value) _WLI_IS_WRAPPED_CHECK(_WLI_IS_WRAPPED_PROBE Value)

#define _WLI_EQ_0_0				  ~, 1
#define _WLI_EQ_1_1				  ~, 1
#define _WLI_EQ_2_2				  ~, 1
#define _WLI_EQ_3_3				  ~, 1
#define _WLI_EQ_4_4				  ~, 1
#define _WLI_EQ_5_5				  ~, 1
#define _WLI_EQ_6_6				  ~, 1
#define _WLI_EQ_7_7				  ~, 1
#define _WLI_EQ_8_8				  ~, 1
#define _WLI_EQ_9_9				  ~, 1
#define _WLI_EQ_10_10			  ~, 1
#define _WLI_EQ_11_11			  ~, 1
#define _WLI_EQ_12_12			  ~, 1
#define _WLI_EQ_13_13			  ~, 1
#define _WLI_EQ_14_14			  ~, 1
#define _WLI_EQ_15_15			  ~, 1
#define _WLI_EQ_16_16			  ~, 1
#define _WLI_EQ_17_17			  ~, 1
#define _WLI_EQ_18_18			  ~, 1
#define _WLI_EQ_19_19			  ~, 1
#define _WLI_EQ_20_20			  ~, 1
#define _WLI_EQ_21_21			  ~, 1
#define _WLI_EQ_22_22			  ~, 1
#define _WLI_EQ_23_23			  ~, 1
#define _WLI_EQ_24_24			  ~, 1
#define _WLI_EQ_25_25			  ~, 1
#define _WLI_EQ_26_26			  ~, 1
#define _WLI_EQ_27_27			  ~, 1
#define _WLI_EQ_28_28			  ~, 1
#define _WLI_EQ_29_29			  ~, 1
#define _WLI_EQ_30_30			  ~, 1
#define _WLI_EQ_31_31			  ~, 1
#define _WLI_EQ_(A, B)			  _WLI_EQ_##A##_##B
#define _WLI_EQ_CHECK_(_, X, ...) X
#define _WLI_EQ_CHECK(...)		  _WLI_EQ_CHECK_(__VA_ARGS__, 0, )
#define WLI_EQ(A, B)			  _WLI_EQ_CHECK(_WLI_EQ_(A, B))

#define _WLI_IF_0(Then, Else) Else
#define _WLI_IF_1(Then, Else) Then
#define _WLI_IF_(Cond)		  _WLI_IF_##Cond
#define _WLI_IF(Cond)		  _WLI_IF_(Cond)

#define WLI_IF_EQ(X, ...)	_WLI_IF(WLI_EQ(X, __VA_ARGS__))
#define WLI_IF_EMPTY(...)	_WLI_IF(WLI_IS_EMPTY(__VA_ARGS__))
#define WLI_IF_WRAPPED(...) _WLI_IF(WLI_IS_WRAPPED(__VA_ARGS__))
#define WLI_IF_LIST(...)	_WLI_IF(WLI_IS_LIST(__VA_ARGS__))

#define _WLI_BOOL_FALSE(...) 0
#define _WLI_BOOL_TRUE(...)	 1
#define _WLI_BOOL_NONEMPTY(X)                           \
	WLI_IF_EQ(0, X)(_WLI_BOOL_FALSE, _WLI_BOOL_TRUE)(X)
#define _WLI_BOOL_TOKEN(X)                                  \
	WLI_IF_EMPTY(X)(_WLI_BOOL_FALSE, _WLI_BOOL_NONEMPTY)(X)
#define _WLI_BOOL_SINGLE(X)                               \
	WLI_IF_WRAPPED(X)(_WLI_BOOL_TRUE, _WLI_BOOL_TOKEN)(X)
#define WLI_BOOL(...)                                                       \
	WLI_IF_LIST(__VA_ARGS__)(_WLI_BOOL_TRUE, _WLI_BOOL_SINGLE)(__VA_ARGS__)

#define WLI_NONE(...)
#define WLI_ALL(...) __VA_ARGS__

#define WLI_IF(...)		_WLI_IF(WLI_BOOL(__VA_ARGS__))
#define WLI_WHEN(...)	WLI_IF(__VA_ARGS__)(WLI_ALL, WLI_NONE)
#define WLI_UNLESS(...) WLI_IF(__VA_ARGS__)(WLI_NONE, WLI_ALL)

#define WLI_NOT(...)  WLI_IF(__VA_ARGS__)(0, 1)
#define WLI_NEQ(A, B) WLI_NOT(WLI_EQ(A, B))
#define WLI_OR(A)	  WLI_IF(A)(_WLI_BOOL_TRUE, WLI_ALL)
#define WLI_AND(A)	  WLI_IF(A)(WLI_ALL, _WLI_BOOL_FALSE)

#define _WLI_THEN_KEEP(...) __VA_ARGS__ WLI_NONE
#define _WLI_THEN_SKIP(...) WLI_ALL
#define WLI_THEN(...)                                                \
	WLI_IF(__VA_ARGS__)(_WLI_THEN_SKIP, _WLI_THEN_KEEP)(__VA_ARGS__)
#define WLI_ELSE(...)                                                \
	WLI_IF(__VA_ARGS__)(_WLI_THEN_KEEP, _WLI_THEN_SKIP)(__VA_ARGS__)

#define WLI_LEN(...) WLI_IF_EMPTY(__VA_ARGS__)(0, _WLI_COUNT(, __VA_ARGS__))
#define WLI_IS_SINGLE(...) WLI_EQ(1, WLI_LEN(__VA_ARGS__))

#define _WLI_LENCALL(Prefix, Len, ...) Prefix##Len
#define WLI_LENCALL(Prefix, ...)                            \
	_WLI_LENCALL(Prefix, WLI_LEN(__VA_ARGS__))(__VA_ARGS__)

#define _WLI_STR(...) #__VA_ARGS__
#define WLI_STR(...)  _WLI_STR(__VA_ARGS__)

#define _WLI_CAT_0()
#define _WLI_CAT_1(_0)			 _0
#define _WLI_CAT_2(_0, _1)		 _0##_1
#define _WLI_CAT_3(_0, _1, ...)	 _WLI_CAT2(_0##_1, __VA_ARGS__)
#define _WLI_CAT_4(_0, _1, ...)	 _WLI_CAT3(_0##_1, __VA_ARGS__)
#define _WLI_CAT_5(_0, _1, ...)	 _WLI_CAT4(_0##_1, __VA_ARGS__)
#define _WLI_CAT_6(_0, _1, ...)	 _WLI_CAT5(_0##_1, __VA_ARGS__)
#define _WLI_CAT_7(_0, _1, ...)	 _WLI_CAT6(_0##_1, __VA_ARGS__)
#define _WLI_CAT_8(_0, _1, ...)	 _WLI_CAT7(_0##_1, __VA_ARGS__)
#define _WLI_CAT_9(_0, _1, ...)	 _WLI_CAT8(_0##_1, __VA_ARGS__)
#define _WLI_CAT_10(_0, _1, ...) _WLI_CAT9(_0##_1, __VA_ARGS__)
#define _WLI_CAT_11(_0, _1, ...) _WLI_CAT10(_0##_1, __VA_ARGS__)
#define _WLI_CAT_12(_0, _1, ...) _WLI_CAT11(_0##_1, __VA_ARGS__)
#define _WLI_CAT_13(_0, _1, ...) _WLI_CAT12(_0##_1, __VA_ARGS__)
#define _WLI_CAT_14(_0, _1, ...) _WLI_CAT13(_0##_1, __VA_ARGS__)
#define _WLI_CAT_15(_0, _1, ...) _WLI_CAT14(_0##_1, __VA_ARGS__)
#define _WLI_CAT_16(_0, _1, ...) _WLI_CAT15(_0##_1, __VA_ARGS__)
#define _WLI_CAT_17(_0, _1, ...) _WLI_CAT16(_0##_1, __VA_ARGS__)
#define _WLI_CAT_18(_0, _1, ...) _WLI_CAT17(_0##_1, __VA_ARGS__)
#define _WLI_CAT_19(_0, _1, ...) _WLI_CAT18(_0##_1, __VA_ARGS__)
#define _WLI_CAT_20(_0, _1, ...) _WLI_CAT19(_0##_1, __VA_ARGS__)
#define _WLI_CAT_21(_0, _1, ...) _WLI_CAT20(_0##_1, __VA_ARGS__)
#define _WLI_CAT_22(_0, _1, ...) _WLI_CAT21(_0##_1, __VA_ARGS__)
#define _WLI_CAT_23(_0, _1, ...) _WLI_CAT22(_0##_1, __VA_ARGS__)
#define _WLI_CAT_24(_0, _1, ...) _WLI_CAT23(_0##_1, __VA_ARGS__)
#define _WLI_CAT_25(_0, _1, ...) _WLI_CAT24(_0##_1, __VA_ARGS__)
#define _WLI_CAT_26(_0, _1, ...) _WLI_CAT25(_0##_1, __VA_ARGS__)
#define _WLI_CAT_27(_0, _1, ...) _WLI_CAT26(_0##_1, __VA_ARGS__)
#define _WLI_CAT_28(_0, _1, ...) _WLI_CAT27(_0##_1, __VA_ARGS__)
#define _WLI_CAT_29(_0, _1, ...) _WLI_CAT28(_0##_1, __VA_ARGS__)
#define _WLI_CAT_30(_0, _1, ...) _WLI_CAT29(_0##_1, __VA_ARGS__)
#define _WLI_CAT_31(_0, _1, ...) _WLI_CAT30(_0##_1, __VA_ARGS__)
#define _WLI_CAT_(N, ...)		 _WLI_CAT_##N(__VA_ARGS__)
#define _WLI_CAT(N, ...)		 _WLI_CAT_(N, __VA_ARGS__)
#define WLI_CAT(...)			 _WLI_CAT(WLI_LEN(__VA_ARGS__), __VA_ARGS__)

#define _WLI_INC_0	1
#define _WLI_INC_1	2
#define _WLI_INC_2	3
#define _WLI_INC_3	4
#define _WLI_INC_4	5
#define _WLI_INC_5	6
#define _WLI_INC_6	7
#define _WLI_INC_7	8
#define _WLI_INC_8	9
#define _WLI_INC_9	10
#define _WLI_INC_10 11
#define _WLI_INC_11 12
#define _WLI_INC_12 13
#define _WLI_INC_13 14
#define _WLI_INC_14 15
#define _WLI_INC_15 16
#define _WLI_INC_16 17
#define _WLI_INC_17 18
#define _WLI_INC_18 19
#define _WLI_INC_19 20
#define _WLI_INC_20 21
#define _WLI_INC_21 22
#define _WLI_INC_22 23
#define _WLI_INC_23 24
#define _WLI_INC_24 25
#define _WLI_INC_25 26
#define _WLI_INC_26 27
#define _WLI_INC_27 28
#define _WLI_INC_28 29
#define _WLI_INC_29 30
#define _WLI_INC_30 31
#define _WLI_INC_31 32
#define WLI_INC(N)	WLI_CAT(_WLI_INC_, N)

#define _WLI_SKIP_0(...)	  __VA_ARGS__
#define _WLI_SKIP_1(_0, ...)  _WLI_SKIP_0(__VA_ARGS__)
#define _WLI_SKIP_2(_0, ...)  _WLI_SKIP_1(__VA_ARGS__)
#define _WLI_SKIP_3(_0, ...)  _WLI_SKIP_2(__VA_ARGS__)
#define _WLI_SKIP_4(_0, ...)  _WLI_SKIP_3(__VA_ARGS__)
#define _WLI_SKIP_5(_0, ...)  _WLI_SKIP_4(__VA_ARGS__)
#define _WLI_SKIP_6(_0, ...)  _WLI_SKIP_5(__VA_ARGS__)
#define _WLI_SKIP_7(_0, ...)  _WLI_SKIP_6(__VA_ARGS__)
#define _WLI_SKIP_8(_0, ...)  _WLI_SKIP_7(__VA_ARGS__)
#define _WLI_SKIP_9(_0, ...)  _WLI_SKIP_8(__VA_ARGS__)
#define _WLI_SKIP_10(_0, ...) _WLI_SKIP_9(__VA_ARGS__)
#define _WLI_SKIP_11(_0, ...) _WLI_SKIP_10(__VA_ARGS__)
#define _WLI_SKIP_12(_0, ...) _WLI_SKIP_11(__VA_ARGS__)
#define _WLI_SKIP_13(_0, ...) _WLI_SKIP_12(__VA_ARGS__)
#define _WLI_SKIP_14(_0, ...) _WLI_SKIP_13(__VA_ARGS__)
#define _WLI_SKIP_15(_0, ...) _WLI_SKIP_14(__VA_ARGS__)
#define _WLI_SKIP_16(_0, ...) _WLI_SKIP_15(__VA_ARGS__)
#define _WLI_SKIP_17(_0, ...) _WLI_SKIP_16(__VA_ARGS__)
#define _WLI_SKIP_18(_0, ...) _WLI_SKIP_17(__VA_ARGS__)
#define _WLI_SKIP_19(_0, ...) _WLI_SKIP_18(__VA_ARGS__)
#define _WLI_SKIP_20(_0, ...) _WLI_SKIP_19(__VA_ARGS__)
#define _WLI_SKIP_21(_0, ...) _WLI_SKIP_20(__VA_ARGS__)
#define _WLI_SKIP_22(_0, ...) _WLI_SKIP_21(__VA_ARGS__)
#define _WLI_SKIP_23(_0, ...) _WLI_SKIP_22(__VA_ARGS__)
#define _WLI_SKIP_24(_0, ...) _WLI_SKIP_23(__VA_ARGS__)
#define _WLI_SKIP_25(_0, ...) _WLI_SKIP_24(__VA_ARGS__)
#define _WLI_SKIP_26(_0, ...) _WLI_SKIP_25(__VA_ARGS__)
#define _WLI_SKIP_27(_0, ...) _WLI_SKIP_26(__VA_ARGS__)
#define _WLI_SKIP_28(_0, ...) _WLI_SKIP_27(__VA_ARGS__)
#define _WLI_SKIP_29(_0, ...) _WLI_SKIP_28(__VA_ARGS__)
#define _WLI_SKIP_30(_0, ...) _WLI_SKIP_29(__VA_ARGS__)
#define _WLI_SKIP_31(_0, ...) _WLI_SKIP_30(__VA_ARGS__)
#define WLI_SKIP(N, ...)	  WLI_CAT(_WLI_SKIP_, N)(__VA_ARGS__)

#define _WLI_TAKE_0(...)
#define _WLI_TAKE_1(_0, ...)  _0
#define _WLI_TAKE_2(_0, ...)  _0, _WLI_TAKE_1(__VA_ARGS__)
#define _WLI_TAKE_3(_0, ...)  _0, _WLI_TAKE_2(__VA_ARGS__)
#define _WLI_TAKE_4(_0, ...)  _0, _WLI_TAKE_3(__VA_ARGS__)
#define _WLI_TAKE_5(_0, ...)  _0, _WLI_TAKE_4(__VA_ARGS__)
#define _WLI_TAKE_6(_0, ...)  _0, _WLI_TAKE_5(__VA_ARGS__)
#define _WLI_TAKE_7(_0, ...)  _0, _WLI_TAKE_6(__VA_ARGS__)
#define _WLI_TAKE_8(_0, ...)  _0, _WLI_TAKE_7(__VA_ARGS__)
#define _WLI_TAKE_9(_0, ...)  _0, _WLI_TAKE_8(__VA_ARGS__)
#define _WLI_TAKE_10(_0, ...) _0, _WLI_TAKE_9(__VA_ARGS__)
#define _WLI_TAKE_11(_0, ...) _0, _WLI_TAKE_10(__VA_ARGS__)
#define _WLI_TAKE_12(_0, ...) _0, _WLI_TAKE_11(__VA_ARGS__)
#define _WLI_TAKE_13(_0, ...) _0, _WLI_TAKE_12(__VA_ARGS__)
#define _WLI_TAKE_14(_0, ...) _0, _WLI_TAKE_13(__VA_ARGS__)
#define _WLI_TAKE_15(_0, ...) _0, _WLI_TAKE_14(__VA_ARGS__)
#define _WLI_TAKE_16(_0, ...) _0, _WLI_TAKE_15(__VA_ARGS__)
#define _WLI_TAKE_17(_0, ...) _0, _WLI_TAKE_16(__VA_ARGS__)
#define _WLI_TAKE_18(_0, ...) _0, _WLI_TAKE_17(__VA_ARGS__)
#define _WLI_TAKE_19(_0, ...) _0, _WLI_TAKE_18(__VA_ARGS__)
#define _WLI_TAKE_20(_0, ...) _0, _WLI_TAKE_19(__VA_ARGS__)
#define _WLI_TAKE_21(_0, ...) _0, _WLI_TAKE_20(__VA_ARGS__)
#define _WLI_TAKE_22(_0, ...) _0, _WLI_TAKE_21(__VA_ARGS__)
#define _WLI_TAKE_23(_0, ...) _0, _WLI_TAKE_22(__VA_ARGS__)
#define _WLI_TAKE_24(_0, ...) _0, _WLI_TAKE_23(__VA_ARGS__)
#define _WLI_TAKE_25(_0, ...) _0, _WLI_TAKE_24(__VA_ARGS__)
#define _WLI_TAKE_26(_0, ...) _0, _WLI_TAKE_25(__VA_ARGS__)
#define _WLI_TAKE_27(_0, ...) _0, _WLI_TAKE_26(__VA_ARGS__)
#define _WLI_TAKE_28(_0, ...) _0, _WLI_TAKE_27(__VA_ARGS__)
#define _WLI_TAKE_29(_0, ...) _0, _WLI_TAKE_28(__VA_ARGS__)
#define _WLI_TAKE_30(_0, ...) _0, _WLI_TAKE_29(__VA_ARGS__)
#define _WLI_TAKE_31(_0, ...) _0, _WLI_TAKE_30(__VA_ARGS__)
#define WLI_TAKE(N, ...)	  WLI_CAT(_WLI_TAKE_, N)(__VA_ARGS__)

#define WLI_HEAD(...)  WLI_TAKE(1, __VA_ARGS__)
#define WLI_REST(...)  WLI_SKIP(1, __VA_ARGS__)
#define WLI_AT(N, ...) WLI_HEAD(WLI_SKIP(N, WLI_PAD(__VA_ARGS__)))

// Iterators

#define _WLI_IM_P		 ()
#define _WLI_IM_E0(...)	 __VA_ARGS__
#define _WLI_IM_E2(...)	 _WLI_IM_E0(_WLI_IM_E0(__VA_ARGS__))
#define _WLI_IM_E4(...)	 _WLI_IM_E2(_WLI_IM_E2(__VA_ARGS__))
#define _WLI_IM_E8(...)	 _WLI_IM_E4(_WLI_IM_E4(__VA_ARGS__))
#define _WLI_IM_E16(...) _WLI_IM_E8(_WLI_IM_E8(__VA_ARGS__))
#define _WLI_IM_E(...)	 _WLI_IM_E16(_WLI_IM_E16(__VA_ARGS__))
#define _WLI_IM_4(...)
#define _WLI_IM_3() _WLI_IM_1
#define _WLI_IM_2(C, F, S, I, ...)                            \
	S() _WLI_IM_3 _WLI_IM_P(C, F, S, WLI_INC(I), __VA_ARGS__)
#define _WLI_IM_1(C, F, S, I, A, ...)                                        \
	F(C, A, I)                                                               \
	WLI_IF_EMPTY(__VA_ARGS__)(_WLI_IM_4, _WLI_IM_2)(C, F, S, I, __VA_ARGS__)
#define _WLI_IM_0(C, F, S, I, ...)                                           \
	WLI_IF_EMPTY(__VA_ARGS__)(_WLI_IM_4, _WLI_IM_1)(C, F, S, I, __VA_ARGS__)
#define _WLI_IM(C, F, S, ...) _WLI_IM_E(_WLI_IM_0(C, F, S, 0, __VA_ARGS__))

#define WLI_MAP_METHODS(Interface, Mapper, Separator, ...) \
	_WLI_IM(Interface, Mapper, Separator, __VA_ARGS__)

#define _WLI_IM_F(C, A, I) WLI_WHEN(C(A))(, A)
#define WLI_FILTER_METHODS(Condition, ...)                          \
	WLI_REST(_WLI_IM(Condition, _WLI_IM_F, WLI_SPACE, __VA_ARGS__))

#define _WLI_IA_P		 ()
#define _WLI_IA_E0(...)	 __VA_ARGS__
#define _WLI_IA_E2(...)	 _WLI_IA_E0(_WLI_IA_E0(__VA_ARGS__))
#define _WLI_IA_E4(...)	 _WLI_IA_E2(_WLI_IA_E2(__VA_ARGS__))
#define _WLI_IA_E8(...)	 _WLI_IA_E4(_WLI_IA_E4(__VA_ARGS__))
#define _WLI_IA_E16(...) _WLI_IA_E8(_WLI_IA_E8(__VA_ARGS__))
#define _WLI_IA_E(...)	 _WLI_IA_E16(_WLI_IA_E16(__VA_ARGS__))
#define _WLI_IA_4(...)
#define _WLI_IA_3() _WLI_IA_1
#define _WLI_IA_2(C, F, S, I, ...)                            \
	S() _WLI_IA_3 _WLI_IA_P(C, F, S, WLI_INC(I), __VA_ARGS__)
#define _WLI_IA_1(C, F, S, I, A, ...)                                        \
	F(C, A, I)                                                               \
	WLI_IF_EMPTY(__VA_ARGS__)(_WLI_IA_4, _WLI_IA_2)(C, F, S, I, __VA_ARGS__)
#define _WLI_IA_0(C, F, S, I, ...)                                           \
	WLI_IF_EMPTY(__VA_ARGS__)(_WLI_IA_4, _WLI_IA_1)(C, F, S, I, __VA_ARGS__)
#define _WLI_IA(C, F, S, ...) _WLI_IA_E(_WLI_IA_0(C, F, S, 0, __VA_ARGS__))

#define _WLI_IA_M2_(C, ...) C(__VA_ARGS__)
#define _WLI_IA_M_(...)		_WLI_IA_M2_(__VA_ARGS__)
#define _WLI_IA_M(C, A, I)	_WLI_IA_M_(WLI_ALL C, A)
#define WLI_MAP_ARGS(Interface, Method, Mapper, Separator, ...)             \
	_WLI_IA((Mapper, Interface, Method), _WLI_IA_M, Separator, __VA_ARGS__)

#define _WLI_IA_F(C, A, I) WLI_WHEN(C(A))(, A)
#define WLI_FILTER_ARGS(Condition, ...)                             \
	WLI_REST(_WLI_IA(Condition, _WLI_IA_F, WLI_SPACE, __VA_ARGS__))

// Constants

#define WLI_THIS
#define WLI_INTERFACE ((WaylandInterface, wayland_interface, 0, 1))

// Dispatch Operators

#define _WLI_METHOD_KIND_R(...) R
#define _WLI_METHOD_KIND_D(...) D
#define _WLI_METHOD_KIND_E(...) E
#define _WLI_METHOD_KIND_C(...) C
#define WLI_METHOD_KIND(Method) WLI_CAT(_WLI_METHOD_KIND_, Method)

#define _WLI_UNWRAP_METHOD_R(...) (__VA_ARGS__)
#define _WLI_UNWRAP_METHOD_D(...) (__VA_ARGS__)
#define _WLI_UNWRAP_METHOD_E(...) (__VA_ARGS__)
#define _WLI_UNWRAP_METHOD_C(...) (__VA_ARGS__)
#define WLI_UNWRAP_METHOD(Method) WLI_CAT(_WLI_UNWRAP_METHOD_, Method)

#define _WLI_METHOD_IS_REQUEST_R(...) 1
#define _WLI_METHOD_IS_REQUEST_D(...) 1
#define _WLI_METHOD_IS_REQUEST_E(...) 0
#define _WLI_METHOD_IS_REQUEST_C(...) 0
#define WLI_METHOD_IS_REQUEST(Method) WLI_CAT(_WLI_METHOD_IS_REQUEST_, Method)
#define WLI_METHOD_IS_EVENT(Method)	  WLI_NOT(WLI_METHOD_IS_REQUEST(Method))

#define _WLI_METHOD_THIS_R(...) O(This)
#define _WLI_METHOD_THIS_D(...) D(This)
#define _WLI_METHOD_THIS_E(...) O(This)
#define _WLI_METHOD_THIS_C(...) D(This)
#define WLI_METHOD_THIS(Method) WLI_CAT(_WLI_METHOD_THIS_, Method)

#define _WLI_ARG_KIND_I(...) I
#define _WLI_ARG_KIND_U(...) U
#define _WLI_ARG_KIND_R(...) R
#define _WLI_ARG_KIND_F(...) F
#define _WLI_ARG_KIND_S(...) S
#define _WLI_ARG_KIND_A(...) A
#define _WLI_ARG_KIND_N(...) N
#define _WLI_ARG_KIND_O(...) O
#define _WLI_ARG_KIND_D(...) D
#define WLI_ARG_KIND(Arg)	 WLI_CAT(_WLI_ARG_KIND_, Arg)

#define _WLI_UNWRAP_ARG_I(...) (__VA_ARGS__)
#define _WLI_UNWRAP_ARG_U(...) (__VA_ARGS__)
#define _WLI_UNWRAP_ARG_R(...) (__VA_ARGS__)
#define _WLI_UNWRAP_ARG_F(...) (__VA_ARGS__)
#define _WLI_UNWRAP_ARG_S(...) (__VA_ARGS__)
#define _WLI_UNWRAP_ARG_A(...) (__VA_ARGS__)
#define _WLI_UNWRAP_ARG_N(...) (__VA_ARGS__)
#define _WLI_UNWRAP_ARG_O(...) (__VA_ARGS__)
#define _WLI_UNWRAP_ARG_D(...) (__VA_ARGS__)
#define WLI_UNWRAP_ARG(Arg)	   WLI_CAT(_WLI_UNWRAP_ARG_, Arg)

#define WLI_ARG_DISPATCH(Prefix, Arg, ...) \
	WLI_CAT(Prefix, WLI_ARG_KIND(Arg))(__VA_ARGS__, WLI_ALL WLI_UNWRAP_ARG(Arg))

#define _WLI_ARG_IS_CTOR_I(...)	 0
#define _WLI_ARG_IS_CTOR_U(...)	 0
#define _WLI_ARG_IS_CTOR_R(...)	 0
#define _WLI_ARG_IS_CTOR_F(...)	 0
#define _WLI_ARG_IS_CTOR_S(...)	 0
#define _WLI_ARG_IS_CTOR_A(...)	 0
#define _WLI_ARG_IS_CTOR_N(...)	 1
#define _WLI_ARG_IS_CTOR_O(...)	 0
#define _WLI_ARG_IS_CTOR_D(...)	 0
#define WLI_ARG_IS_CTOR(Arg)	 WLI_ARG_DISPATCH(_WLI_ARG_IS_CTOR_, Arg)
#define WLI_ARG_IS_NOT_CTOR(Arg) WLI_NOT(WLI_ARG_IS_CTOR(Arg))

#define _WLI_ARG_FORMAT_NULLABLE(...) WLI_WHEN(WLI_AT(3, __VA_ARGS__))("?")
#define _WLI_ARG_FORMAT_I(...)		  "i"
#define _WLI_ARG_FORMAT_U(...)		  "u"
#define _WLI_ARG_FORMAT_R(...)		  "r"
#define _WLI_ARG_FORMAT_F(...)		  "f"
#define _WLI_ARG_FORMAT_S(...)		  "s" _WLI_ARG_FORMAT_NULLABLE(__VA_ARGS__)
#define _WLI_ARG_FORMAT_A(...)		  "a" _WLI_ARG_FORMAT_NULLABLE(__VA_ARGS__)
#define _WLI_ARG_FORMAT_N(...)		  "n"
#define _WLI_ARG_FORMAT_O(...)		  "o" _WLI_ARG_FORMAT_NULLABLE(__VA_ARGS__)
#define _WLI_ARG_FORMAT_D(...)		  "d" _WLI_ARG_FORMAT_NULLABLE(__VA_ARGS__)
#define WLI_ARG_FORMAT(Arg)			  WLI_ARG_DISPATCH(_WLI_ARG_FORMAT_, Arg)

#define _WLI_ARG_INTERFACE_I(...)
#define _WLI_ARG_INTERFACE_U(...)
#define _WLI_ARG_INTERFACE_R(...)
#define _WLI_ARG_INTERFACE_F(...)
#define _WLI_ARG_INTERFACE_S(...)
#define _WLI_ARG_INTERFACE_A(...)
#define _WLI_ARG_INTERFACE_N(Interface, ...)                                 \
	WLI_ELSE(WLI_AT(WLI_IF(WLI_IS_SINGLE(__VA_ARGS__))(0, 1), __VA_ARGS__))( \
		Interface                                                            \
	)
#define _WLI_ARG_INTERFACE_O(Interface, ...)    \
	WLI_ELSE(WLI_AT(1, __VA_ARGS__))(Interface)
#define _WLI_ARG_INTERFACE_D(Interface, ...)    \
	WLI_ELSE(WLI_AT(1, __VA_ARGS__))(Interface)
#define WLI_ARG_INTERFACE(Interface, Arg)                 \
	WLI_ARG_DISPATCH(_WLI_ARG_INTERFACE_, Arg, Interface)

#define _WLI_ARG_NAME_I(Interface, ...) WLI_HEAD(__VA_ARGS__)
#define _WLI_ARG_NAME_U(Interface, ...) WLI_HEAD(__VA_ARGS__)
#define _WLI_ARG_NAME_R(Interface, ...) WLI_HEAD(__VA_ARGS__)
#define _WLI_ARG_NAME_F(Interface, ...) WLI_HEAD(__VA_ARGS__)
#define _WLI_ARG_NAME_S(Interface, ...) WLI_HEAD(__VA_ARGS__)
#define _WLI_ARG_NAME_A(Interface, ...) WLI_HEAD(__VA_ARGS__)
#define _WLI_ARG_NAME_N(Interface, ...)                                      \
	WLI_ELSE(WLI_UNLESS(WLI_IS_SINGLE(__VA_ARGS__))(WLI_HEAD(__VA_ARGS__)))( \
		WLI_NAME(_WLI_ARG_INTERFACE_N(Interface, __VA_ARGS__))               \
	)
#define _WLI_ARG_NAME_O(Interface, ...) WLI_HEAD(__VA_ARGS__)
#define _WLI_ARG_NAME_D(Interface, ...) WLI_HEAD(__VA_ARGS__)
#define WLI_ARG_NAME(Interface, Arg)                 \
	WLI_ARG_DISPATCH(_WLI_ARG_NAME_, Arg, Interface)

#define _WLI_ARG_TYPE_I(Interface, ...)                              \
	WLI_IF(WLI_AT(1, __VA_ARGS__))(enum WLI_AT(1, __VA_ARGS__), s32)
#define _WLI_ARG_TYPE_U(Interface, ...)                              \
	WLI_IF(WLI_AT(1, __VA_ARGS__))(enum WLI_AT(1, __VA_ARGS__), u32)
#define _WLI_ARG_TYPE_R(...) wayland_fixed
#define _WLI_ARG_TYPE_F(...) wayland_fd
#define _WLI_ARG_TYPE_S(...) wayland_string
#define _WLI_ARG_TYPE_A(...) wayland_array
#define _WLI_ARG_TYPE_N(...) WLI_TYPE(_WLI_ARG_INTERFACE_N(__VA_ARGS__))
#define _WLI_ARG_TYPE_O(...) WLI_TYPE(_WLI_ARG_INTERFACE_O(__VA_ARGS__))
#define _WLI_ARG_TYPE_D(...) WLI_TYPE(_WLI_ARG_INTERFACE_D(__VA_ARGS__))
#define WLI_ARG_TYPE(Interface, Arg)                 \
	WLI_ARG_DISPATCH(_WLI_ARG_TYPE_, Arg, Interface)

// Record Getters

#define WLI_GET_META(Interface)	   WLI_HEAD(WLI_ALL Interface)
#define WLI_GET_METHODS(Interface) WLI_REST(WLI_ALL Interface)
#define WLI_GET_UPPER(Interface)   WLI_AT(0, WLI_ALL WLI_GET_META(Interface))
#define WLI_GET_LOWER(Interface)   WLI_AT(1, WLI_ALL WLI_GET_META(Interface))
#define WLI_GET_VERSION(Interface) WLI_AT(2, WLI_ALL WLI_GET_META(Interface))
#define WLI_GET_DEP(Interface)	   WLI_AT(3, WLI_ALL WLI_GET_META(Interface))

#define WLI_METHOD_GET_META(Method) WLI_HEAD(WLI_ALL WLI_UNWRAP_METHOD(Method))
#define WLI_METHOD_GET_ARGS(Method) WLI_REST(WLI_ALL WLI_UNWRAP_METHOD(Method))
#define WLI_METHOD_GET_NAME(Method)                \
	WLI_AT(0, WLI_ALL WLI_METHOD_GET_META(Method))
#define WLI_METHOD_GET_SINCE(Method)               \
	WLI_AT(1, WLI_ALL WLI_METHOD_GET_META(Method))
#define WLI_METHOD_GET_UNTIL(Method)               \
	WLI_AT(2, WLI_ALL WLI_METHOD_GET_META(Method))

// Utility

#define WLI_NAME(Interface)		   WLI_GET_UPPER(Interface)
#define WLI_TYPE_NAME(Interface)   WLI_GET_LOWER(Interface)
#define WLI_TYPE(Interface)		   struct WLI_TYPE_NAME(Interface) *
#define WLI_PROTO(Interface)	   WLI_CAT(WLI_GET_UPPER(Interface), Prototype)
#define WLI_STR_NAME(Interface)	   WLI_STR(WLI_GET_LOWER(Interface))
#define WLI_MAX_VERSION(Interface) WLI_ELSE(WLI_GET_VERSION(Interface))(1)
#define WLI_FROZEN(Interface)	   WLI_EQ(0, WLI_GET_VERSION(Interface))
#define WLI_DEPRECATED(Interface)  WLI_ELSE(WLI_GET_DEP(Interface))(0)
#define WLI_METHODS(Interface)	   WLI_GET_METHODS(Interface)
#define WLI_REQUESTS(Interface)                                       \
	WLI_FILTER_METHODS(WLI_METHOD_IS_REQUEST, WLI_METHODS(Interface))
#define WLI_EVENTS(Interface)                                       \
	WLI_FILTER_METHODS(WLI_METHOD_IS_EVENT, WLI_METHODS(Interface))

#define WLI_FOREACH_REQUEST(Interface, Mapper, Separator)                  \
	WLI_MAP_METHODS(Interface, Mapper, Separator, WLI_REQUESTS(Interface))
#define WLI_FOREACH_EVENT(Interface, Mapper, Separator)                  \
	WLI_MAP_METHODS(Interface, Mapper, Separator, WLI_EVENTS(Interface))

#define WLI_METHOD_NAME(Method)	 WLI_METHOD_GET_NAME(Method)
#define WLI_METHOD_SINCE(Method) WLI_ELSE(WLI_METHOD_GET_SINCE(Method))(1)
#define WLI_METHOD_UNTIL(Interface, Method)                            \
	WLI_ELSE(WLI_METHOD_GET_UNTIL(Method))(WLI_MAX_VERSION(Interface))
#define WLI_METHOD_ARGS(Method)                                           \
	WLI_METHOD_THIS(Method)                                               \
	WLI_WHEN(WLI_METHOD_GET_ARGS(Method))(, ) WLI_METHOD_GET_ARGS(Method)
#define WLI_METHOD_CTORS(Method)                              \
	WLI_FILTER_ARGS(WLI_ARG_IS_CTOR, WLI_METHOD_ARGS(Method))
#define WLI_METHOD_NON_CTORS(Method)                              \
	WLI_FILTER_ARGS(WLI_ARG_IS_NOT_CTOR, WLI_METHOD_ARGS(Method))
#define WLI_METHOD_IS_SINGLE_CTOR_REQUEST(Method) \
	WLI_AND(WLI_METHOD_IS_REQUEST(Method))(       \
		WLI_IS_SINGLE(WLI_METHOD_CTORS(Method))   \
	)

#define WLI_FOREACH_ARG(Interface, Method, Mapper, Separator) \
	WLI_MAP_ARGS(Interface, Method, Mapper, Separator, WLI_METHOD_ARGS(Method))

// Generators

#define WLI_METHOD_FORMAT_ARG(Interface, Method, Arg) WLI_ARG_FORMAT(Arg)

#define WLI_METHOD_FORMAT(Interface, Method)                             \
	WLI_FOREACH_ARG(Interface, Method, WLI_METHOD_FORMAT_ARG, WLI_SPACE)

#define WLI_EVENT_FUNCTION_POINTER_PARAM(Interface, Method, Arg) \
	WLI_ARG_TYPE(Interface, Arg) WLI_ARG_NAME(Interface, Arg)

#define WLI_EVENT_FUNCTION_POINTER(Interface, Method) \
	void (*WLI_METHOD_NAME(Method))(WLI_FOREACH_ARG(  \
		Interface,                                    \
		Method,                                       \
		WLI_EVENT_FUNCTION_POINTER_PARAM,             \
		WLI_COMMA                                     \
	))

#define WLI_REQUEST_NAME(Interface, Method)               \
	WLI_CAT(WLI_NAME(Interface), WLI_METHOD_NAME(Method))

#define WLI_REQUEST_HANDLER_CALL(Interface, Method, Index, ArgMapper)          \
	Wayland_HandleRequest(                                                     \
		(wayland_interface *)                                                  \
			ArgMapper(Interface, Method, WLI_HEAD(WLI_METHOD_ARGS(Method))),   \
		Index WLI_WHEN(WLI_IS_LIST(WLI_METHOD_ARGS(Method)))(, ) WLI_MAP_ARGS( \
			Interface,                                                         \
			Method,                                                            \
			ArgMapper,                                                         \
			WLI_COMMA,                                                         \
			WLI_REST(WLI_METHOD_ARGS(Method))                                  \
		)                                                                      \
	)

#define WLI_SINGLE_CTOR_REQUEST_HEADER_PARAM(Interface, Method, Arg) \
	WLI_ARG_TYPE(Interface, Arg) WLI_ARG_NAME(Interface, Arg)

#define WLI_SINGLE_CTOR_REQUEST_BODY_PARAM(Interface, Method, Arg) \
	WLI_WHEN(WLI_ARG_IS_CTOR(Arg))(&) WLI_ARG_NAME(Interface, Arg)

#define WLI_SINGLE_CTOR_REQUEST(Interface, Method, Index, Arg)     \
	WLI_ARG_TYPE(Interface, Arg)                                   \
	WLI_REQUEST_NAME(Interface, Method)(WLI_MAP_ARGS(              \
		Interface,                                                 \
		Method,                                                    \
		WLI_SINGLE_CTOR_REQUEST_HEADER_PARAM,                      \
		WLI_COMMA,                                                 \
		WLI_METHOD_NON_CTORS(Method)                               \
	))                                                             \
	{                                                              \
		WLI_ARG_TYPE(Interface, Arg) WLI_ARG_NAME(Interface, Arg); \
		WLI_REQUEST_HANDLER_CALL(                                  \
			Interface,                                             \
			Method,                                                \
			Index,                                                 \
			WLI_SINGLE_CTOR_REQUEST_BODY_PARAM                     \
		);                                                         \
		return WLI_ARG_NAME(Interface, Arg);                       \
	}

#define WLI_MULTI_CTOR_REQUEST_HEADER_PARAM(Interface, Method, Arg) \
	WLI_ARG_TYPE(Interface, Arg)                                    \
	WLI_WHEN(WLI_ARG_IS_CTOR(Arg))(*) WLI_ARG_NAME(Interface, Arg)

#define WLI_MULTI_CTOR_REQUEST_BODY_PARAM(Interface, Method, Arg) \
	WLI_ARG_NAME(Interface, Arg)

#define WLI_MULTI_CTOR_REQUEST(Interface, Method, Index)      \
	void WLI_REQUEST_NAME(Interface, Method)(WLI_FOREACH_ARG( \
		Interface,                                            \
		Method,                                               \
		WLI_MULTI_CTOR_REQUEST_HEADER_PARAM,                  \
		WLI_COMMA                                             \
	))                                                        \
	{                                                         \
		WLI_REQUEST_HANDLER_CALL(                             \
			Interface,                                        \
			Method,                                           \
			Index,                                            \
			WLI_MULTI_CTOR_REQUEST_BODY_PARAM                 \
		);                                                    \
	}

#define WLI_ARG_PROTO_PTR(Interface, Method, Arg) \
	&WLI_PROTO(WLI_ARG_INTERFACE(Interface, Arg))

#define WLI_METHOD_PROTO_LITERAL(Interface, Method, Index)  \
	((wayland_prototype_method){                            \
		.Since		= WLI_METHOD_SINCE(Method),             \
		.Until		= WLI_METHOD_UNTIL(Interface, Method),  \
		.Format		= WLI_METHOD_FORMAT(Interface, Method), \
		.Prototypes = WLI_IF(WLI_METHOD_CTORS(Method))(     \
			((wayland_prototype *[]){ WLI_MAP_ARGS(         \
				Interface,                                  \
				Method,                                     \
				WLI_ARG_PROTO_PTR,                          \
				WLI_COMMA,                                  \
				WLI_METHOD_CTORS(Method)                    \
			) }),                                           \
			NULL                                            \
		),                                                  \
	})

#define WLI_PROTO_LITERAL(Interface)                                       \
	((wayland_prototype){                                                  \
		.Name		  = WLI_STR_NAME(Interface),                           \
		.MaxVersion	  = WLI_MAX_VERSION(Interface),                        \
		.Frozen		  = WLI_FROZEN(Interface),                             \
		.Deprecated	  = WLI_DEPRECATED(Interface),                         \
		.RequestCount = WLI_LEN(WLI_REQUESTS(Interface)),                  \
		.EventCount	  = WLI_LEN(WLI_EVENTS(Interface)),                    \
		.Requests	  = (wayland_prototype_method[]){ WLI_FOREACH_REQUEST( \
			Interface,                                                     \
			WLI_METHOD_PROTO_LITERAL,                                      \
			WLI_COMMA                                                      \
		) },                                                               \
		.Events		  = (wayland_prototype_method[]){ WLI_FOREACH_EVENT(   \
			Interface,                                                     \
			WLI_METHOD_PROTO_LITERAL,                                      \
			WLI_COMMA                                                      \
		) },                                                               \
	})

// Declarators

#define WLI_DECLARE_EVENT(Interface, Method, Index) \
	WLI_EVENT_FUNCTION_POINTER(Interface, Method);

#define WLI_DECLARE_EVENTS(Interface)                          \
	WLI_FOREACH_EVENT(Interface, WLI_DECLARE_EVENT, WLI_SPACE)

#define WLI_DECLARE_STRUCT(Interface)                                 \
	typedef struct WLI_TYPE_NAME(Interface) WLI_TYPE_NAME(Interface);

#define WLI_DEFINE_STRUCT(Interface)            \
	struct WLI_TYPE_NAME(Interface) {           \
		struct wayland_interface_header Header; \
		WLI_DECLARE_EVENTS(Interface)           \
	};

#define WLI_DECLARE_PROTOTYPE(Interface)                  \
	extern struct wayland_prototype WLI_PROTO(Interface);

#define WLI_DEFINE_PROTOTYPE(Interface)             \
	struct wayland_prototype WLI_PROTO(Interface) = \
		WLI_PROTO_LITERAL(Interface);

#define WLI_DEFINE_REQUEST(Interface, Method, Index)                 \
	static inline WLI_IF(WLI_METHOD_IS_SINGLE_CTOR_REQUEST(Method))( \
		WLI_SINGLE_CTOR_REQUEST(                                     \
			Interface,                                               \
			Method,                                                  \
			Index,                                                   \
			WLI_METHOD_CTORS(Method)                                 \
		),                                                           \
		WLI_MULTI_CTOR_REQUEST(Interface, Method, Index)             \
	)

#define WLI_DEFINE_REQUESTS(Interface)                            \
	WLI_FOREACH_REQUEST(Interface, WLI_DEFINE_REQUEST, WLI_SPACE)

#define WLI_DECLARE(Interface)       \
	WLI_DEFINE_STRUCT(Interface)     \
	WLI_DECLARE_PROTOTYPE(Interface) \
	WLI_DEFINE_REQUESTS(Interface)

#define WLI_DEFINE(Interface) WLI_DEFINE_PROTOTYPE(Interface)
