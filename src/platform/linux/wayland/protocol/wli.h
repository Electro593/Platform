/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
*                                                                             *
*  Author: Aria Seiler                                                        *
*                                                                             *
*  This program is in the public domain. There is no implied warranty, so use *
*  it at your own risk.                                                       *
*                                                                             *
\* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#pragma once

// Utility

#define WLI_TYPE_NAME(Interface) WLI_CAT(wayland_, WLI_LOWER_NAME(Interface))

#define WLI_PROTO(Interface)
WLI_CONCAT(
	MAC_CONCAT(Wayland_, WAYLAND_INTERFACE_GET_UPPER_NAME(Interface)),
	_Prototype
)

// Record Getters

#define WLI_META(Interface)	   WLI_HEAD Interface
#define WLI_METHODS(Interface) WLI_REST Interface

#define WLI_LOWER_NAME(Interface) WLI_SECOND(WLI_META(Interface))

#define WLI_REQUESTS(Interface)                                       \
	WLI_FILTER_METHODS(WLI_METHOD_IS_REQUEST, WLI_METHODS(Interface))
#define WLI_EVENTS(Interface)                                       \
	WLI_FILTER_METHODS(WLI_METHOD_IS_EVENT, WLI_METHODS(Interface))

// #define WLI_METHOD_META(Method) WLI_HEAD WLI_UNWRAP_METHOD(Method)
// #define WLI_METHOD_ARGS(Method) WLI_REST WLI_UNWRAP_METHOD(Method)

// #define WLI_METHOD_CTORS(Method)                              \
	WLI_FILTER_ARGS(WLI_ARG_IS_CTOR, WLI_METHOD_ARGS(Method))

// Dispatch Operators

// #define _WLI_UNWRAP_METHOD_R(Meta, ...) (Meta, __VA_ARGS__)
// #define _WLI_UNWRAP_METHOD_E(Meta, ...) (Meta, __VA_ARGS__)
// #define WLI_UNWRAP_METHOD(Method)		WLI_CAT(_WLI_UNWRAP_METHOD_, Method)

#define _WLI_METHOD_IS_REQUEST_R(Method) TRUE
#define _WLI_METHOD_IS_REQUEST_E(Method) FALSE
#define WLI_METHOD_IS_REQUEST(Method) WLI_CAT(_WLI_METHOD_IS_REQUEST_, Method)

#define _WLI_METHOD_IS_EVENT_R(Method) FALSE
#define _WLI_METHOD_IS_EVENT_E(Method) TRUE
#define WLI_METHOD_IS_EVENT(Method)	   WLI_CAT(_WLI_METHOD_IS_EVENT_, Method)

// #define _WLI_ARG_IS_CTOR_I(...) FALSE
// #define _WLI_ARG_IS_CTOR_U(...) FALSE
// #define _WLI_ARG_IS_CTOR_R(...) FALSE
// #define _WLI_ARG_IS_CTOR_F(...) FALSE
// #define _WLI_ARG_IS_CTOR_S(...) FALSE
// #define _WLI_ARG_IS_CTOR_A(...) FALSE
// #define _WLI_ARG_IS_CTOR_N(...) TRUE
// #define _WLI_ARG_IS_CTOR_O(...) FALSE
// #define _WLI_ARG_IS_CTOR_D(...) FALSE
// #define WLI_ARG_IS_CTOR(Arg)	WLI_CAT(_WLI_ARG_IS_CTOR_, Arg)

// Iterators

#define _WLI_IM_P		()
#define _WLI_IM_E0(...) __VA_ARGS__
#define _WLI_IM_E1(...)	_WLI_IM_E0(_WLI_IM_E0(_WLI_IM_E0(__VA_ARGS__))))
#define _WLI_IM_E2(...)	_WLI_IM_E1(_WLI_IM_E1(_WLI_IM_E1(__VA_ARGS__))))
#define _WLI_IM_E3(...)	_WLI_IM_E2(_WLI_IM_E2(_WLI_IM_E2(__VA_ARGS__))))
#define _WLI_IM_E(...)	_WLI_IM_E3(_WLI_IM_E3(_WLI_IM_E3(__VA_ARGS__))))
#define _WLI_IM_M(C, A, I) C(A, I)
#define _WLI_IM_F(C, A, I) WLI_WHEN(C(A))(A)
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

#define WLI_FILTER_METHODS(Condition, ...)                \
	_WLI_IM(Condition, _WLI_IM_F, WLI_SPACE, __VA_ARGS__)

#define WLI_FOREACH_REQUEST(Interface, Mapper, Separator)          \
	_WLI_IM(Interface, Mapper, Separator, WLI_REQUESTS(Interface))
#define WLI_FOREACH_EVENT(Interface, Mapper, Separator)          \
	_WLI_IM(Interface, Mapper, Separator, WLI_EVENTS(Interface))

// #define _WLI_IA_P		()
// #define _WLI_IA_E0(...) __VA_ARGS__
// #define _WLI_IA_E1(...)	_WLI_IA_E0(_WLI_IA_E0(_WLI_IA_E0(__VA_ARGS__))))
// #define _WLI_IA_E2(...)	_WLI_IA_E1(_WLI_IA_E1(_WLI_IA_E1(__VA_ARGS__))))
// #define _WLI_IA_E3(...)	_WLI_IA_E2(_WLI_IA_E2(_WLI_IA_E2(__VA_ARGS__))))
// #define _WLI_IA_E(...)	_WLI_IA_E3(_WLI_IA_E3(_WLI_IA_E3(__VA_ARGS__))))
// #define _WLI_IA_M(C, A, I) C(A, I)
// #define _WLI_IA_F(C, A, I) WLI_WHEN(C(A))(A)
// #define _WLI_IA_4(...)
// #define _WLI_IA_3() _WLI_IA_1
 // #define _WLI_IA_2(C, F, S, I, ...)                          \
	S _WLI_IA_3 _WLI_IA_P(C, F, S, WLI_INC(I), __VA_ARGS__)
 // #define _WLI_IA_1(C, F, S, I, A, ...) \
	F(C, A, I)                                                               \
	WLI_IF_EMPTY(__VA_ARGS__)(_WLI_IA_4, _WLI_IA_2)(C, F, S, I, __VA_ARGS__)
 // #define _WLI_IA_0(C, F, S, I, ...) \
	WLI_IF_EMPTY(__VA_ARGS__)(_WLI_IA_4, _WLI_IA_1)(C, F, S, I, __VA_ARGS__)
// #define _WLI_IA(C, F, S, ...) _WLI_IA_E(_WLI_IA_0(C, F, S, 0, __VA_ARGS__))

// #define WLI_MAP_ARGS(Mapper, ...)                      \
	_WLI_IA(Mapper, _WLI_IA_M, WLI_COMMA, __VA_ARGS__)
// #define WLI_FILTER_ARGS(Condition, ...)                   \
	_WLI_IA(Condition, _WLI_IA_F, WLI_SPACE, __VA_ARGS__)

// Helpers

#define WLI_SPACE(...)
#define WLI_COMMA(...) ,

// clang-format off
#define _WLI_COUNT_(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, I, ...) I
#define _WLI_COUNT(...) _WLI_COUNT_(__VA_ARGS__, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, )
#define WLI_IS_LIST(...) _WLI_COUNT_(__VA_ARGS__, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, )
// clang-format on

#define _WLI_IS_EMPTY_CHECK_0001 ,
#define _WLI_IS_EMPTY_CHECK_(_0, _1, _2, _3)                   \
	WLI_IS_LIST(_WLI_IS_EMPTY_CHECK_##_0##_##_1##_##_2##_##_3)
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
#define _WLI_EQ_(A, B)			  _WLI_EQ_##A##_##B
#define _WLI_EQ_CHECK_(_, X, ...) X
#define _WLI_EQ_CHECK(...)		  _WLI_EQ_CHECK_(__VA_ARGS__, 0, )
#define WLI_EQ(A, B)			  _WLI_EQ_CHECK(_WLI_EQ_(A, B))

#define _WLI_IF_0(Then, Else) Else
#define _WLI_IF_1(Then, Else) Then
#define _WLI_IF(Cond)		  _WLI_IF_##Cond

#define WLI_IF_EQ(X, ...)	_WLI_IF(WLI_EQ(X, __VA_ARGS__))
#define WLI_IF_EMPTY(...)	_WLI_IF(WLI_IS_EMPTY(__VA_ARGS__))
#define WLI_IF_WRAPPED(...) _WLI_IF(WLI_IS_WRAPPED(__VA_ARGS__))
#define WLI_IF_LIST(...)	_WLI_IF(WLI_IS_LIST(__VA_ARGS__))

#define _WLI_BOOL_TRUE(...)	 1
#define _WLI_BOOL_FALSE(...) 0
#define _WLI_BOOL_NONEMPTY(X)                           \
	WLI_IF_EQ(0, X)(_WLI_BOOL_FALSE, _WLI_BOOL_TRUE)(X)
#define _WLI_BOOL_TOKEN(X)                                 \
	WLI_IF_EMPTY(X)(_WLI_BOOL_TRUE, _WLI_BOOL_NONEMPTY)(X)
#define _WLI_BOOL_SINGLE(X)                               \
	WLI_IF_WRAPPED(X)(_WLI_BOOL_TRUE, _WLI_BOOL_TOKEN)(X)
#define WLI_BOOL(...)                                                       \
	WLI_IF_LIST(__VA_ARGS__)(_WLI_BOOL_TRUE, _WLI_BOOL_SINGLE)(__VA_ARGS__)

#define WLI_NONE(...)
#define WLI_HEAD(Head, ...) Head
#define WLI_REST(Head, ...) __VA_ARGS__
#define WLI_ALL(...)		__VA_ARGS__

#define WLI_IF(...)	  _WLI_IF(WLI_BOOL(__VA_ARGS__))
#define WLI_WHEN(...) WLI_IF(__VA_ARGS__)(WLI_ALL, WLI_NONE)
#define WLI_ELSE(...) WLI_IF(__VA_ARGS__)(WLI_NONE, WLI_ALL)

#define WLI_LEN(...) MAC_IF_EMPTY(__VA_ARGS__)(0, _WLI_COUNT(, __VA_ARGS__))

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
#define WLI_CAT(...)			 WLI_LENCALL(_WLI_CAT_, __VA_ARGS__)

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
#define WLI_INC(N)	WLI_CAT(_WLI_INC_, N)

// #define _WLI_FIRST(_0, ...)				 _0
// #define _WLI_SECOND(_0, _1, ...) _1
// #define _WLI_THIRD(_0, _1, _2, ...)		 _2
// #define _WLI_FOURTH(_0, _1, _2, _4, ...) _4

// #define WLI_FIRST(...)	_WLI_FIRST(__VA_ARGS__, )
// #define WLI_SECOND(...) _WLI_SECOND(__VA_ARGS__, , )
// #define WLI_THIRD(...)	_WLI_THIRD(__VA_ARGS__, , , )
// #define WLI_FOURTH(...) _WLI_FOURTH(__VA_ARGS__, , , , )

// Rest

#define WAYLAND_INTERFACE_GET_UPPER_NAME(Interface)                     \
	MAC_CHOOSE(0, MAC_UNPACKAGE(WAYLAND_INTERFACE_GET_META(Interface)))
#define WAYLAND_INTERFACE_GET_VERSION(Interface)                  \
	MAC_DEFAULT(                                                  \
		MAC_CHOOSE(                                               \
			2,                                                    \
			MAC_UNPACKAGE(WAYLAND_INTERFACE_GET_META(Interface)), \
			,                                                     \
                                                                  \
		),                                                        \
		1                                                         \
	)
#define WAYLAND_INTERFACE_GET_DEPRECATED(Interface)               \
	MAC_DEFAULT(                                                  \
		MAC_CHOOSE(                                               \
			3,                                                    \
			MAC_UNPACKAGE(WAYLAND_INTERFACE_GET_META(Interface)), \
			,                                                     \
                                                                  \
		),                                                        \
		FALSE                                                     \
	)

#define WAYLAND_INTERFACE_NAME(Interface)                              \
	("wl_" MAC_STRINGIFY(WAYLAND_INTERFACE_GET_LOWER_NAME(Interface)))

#define WAYLAND_INTERFACE_TYPE(Interface)           \
	struct WAYLAND_INTERFACE_TYPE_NAME(Interface) *

#define WAYLAND_INTERFACE_METHOD_GET_META(Method) \
	MAC_CHOOSE(0, MAC_UNPACKAGE(Method))
#define WAYLAND_INTERFACE_METHOD_GET_ARGS(Method) \
	MAC_SKIP(1, MAC_UNPACKAGE(Method))

#define WAYLAND_INTERFACE_METHOD_GET_NAME(Method) \
	MAC_CHOOSE(0, MAC_UNPACKAGE(WAYLAND_INTERFACE_METHOD_GET_META(Method)), , )
#define WAYLAND_INTERFACE_METHOD_GET_VERSION_SINCE(Method) \
	MAC_CHOOSE(1, MAC_UNPACKAGE(WAYLAND_INTERFACE_METHOD_GET_META(Method)), , )
#define WAYLAND_INTERFACE_METHOD_GET_VERSION_UNTIL(Method) \
	MAC_CHOOSE(2, MAC_UNPACKAGE(WAYLAND_INTERFACE_METHOD_GET_META(Method)), , )

#define _WL_INTERFACE(Name) WAYLAND_##Name

#define _WL_ARG_NAME_I(...)						   MAC_CHOOSE(0, __VA_ARGS__)
#define _WL_ARG_NAME_U(...)						   MAC_CHOOSE(0, __VA_ARGS__)
#define _WL_ARG_NAME_R(...)						   MAC_CHOOSE(0, __VA_ARGS__)
#define _WL_ARG_NAME_F(...)						   MAC_CHOOSE(0, __VA_ARGS__)
#define _WL_ARG_NAME_S(...)						   MAC_CHOOSE(0, __VA_ARGS__)
#define _WL_ARG_NAME_A(...)						   MAC_CHOOSE(0, __VA_ARGS__)
#define _WL_ARG_NAME_N(...)						   MAC_CHOOSE(0, __VA_ARGS__)
#define _WL_ARG_NAME_O(...)						   MAC_CHOOSE(0, __VA_ARGS__)
#define _WL_ARG_NAME_D(...)						   MAC_CHOOSE(0, __VA_ARGS__)
#define WAYLAND_INTERFACE_METHOD_ARG_GET_NAME(Arg) _WL_ARG_NAME_##Arg

#define _WL_ARG_TYPE_INT(Type, Default)                    \
	MAC_IF(Type)(enum MAC_CONCAT(wayland_, Type), Default)
#define _WL_ARG_TYPE_OBJ(Name) WAYLAND_INTERFACE_TYPE(_WL_INTERFACE(Name))
#define _WL_ARG_TYPE_I(...) _WL_ARG_TYPE_INT(MAC_CHOOSE(1, __VA_ARGS__, ), s32)
#define _WL_ARG_TYPE_U(...) _WL_ARG_TYPE_INT(MAC_CHOOSE(1, __VA_ARGS__, ), u32)
#define _WL_ARG_TYPE_R(...) wayland_fixed
#define _WL_ARG_TYPE_F(...) wayland_fd
#define _WL_ARG_TYPE_S(...) string
#define _WL_ARG_TYPE_A(...) wayland_array
#define _WL_ARG_TYPE_N(...) _WL_ARG_TYPE_OBJ(MAC_CHOOSE(1, __VA_ARGS__)) *
#define _WL_ARG_TYPE_O(...) _WL_ARG_TYPE_OBJ(MAC_CHOOSE(1, __VA_ARGS__))
#define _WL_ARG_TYPE_D(...) _WL_ARG_TYPE_OBJ(MAC_CHOOSE(1, __VA_ARGS__))
#define WAYLAND_INTERFACE_METHOD_ARG_GET_TYPE(Arg) _WL_ARG_TYPE_##Arg

#define _WL_ARG_FORMAT_NULLABLE(...)              \
	MAC_WHEN(MAC_CHOOSE(2, __VA_ARGS__, , ))("?")
#define _WL_ARG_FORMAT_I(...) "i"
#define _WL_ARG_FORMAT_U(...) "u"
#define _WL_ARG_FORMAT_R(...) "r"
#define _WL_ARG_FORMAT_F(...) "f"
#define _WL_ARG_FORMAT_S(...) "s" _WL_ARG_FORMAT_NULLABLE(__VA_ARGS__)
#define _WL_ARG_FORMAT_A(...) "a" _WL_ARG_FORMAT_NULLABLE(__VA_ARGS__)
#define _WL_ARG_FORMAT_N(...) "n"
#define _WL_ARG_FORMAT_O(...) "o" _WL_ARG_FORMAT_NULLABLE(__VA_ARGS__)
#define _WL_ARG_FORMAT_D(...) "d" _WL_ARG_FORMAT_NULLABLE(__VA_ARGS__)
#define WAYLAND_INTERFACE_METHOD_ARG_GET_FORMAT(Arg) _WL_ARG_FORMAT_##Arg

#define _WL_ARG_PROTOTYPE(Name)                                 \
	MAC_IF(Name)(&WAYLAND_PROTOTYPE(_WL_INTERFACE(Name)), NULL)
#define _WL_ARG_PROTOTYPE_I(...)
#define _WL_ARG_PROTOTYPE_U(...)
#define _WL_ARG_PROTOTYPE_R(...)
#define _WL_ARG_PROTOTYPE_F(...)
#define _WL_ARG_PROTOTYPE_S(...)
#define _WL_ARG_PROTOTYPE_A(...)
#define _WL_ARG_PROTOTYPE_N(...) _WL_ARG_PROTOTYPE(MAC_CHOOSE(1, __VA_ARGS__, ))
#define _WL_ARG_PROTOTYPE_O(...)
#define _WL_ARG_PROTOTYPE_D(...)
#define WAYLAND_INTERFACE_METHOD_ARG_GET_PROTOTYPE(Arg) _WL_ARG_PROTOTYPE_##Arg

#define _WL_ARG_PROTOTYPE_TYPE(Name)                                \
	MAC_IF(Name)(WAYLAND_INTERFACE_TYPE(_WL_INTERFACE(Name)), NULL)
#define _WL_ARG_PROTOTYPE_TYPE_I(...)
#define _WL_ARG_PROTOTYPE_TYPE_U(...)
#define _WL_ARG_PROTOTYPE_TYPE_R(...)
#define _WL_ARG_PROTOTYPE_TYPE_F(...)
#define _WL_ARG_PROTOTYPE_TYPE_S(...)
#define _WL_ARG_PROTOTYPE_TYPE_A(...)
#define _WL_ARG_PROTOTYPE_TYPE_N(...)                    \
	_WL_ARG_PROTOTYPE_TYPE(MAC_CHOOSE(1, __VA_ARGS__, ))
#define _WL_ARG_PROTOTYPE_TYPE_O(...)
#define _WL_ARG_PROTOTYPE_TYPE_D(...)
#define WAYLAND_INTERFACE_METHOD_ARG_GET_PROTOTYPE_TYPE(Arg) \
	_WL_ARG_PROTOTYPE_TYPE_##Arg

#define WAYLAND_INTERFACE_METHOD_ARG_GET_PARAM(Arg) \
	WAYLAND_INTERFACE_METHOD_ARG_GET_TYPE(Arg)      \
	WAYLAND_INTERFACE_METHOD_ARG_GET_NAME(Arg)

#define _WL_PARENS	  ()
#define _WL_E0(...)	  __VA_ARGS__
#define _WL_E1(...)	  _WL_E0(_WL_E0(_WL_E0(__VA_ARGS__)))
#define _WL_E2(...)	  _WL_E1(_WL_E1(_WL_E1(__VA_ARGS__)))
#define _WL_EVAL(...) _WL_E2(_WL_E2(_WL_E2(__VA_ARGS__)))
#define _WL_ARGS_STOP(...)
#define _WL_ARGS_NEXT() _WL_ARGS_STEP
#define _WL_ARGS_ADJACENT()
#define _WL_ARGS_COMMA() ,
#define _WL_ARGS_MORE(P, Sep, ...)                      \
	Sep() _WL_ARGS_NEXT _WL_PARENS(P, Sep, __VA_ARGS__)
#define _WL_ARGS_START(P, Sep, ...)                                  \
	MAC_IF(MAC_IS_EMPTY(__VA_ARGS__))(_WL_ARGS_STOP, _WL_ARGS_STEP)( \
		P,                                                           \
		Sep,                                                         \
		__VA_ARGS__                                                  \
	)
#define _WL_ARGS_STEP(P, Sep, A, ...)                                \
	P(A)                                                             \
	MAC_IF(MAC_IS_EMPTY(__VA_ARGS__))(_WL_ARGS_STOP, _WL_ARGS_MORE)( \
		P,                                                           \
		Sep,                                                         \
		__VA_ARGS__                                                  \
	)
#define _WL_ARGS_JOIN(P, Sep, Method) \
	_WL_EVAL(_WL_ARGS_START(P, Sep, WAYLAND_INTERFACE_METHOD_GET_ARGS(Method)))
#define _WL_ARGS(P, Method) _WL_ARGS_JOIN(P, _WL_ARGS_ADJACENT, Method)

#define WAYLAND_INTERFACE_METHOD_GET_FORMAT(Method)                            \
	(MAC_WHEN(WAYLAND_INTERFACE_METHOD_GET_VERSION_SINCE(Method))(             \
		MAC_STRINGIFY(WAYLAND_INTERFACE_METHOD_GET_VERSION_SINCE(Method))      \
	)                                                                          \
			MAC_WHEN(WAYLAND_INTERFACE_METHOD_GET_VERSION_UNTIL(Method))(      \
				"-" MAC_STRINGIFY(                                             \
					WAYLAND_INTERFACE_METHOD_GET_VERSION_UNTIL(Method)         \
				)                                                              \
			)                                                                  \
				MAC_DEFAULT(                                                   \
					_WL_ARGS(WAYLAND_INTERFACE_METHOD_ARG_GET_FORMAT, Method), \
					""                                                         \
				))

#define WAYLAND_INTERFACE_METHOD_GET_PROTOTYPES(Method) \
	_WL_ARGS_JOIN(                                      \
		WAYLAND_INTERFACE_METHOD_ARG_GET_PROTOTYPE,     \
		_WL_ARGS_COMMA,                                 \
		Method                                          \
	)

#define WAYLAND_INTERFACE_METHOD_GET_PROTOTYPE_TYPES(Method)          \
	_WL_ARGS(WAYLAND_INTERFACE_METHOD_ARG_GET_PROTOTYPE_TYPE, Method)

#define WAYLAND_INTERFACE_PROTOTYPE_METHOD_HAS_PROTOTYPES(Method) \
	MAC_BOOL(WAYLAND_INTERFACE_METHOD_GET_PROTOTYPES(Method))

#define WAYLAND_INTERFACE_PROTOTYPE_METHOD_HAS_SINGLE_CTOR(Method)           \
	MAC_EQ(MAC_ARGCOUNT(WAYLAND_INTERFACE_METHOD_GET_PROTOTYPES(Method)), 1)

#define WAYLAND_INTERFACE_METHOD_PARAMS(Method) \
	(_WL_ARGS_JOIN(                             \
		WAYLAND_INTERFACE_METHOD_ARG_GET_PARAM, \
		_WL_ARGS_COMMA,                         \
		Method                                  \
	))

#define WAYLAND_INTERFACE_REQUEST_HEADER(Method)                        \
	MAC_IF(WAYLAND_INTERFACE_PROTOTYPE_METHOD_HAS_SINGLE_CTOR(Method))( \
		WAYLAND_INTERFACE_METHOD_GET_PROTOTYPE_TYPES(Method),           \
		void                                                            \
	) WAYLAND_INTERFACE_METHOD_PARAMS(Method)

#define WAYLAND_INTERFACE_REQUEST_DECL(Method) \
	WAYLAND_INTERFACE_REQUEST_HEADER(Method);

#define WAYLAND_INTERFACE_EVENT_DECL(Method)         \
	void(*WAYLAND_INTERFACE_METHOD_GET_NAME(Method)) \
		WAYLAND_INTERFACE_METHOD_PARAMS(Method);

#define WAYLAND_INTERFACE_REQUEST_DEFN(Method)              \
	WAYLAND_INTERFACE_REQUEST_HEADER(Method) { /* TODO */ }

#define WAYLAND_INTERFACE_PROTOTYPE_METHOD_PROTOTYPES_LITERAL(Method) \
	(wayland_prototype *[]) { WAYLAND_INTERFACE_METHOD_GET_PROTOTYPES(Method) }

#define WAYLAND_INTERFACE_PROTOTYPE_METHOD_LITERAL(Method)                     \
	((wayland_prototype_method){                                               \
		.Format = WAYLAND_INTERFACE_METHOD_GET_FORMAT(Method),                 \
		.Prototypes =                                                          \
			MAC_IF(WAYLAND_INTERFACE_PROTOTYPE_METHOD_HAS_PROTOTYPES(Method))( \
				WAYLAND_INTERFACE_PROTOTYPE_METHOD_PROTOTYPES_LITERAL(Method), \
				NULL                                                           \
			),                                                                 \
	})

#define WAYLAND_INTERFACE_PROTOTYPE_METHODS_LITERAL(...) \
	((wayland_prototype_method[]){ MAC_FOREACH(          \
		WAYLAND_INTERFACE_PROTOTYPE_METHOD_LITERAL,      \
		MAC_FOR_OP_SEQ,                                  \
		MAC_FOR_FUNC_CALL,                               \
		__VA_ARGS__                                      \
	) })

#define WAYLAND_INTERFACE_PROTOTYPE_LITERAL(Interface)                       \
	((wayland_prototype){                                                    \
		.Name		= WAYLAND_INTERFACE_NAME(Interface),                     \
		.Version	= WAYLAND_INTERFACE_GET_VERSION(Interface),              \
		.Deprecated = WAYLAND_INTERFACE_GET_DEPRECATED(Interface),           \
		.RequestCount =                                                      \
			MAC_ARGCOUNT(WAYLAND_INTERFACE_GET_REQUESTS(Interface)),         \
		.EventCount = MAC_ARGCOUNT(WAYLAND_INTERFACE_GET_EVENTS(Interface)), \
		.Requests	= WAYLAND_INTERFACE_PROTOTYPE_METHODS_LITERAL(           \
			WAYLAND_INTERFACE_GET_REQUESTS(Interface)                        \
		),                                                                   \
		.Events = WAYLAND_INTERFACE_PROTOTYPE_METHODS_LITERAL(               \
			WAYLAND_INTERFACE_GET_EVENTS(Interface)                          \
		),                                                                   \
	})

// Declarators

// TODO
#define WLI_PROTOTYPE_LITERAL(Interface)
#define WLI_METHOD_TYPE(Method)
#define WLI_METHOD_HEADER(Method)
#define WLI_METHOD_BODY(Method)

#define WLI_DECLARE_EVENT(Interface, Method, Index) \
	WLI_METHOD_TYPE(Interface, Method);

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
		WLI_PROTOTYPE_LITERAL(Interface);

#define WLI_DECLARE_REQUEST(Interface, Method, Index) \
	WLI_METHOD_HEADER(Interface, Method);

#define WLI_DECLARE_REQUESTS(Interface)                            \
	WLI_FOREACH_REQUEST(Interface, WLI_DECLARE_REQUEST, WLI_SPACE)

#define WLI_DEFINE_REQUEST(Interface, Method, Index) \
	WLI_METHOD_HEADER(Interface, Method)             \
	WLI_METHOD_BODY(Interface, Method, Index)

#define WLI_DEFINE_REQUESTS(Interface)                            \
	WLI_FOREACH_REQUEST(Interface, WLI_DEFINE_REQUEST, WLI_SPACE)

#define WLI_DECLARE(Interface)       \
	WLI_DECLARE_STRUCT(Interface)    \
	WLI_DEFINE_STRUCT(Interface)     \
	WLI_DECLARE_PROTOTYPE(Interface) \
	WLI_DECLARE_REQUESTS(Interface)

#define WLI_DEFINE(Interface)       \
	WLI_DEFINE_PROTOTYPE(Interface) \
	WLI_DEFINE_REQUESTS(Interface)
