/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
**                                                                         **
**  Author: Aria Seiler                                                    **
**                                                                         **
**  This program is in the public domain. There is no implied warranty,    **
**  so use it at your own risk.                                            **
**                                                                         **
\* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifdef INCLUDE_HEADER

// CREDIT https://gitlab.inria.fr/gustedt/p99/-/blob/master/p99/p99_for.h

#define MAC_CAT_2(_0, _1) _0 ## _1
#define MAC_PASTE_2(_0, _1) MAC_CAT_2(_0, _1)

#define MAC_PRED(N) _MAC_PRED(N)
#define _MAC_PRED(N) _MAC_PRED_(MAC_PRED_ , N)
#define _MAC_PRED_(P, N) P ## N
#define MAC_PRED_1 0
#define MAC_PRED_2 1
#define MAC_PRED_3 2
#define MAC_PRED_4 3

#define _MAC_ARG(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, ...) _16
#define _MAC_NARG_(...) _MAC_ARG(__VA_ARGS__, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0,)
#define _MAC_NARG(...) _MAC_NARG_(__VA_ARGS__)

#define MAC_SKP(N, ...) MAC_PASTE_2(MAC_SKP_, N)(__VA_ARGS__)
#define MAC_SKP_0(...) __VA_ARGS__
#define MAC_SKP_1(_0, ...) __VA_ARGS__
#define MAC_SKP_2(_0, ...) MAC_SKP_1(__VA_ARGS__)
#define MAC_SKP_3(_0, ...) MAC_SKP_2(__VA_ARGS__)

#define MAC_CHS(N, ...) _MAC_CHS(MAC_SKP(N, __VA_ARGS__))
#define _MAC_CHS(...) _MAC_CHS_(__VA_ARGS__,)
#define _MAC_CHS_(X, ...) X

#define MAC_PRE_0(...)
#define MAC_PRE_1(_0, ...) _0
#define MAC_PRE_2(_0, ...) _0, MAC_PRE_1(__VA_ARGS__)
#define MAC_PRE_3(_0, ...) _0, MAC_PRE_2(__VA_ARGS__)

#define MAC_LAST(...) MAC_CHS(MAC_PRED(_MAC_NARG(__VA_ARGS__)), __VA_ARGS__,)
#define MAC_ALLBUTLAST(...) MAC_PASTE_2(MAC_PRE_, MAC_PRED(_MAC_NARG(__VA_ARGS__)))(__VA_ARGS__,)

#define MAC_FOR_OP_SEP_REV(NAME, ITER, PREV, CURR) CURR; PREV
#define MAC_FOR_OP_SEQ_REV(NAME, ITER, PREV, CURR) CURR, PREV
#define MAC_FOR_OP_NAME_REV(NAME, ITER, PREV, CURR) CURR NAME PREV

#define MAC_FOR_FUNC_DECLVAR(NAME, ARG, ITER)  NAME ARG
#define MAC_FOR_FUNC_V_ASSIGN(NAME, ARG, ITER) Result.ARG = ARG
#define MAC_FOR_FUNC_V_OP(NAME, ARG, ITER)     Result.ARG = NAME V.ARG
#define MAC_FOR_FUNC_VV_OP(NAME, ARG, ITER)    Result.ARG = A.ARG NAME B.ARG
#define MAC_FOR_FUNC_VS_OP(NAME, ARG, ITER)    Result.ARG = V.ARG NAME S
#define MAC_FOR_FUNC_VV_EQ(NAME, ARG, ITER)    A.ARG == B.ARG
#define MAC_FOR_FUNC_VEC_DOT(NAME, ARG, ITER)  (B.ARG*A.ARG)
#define MAC_FOR_FUNC_CLAMP(NAME, ARG, ITER)    Result.ARG = NAME##_Clamp(V.ARG, S, E)
#define MAC_FOR_FUNC_LERP(NAME, ARG, ITER)     NAME##_Lerp(A.ARG, B.ARG, T)

#define MAC_FOR(NAME, N, OP, FUNC, ...) MAC_PASTE_2(MAC_FOR_, N)(NAME, OP, FUNC, __VA_ARGS__)
#define MAC_FOR_0(NAME, OP, FUNC, ...)
#define MAC_FOR_1(NAME, OP, FUNC, ...) FUNC(NAME, MAC_LAST(__VA_ARGS__), 0)
#define MAC_FOR_2(NAME, OP, FUNC, ...) OP(NAME, 1, MAC_FOR_1(NAME, OP, FUNC, MAC_ALLBUTLAST(__VA_ARGS__)), FUNC(NAME, MAC_LAST(__VA_ARGS__), 1))
#define MAC_FOR_3(NAME, OP, FUNC, ...) OP(NAME, 2, MAC_FOR_2(NAME, OP, FUNC, MAC_ALLBUTLAST(__VA_ARGS__)), FUNC(NAME, MAC_LAST(__VA_ARGS__), 2))
#define MAC_FOR_4(NAME, OP, FUNC, ...) OP(NAME, 3, MAC_FOR_3(NAME, OP, FUNC, MAC_ALLBUTLAST(__VA_ARGS__)), FUNC(NAME, MAC_LAST(__VA_ARGS__), 3))

#define MAC_FOR_ARGS_VEC W, Z, Y, X
#define MAC_FOR_ARGS_MAT W, Z, Y, X

#define DECLARE_VECTOR_TYPE(Count, Type) \
   typedef union v##Count##Type {        \
      struct {                           \
         MAC_FOR(Type, Count, MAC_FOR_OP_SEP_REV, MAC_FOR_FUNC_DECLVAR, MAC_FOR_ARGS_VEC); \
      };                                 \
      Type E[Count];                     \
   } v##Count##Type;

#define DECLARE_MATRIX_TYPE(Rows, Cols, Type) \
   typedef union m##Rows##x##Cols##Type {     \
      v##Cols##Type V[Rows];                  \
      Type E[Rows*Cols];                      \
   } m##Rows##x##Cols##Type;

DECLARE_VECTOR_TYPE(2, r32)
DECLARE_VECTOR_TYPE(2, r64)
DECLARE_VECTOR_TYPE(2, s16)
DECLARE_VECTOR_TYPE(2, s32)
DECLARE_VECTOR_TYPE(2, u16)
DECLARE_VECTOR_TYPE(2, u32)

DECLARE_VECTOR_TYPE(3, r32)
DECLARE_VECTOR_TYPE(3, s32)
DECLARE_VECTOR_TYPE(3, u08)
DECLARE_VECTOR_TYPE(3, u32)

DECLARE_VECTOR_TYPE(4, r32)
DECLARE_VECTOR_TYPE(4, s16)
DECLARE_VECTOR_TYPE(4, u08)
DECLARE_VECTOR_TYPE(4, u32)

DECLARE_MATRIX_TYPE(4, 4, r32);

#undef DECLARE_MATRIX_TYPE
#undef DECLARE_VECTOR_TYPE

#define M4x4r32_I (m4x4r32){1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1}

#define DEFINE_VECTOR_CROSS(N, T)     DEFINE_VECTOR_CROSS_##N(T)

#define VECTOR_FUNCS \
   DEFINE_VECTOR_ADD(2, r32) \
   DEFINE_VECTOR_ADD(2, u32) \
   DEFINE_VECTOR_ADD(2, s16) \
   DEFINE_VECTOR_ADD(2, s32) \
   DEFINE_VECTOR_ADD(3, r32) \
   DEFINE_VECTOR_ADD(3, s32) \
   \
   DEFINE_VECTOR_ADDS(3, r32) \
   DEFINE_VECTOR_ADDS(4, r32) \
   \
   DEFINE_VECTOR_SUB(2, r32) \
   DEFINE_VECTOR_SUB(3, r32) \
   DEFINE_VECTOR_SUB(2, s16) \
   DEFINE_VECTOR_SUB(2, s32) \
   DEFINE_VECTOR_SUB(2, u32) \
   \
   DEFINE_VECTOR_SUBS(3, r32) \
   DEFINE_VECTOR_SUBS(3, s32) \
   \
   DEFINE_VECTOR_MUL(2, r32) \
   DEFINE_VECTOR_MUL(3, r32) \
   DEFINE_VECTOR_MUL(3, s32) \
   \
   DEFINE_VECTOR_MULS(2, r32) \
   DEFINE_VECTOR_MULS(3, r32) \
   DEFINE_VECTOR_MULS(4, r32) \
   DEFINE_VECTOR_MULS(2, s16) \
   \
   DEFINE_VECTOR_DIV(2, r32) \
   DEFINE_VECTOR_DIV(3, r32) \
   DEFINE_VECTOR_DIV(3, s32) \
   \
   DEFINE_VECTOR_DIVS(2, r32) \
   DEFINE_VECTOR_DIVS(3, r32) \
   DEFINE_VECTOR_DIVS(4, r32) \
   DEFINE_VECTOR_DIVS(3, u32) \
   \
   DEFINE_VECTOR_DOT(2, r32) \
   DEFINE_VECTOR_DOT(2, s16) \
   DEFINE_VECTOR_DOT(3, r32) \
   DEFINE_VECTOR_DOT(4, r32) \
   \
   DEFINE_VECTOR_VOLUME(s32, S32) \
   DEFINE_VECTOR_VOLUME(u32, U32) \
   \
   DEFINE_VECTOR_CROSS(2, r32) \
   DEFINE_VECTOR_CROSS(3, r32) \
   DEFINE_VECTOR_CROSS(2, s16) \
   \
   DEFINE_VECTOR_CAST(2, r32, r32) \
   DEFINE_VECTOR_CAST(2, s16, r32) \
   DEFINE_VECTOR_CAST(3, r32, r32) \
   DEFINE_VECTOR_CAST(3, r32, s32) \
   DEFINE_VECTOR_CAST(3, r32, u32) \
   DEFINE_VECTOR_CAST(3, s32, r32) \
   DEFINE_VECTOR_CAST(3, u32, r32) \
   DEFINE_VECTOR_CAST(3, u32, s32) \
   \
   DEFINE_VECTOR_CLAMP(3, r32, R32) \
   DEFINE_VECTOR_CLAMP(4, r32, R32) \
   DEFINE_VECTOR_CLAMP(3, s32, S32) \
   \
   DEFINE_VECTOR_LERP(4, u08, U08) \
   \
   DEFINE_VECTOR_EQUAL(2, u32) \
   DEFINE_VECTOR_EQUAL(3, r32) \
   DEFINE_VECTOR_EQUAL(3, s32) \
   DEFINE_VECTOR_EQUAL(4, u08) \
   DEFINE_VECTOR_EQUAL(4, u32) \
   \
   DEFINE_VECTOR_ISZERO(2, u32) \
   DEFINE_VECTOR_ISZERO(4, u08) \
   DEFINE_VECTOR_ISZERO(4, u32) \
   \
   DEFINE_VECTOR_LEN(2, r32) \
   DEFINE_VECTOR_LEN(2, s16) \
   DEFINE_VECTOR_LEN(3, r32) \
   \
   DEFINE_VECTOR_NORM(2, r32) \
   DEFINE_VECTOR_NORM(2, s16) \
   DEFINE_VECTOR_NORM(3, r32) \
   \
   EXPORT(m4x4r32, M4x4r32_Translation,  r32 X, r32 Y, r32 Z) \
   EXPORT(m4x4r32, M4x4r32_Scaling,      r32 X, r32 Y, r32 Z) \
   EXPORT(m4x4r32, M4x4r32_Rotation,     r32 Theta, v3r32 Axis) \
   EXPORT(m4x4r32, M4x4r32_Transpose,    m4x4r32 M) \
   EXPORT(v4r32,   M4x4r32_MulMV,        m4x4r32 M, v4r32 V) \
   EXPORT(m4x4r32, M4x4r32_Mul,          m4x4r32 A, m4x4r32 B) \
   EXPORT(b08,     RayPlaneIntersection, v3r32 PlanePoint, v3r32 PlaneNormal, v3r32 RayPoint, v3r32 RayDir, r32 *T) \
   EXPORT(b08,     RayRectIntersectionA, v3r32 RectStart, v3r32 RectEnd, v3r32 RectNormal, v3r32 RayPoint, v3r32 RayDir, r32 *T, v3r32 *Intersection)

#endif

#ifdef INCLUDE_SOURCE

#undef DEFINE_VECTOR_INIT
#undef DEFINE_VECTOR_ADD
#undef DEFINE_VECTOR_ADDS
#undef DEFINE_VECTOR_SUB
#undef DEFINE_VECTOR_SUBS
#undef DEFINE_VECTOR_MUL
#undef DEFINE_VECTOR_MULS
#undef DEFINE_VECTOR_DIV
#undef DEFINE_VECTOR_DIVS
#undef DEFINE_VECTOR_VOLUME
#undef DEFINE_VECTOR_DOT
#undef DEFINE_VECTOR_CROSS_2
#undef DEFINE_VECTOR_CROSS_3
#undef DEFINE_VECTOR_CAST
#undef DEFINE_VECTOR_CLAMP
#undef DEFINE_VECTOR_LERP
#undef DEFINE_VECTOR_EQUAL
#undef DEFINE_VECTOR_ISZERO
#undef DEFINE_VECTOR_LEN
#undef DEFINE_VECTOR_NORM

#define DEFINE_VECTOR_INIT(Count, Type) \
   internal v##Count##Type \
   V##Count##Type( \
      MAC_FOR(Type, Count, MAC_FOR_OP_SEQ_REV, MAC_FOR_FUNC_DECLVAR, MAC_FOR_ARGS_VEC)) \
   { \
      v##Count##Type Result; \
      MAC_FOR(, Count, MAC_FOR_OP_SEP_REV, MAC_FOR_FUNC_V_ASSIGN, MAC_FOR_ARGS_VEC); \
      return Result; \
   }

#define DEFINE_VECTOR_ADD(Count, Type) \
   internal v##Count##Type \
   V##Count##Type##_Add(v##Count##Type A, \
                        v##Count##Type B) \
   { \
      v##Count##Type Result; \
      MAC_FOR(+, Count, MAC_FOR_OP_SEP_REV, MAC_FOR_FUNC_VV_OP, MAC_FOR_ARGS_VEC); \
      return Result; \
   }

#define DEFINE_VECTOR_ADDS(Count, Type) \
   internal v##Count##Type \
   V##Count##Type##_AddS(v##Count##Type V, \
                         Type S) \
   { \
      v##Count##Type Result; \
      MAC_FOR(+, Count, MAC_FOR_OP_SEP_REV, MAC_FOR_FUNC_VS_OP, MAC_FOR_ARGS_VEC); \
      return Result; \
   }

#define DEFINE_VECTOR_SUB(Count, Type) \
   internal v##Count##Type \
   V##Count##Type##_Sub(v##Count##Type A, \
                        v##Count##Type B) \
   { \
      v##Count##Type Result; \
      MAC_FOR(-, Count, MAC_FOR_OP_SEP_REV, MAC_FOR_FUNC_VV_OP, MAC_FOR_ARGS_VEC); \
      return Result; \
   }

#define DEFINE_VECTOR_SUBS(Count, Type)                                            \
   internal v##Count##Type                                                         \
   V##Count##Type##_SubS(v##Count##Type V,                                         \
                         Type S)                                                   \
   {                                                                               \
      v##Count##Type Result;                                                       \
      MAC_FOR(-, Count, MAC_FOR_OP_SEP_REV, MAC_FOR_FUNC_VS_OP, MAC_FOR_ARGS_VEC); \
      return Result;                                                               \
   }

#define DEFINE_VECTOR_MUL(Count, Type) \
   internal v##Count##Type \
   V##Count##Type##_Mul(v##Count##Type A, \
                        v##Count##Type B) \
   { \
      v##Count##Type Result; \
      MAC_FOR(*, Count, MAC_FOR_OP_SEP_REV, MAC_FOR_FUNC_VV_OP, MAC_FOR_ARGS_VEC); \
      return Result; \
   }

#define DEFINE_VECTOR_MULS(Count, Type) \
   internal v##Count##Type \
   V##Count##Type##_MulS(v##Count##Type V, \
                         Type S) \
   { \
      v##Count##Type Result; \
      MAC_FOR(*, Count, MAC_FOR_OP_SEP_REV, MAC_FOR_FUNC_VS_OP, MAC_FOR_ARGS_VEC); \
      return Result; \
   }

#define DEFINE_VECTOR_DIV(Count, Type) \
   internal v##Count##Type \
   V##Count##Type##_Div(v##Count##Type A, \
                        v##Count##Type B) \
   { \
      v##Count##Type Result; \
      MAC_FOR(/, Count, MAC_FOR_OP_SEP_REV, MAC_FOR_FUNC_VV_OP, MAC_FOR_ARGS_VEC); \
      return Result; \
   }

#define DEFINE_VECTOR_DIVS(Count, Type) \
   internal v##Count##Type \
   V##Count##Type##_DivS(v##Count##Type V, \
                         Type S) \
   { \
      v##Count##Type Result; \
      MAC_FOR(/, Count, MAC_FOR_OP_SEP_REV, MAC_FOR_FUNC_VS_OP, MAC_FOR_ARGS_VEC); \
      return Result; \
   }

#define DEFINE_VECTOR_VOLUME(Type, TypeName) \
   internal Type \
   V3##Type##_Volume(v3##Type V) \
   { \
      return TypeName##_Abs(V.X) * TypeName##_Abs(V.Y) * TypeName##_Abs(V.Z); \
   }

#define DEFINE_VECTOR_DOT(Count, Type) \
   internal Type \
   V##Count##Type##_Dot(v##Count##Type A, \
                        v##Count##Type B) \
   { \
      Type Result; \
      Result = MAC_FOR(+, Count, MAC_FOR_OP_NAME_REV, MAC_FOR_FUNC_VEC_DOT, MAC_FOR_ARGS_VEC); \
      return Result; \
   }

#define DEFINE_VECTOR_CROSS_3(Type) \
   internal v3##Type \
   V3##Type##_Cross(v3##Type A, \
                    v3##Type B) \
   { \
      v3##Type Result; \
      Result.X = (A.Y*B.Z)-(A.Z*B.Y); \
      Result.Y = (A.Z*B.X)-(A.X*B.Z); \
      Result.Z = (A.X*B.Y)-(A.Y*B.X); \
      return Result; \
   }

#define DEFINE_VECTOR_CROSS_2(Type) \
   internal Type \
   V2##Type##_Cross(v2##Type A, \
                    v2##Type B) \
   { \
      Type Result; \
      Result = (A.X*B.Y)-(A.Y*B.X); \
      return Result; \
   }

#define DEFINE_VECTOR_CAST(Count, FromType, ToType) \
   internal v##Count##ToType \
   V##Count##FromType##_ToV##Count##ToType(v##Count##FromType V) \
   { \
      v##Count##ToType Result; \
      MAC_FOR((ToType), Count, MAC_FOR_OP_SEP_REV, MAC_FOR_FUNC_V_OP, MAC_FOR_ARGS_VEC); \
      return Result; \
   }

#define DEFINE_VECTOR_CLAMP(Count, Type, TypeName)                                        \
   internal v##Count##Type                                                                \
   V##Count##Type##_Clamp(v##Count##Type V, Type S, Type E)                               \
   {                                                                                      \
      v##Count##Type Result = V;                                                          \
      MAC_FOR(TypeName, Count, MAC_FOR_OP_SEP_REV, MAC_FOR_FUNC_CLAMP, MAC_FOR_ARGS_VEC); \
      return Result;                                                                      \
   }

#define DEFINE_VECTOR_LERP(Count, Type, TypeName)                                          \
   internal v##Count##Type                                                                 \
   V##Count##Type##_Lerp(v##Count##Type A,                                                 \
                         v##Count##Type B,                                                 \
                         r32 T)                                                            \
   {                                                                                       \
      return (v##Count##Type){                                                             \
         MAC_FOR(TypeName, Count, MAC_FOR_OP_SEQ_REV, MAC_FOR_FUNC_LERP, MAC_FOR_ARGS_VEC) \
      };                                                                                   \
   }

#define DEFINE_VECTOR_EQUAL(Count, Type)                                                    \
   internal b08                                                                             \
   V##Count##Type##_IsEqual(v##Count##Type A,                                               \
                            v##Count##Type B)                                               \
   {                                                                                        \
      return MAC_FOR(&&, Count, MAC_FOR_OP_NAME_REV, MAC_FOR_FUNC_VV_EQ, MAC_FOR_ARGS_VEC); \
   }

#define DEFINE_VECTOR_ISZERO(Count, Type)                                      \
   internal b08                                                                \
   V##Count##Type##_IsZero(v##Count##Type V)                                   \
   {                                                                           \
      return V##Count##Type##_IsEqual(V, (v##Count##Type){0});                 \
   }

#define DEFINE_VECTOR_LEN(Count, Type)                          \
   internal r32                                                 \
   V##Count##Type##_Len(v##Count##Type V)                       \
   {                                                            \
      v##Count##r32 Vr32 = V##Count##Type##_ToV##Count##r32(V); \
      return R32_sqrt(V##Count##r32_Dot(Vr32, Vr32));           \
   }

#define DEFINE_VECTOR_NORM(Count, Type)                       \
   internal v##Count##r32                                     \
   V##Count##Type##_Norm(v##Count##Type V)                    \
   {                                                          \
      r32 L = V##Count##Type##_Len(V);                        \
      v##Count##r32 CV = V##Count##Type##_ToV##Count##r32(V); \
      return V##Count##r32_DivS(CV, L);                       \
   }

#define EXPORT(...)

VECTOR_FUNCS

#undef DEFINE_VECTOR_INIT
#undef DEFINE_VECTOR_ADD
#undef DEFINE_VECTOR_ADDS
#undef DEFINE_VECTOR_SUB
#undef DEFINE_VECTOR_SUBS
#undef DEFINE_VECTOR_MUL
#undef DEFINE_VECTOR_MULS
#undef DEFINE_VECTOR_DIV
#undef DEFINE_VECTOR_DIVS
#undef DEFINE_VECTOR_VOLUME
#undef DEFINE_VECTOR_DOT
#undef DEFINE_VECTOR_CROSS_2
#undef DEFINE_VECTOR_CROSS_3
#undef DEFINE_VECTOR_CAST
#undef DEFINE_VECTOR_CLAMP
#undef DEFINE_VECTOR_LERP
#undef DEFINE_VECTOR_EQUAL
#undef DEFINE_VECTOR_ISZERO
#undef DEFINE_VECTOR_LEN
#undef DEFINE_VECTOR_NORM
#undef EXPORT

internal m4x4r32
M4x4r32_Translation(r32 X, r32 Y, r32 Z)
{
	m4x4r32 Result	 = M4x4r32_I;
	Result.V[0].E[3] = X;
	Result.V[1].E[3] = Y;
	Result.V[2].E[3] = Z;
	return Result;
}

internal m4x4r32
M4x4r32_Scaling(r32 X, r32 Y, r32 Z)
{
	m4x4r32 Result	 = M4x4r32_I;
	Result.V[0].E[0] = X;
	Result.V[1].E[1] = Y;
	Result.V[2].E[2] = Z;
	return Result;
}

internal m4x4r32
M4x4r32_Rotation(r32 Theta, v3r32 Axis)
{
	r32 X, Y, Z, C, S;

	v3r32 A = V3r32_Norm(Axis);
	X		= A.X;
	Y		= A.Y;
	Z		= A.Z;
	C		= R32_cos(Theta);
	S		= R32_sin(Theta);

	m4x4r32 Result;
	Result.V[0] = (v4r32) {X * X * (1 - C) + C, Y * X * (1 - C) - Z * S, Z * X * (1 - C) + Y * S, 0};
	Result.V[1] = (v4r32) {X * Y * (1 - C) + Z * S, Y * Y * (1 - C) + C, Z * Y * (1 - C) - X * S, 0};
	Result.V[2] = (v4r32) {X * Z * (1 - C) - Y * S, Y * Z * (1 - C) + X * S, Z * Z * (1 - C) + C, 0};
	Result.V[3] = (v4r32) {0, 0, 0, 1};
	return Result;
}

internal m4x4r32
M4x4r32_Transpose(m4x4r32 M)
{
	m4x4r32 Result;
	Result.V[0] = (v4r32) {M.V[0].E[0], M.V[1].E[0], M.V[2].E[0], M.V[3].E[0]};
	Result.V[1] = (v4r32) {M.V[0].E[1], M.V[1].E[1], M.V[2].E[1], M.V[3].E[1]};
	Result.V[2] = (v4r32) {M.V[0].E[2], M.V[1].E[2], M.V[2].E[2], M.V[3].E[2]};
	Result.V[3] = (v4r32) {M.V[0].E[3], M.V[1].E[3], M.V[2].E[3], M.V[3].E[3]};
	return Result;
}

internal v4r32
M4x4r32_MulMV(m4x4r32 M, v4r32 V)
{
	v4r32 Result;
	Result.X = V4r32_Dot(M.V[0], V);
	Result.Y = V4r32_Dot(M.V[1], V);
	Result.Z = V4r32_Dot(M.V[2], V);
	Result.W = V4r32_Dot(M.V[3], V);
	return Result;
}

internal m4x4r32
M4x4r32_Mul(m4x4r32 A, m4x4r32 B)
{
	B = M4x4r32_Transpose(B);

	m4x4r32 Result;
	Result.V[0].E[0] = V4r32_Dot(A.V[0], B.V[0]);
	Result.V[0].E[1] = V4r32_Dot(A.V[0], B.V[1]);
	Result.V[0].E[2] = V4r32_Dot(A.V[0], B.V[2]);
	Result.V[0].E[3] = V4r32_Dot(A.V[0], B.V[3]);
	Result.V[1].E[0] = V4r32_Dot(A.V[1], B.V[0]);
	Result.V[1].E[1] = V4r32_Dot(A.V[1], B.V[1]);
	Result.V[1].E[2] = V4r32_Dot(A.V[1], B.V[2]);
	Result.V[1].E[3] = V4r32_Dot(A.V[1], B.V[3]);
	Result.V[2].E[0] = V4r32_Dot(A.V[2], B.V[0]);
	Result.V[2].E[1] = V4r32_Dot(A.V[2], B.V[1]);
	Result.V[2].E[2] = V4r32_Dot(A.V[2], B.V[2]);
	Result.V[2].E[3] = V4r32_Dot(A.V[2], B.V[3]);
	Result.V[3].E[0] = V4r32_Dot(A.V[3], B.V[0]);
	Result.V[3].E[1] = V4r32_Dot(A.V[3], B.V[1]);
	Result.V[3].E[2] = V4r32_Dot(A.V[3], B.V[2]);
	Result.V[3].E[3] = V4r32_Dot(A.V[3], B.V[3]);
	return Result;
}

internal b08
RayPlaneIntersection(v3r32 PlanePoint, v3r32 PlaneNormal, v3r32 RayPoint, v3r32 RayDir, r32 *T)
{
	v3r32 P0 = PlanePoint;
	v3r32 N	 = PlaneNormal;

	v3r32 L0 = RayPoint;
	v3r32 L	 = RayDir;

	*T = V3r32_Dot(V3r32_Sub(P0, L0), N) / V3r32_Dot(L, N);

	return TRUE;
}

internal b08
RayRectIntersectionA(
	v3r32  RectStart,
	v3r32  RectEnd,
	v3r32  RectNormal,
	v3r32  RayPoint,
	v3r32  RayDir,
	r32	  *T,
	v3r32 *Intersection
)
{
	v3r32 P0 = RectStart;
	v3r32 N	 = RectNormal;

	v3r32 L0 = RayPoint;
	v3r32 L	 = RayDir;

	r32 LDotN = V3r32_Dot(L, N);
	if (LDotN == 0) return FALSE;

	r32 t = V3r32_Dot(V3r32_Sub(P0, L0), N) / LDotN;
	if (T) *T = t;

	v3r32 I = V3r32_Add(L0, V3r32_MulS(L, t));
	if (Intersection) *Intersection = I;

	r32 Epsilon = 0.000001;
	return R32_Within(I.X, RectStart.X, RectEnd.X, Epsilon)
		&& R32_Within(I.Y, RectStart.Y, RectEnd.Y, Epsilon)
		&& R32_Within(I.Z, RectStart.Z, RectEnd.Z, Epsilon);
}
#endif

#define DEFINE_VECTOR_INIT(N, T)      EXPORT(v##N##T,   V##N##T##_Init,        MAC_FOR(T, N, MAC_FOR_OP_SEQ_REV, MAC_FOR_FUNC_DECLVAR, MAC_FOR_ARGS_VEC))
#define DEFINE_VECTOR_ADD(N, T)       EXPORT(v##N##T,   V##N##T##_Add,         v##N##T, v##N##T)
#define DEFINE_VECTOR_ADDS(N, T)      EXPORT(v##N##T,   V##N##T##_AddS,        v##N##T, T)
#define DEFINE_VECTOR_SUB(N, T)       EXPORT(v##N##T,   V##N##T##_Sub,         v##N##T, v##N##T)
#define DEFINE_VECTOR_SUBS(N, T)      EXPORT(v##N##T,   V##N##T##_SubS,        v##N##T, T)
#define DEFINE_VECTOR_MUL(N, T)       EXPORT(v##N##T,   V##N##T##_Mul,         v##N##T, v##N##T)
#define DEFINE_VECTOR_MULS(N, T)      EXPORT(v##N##T,   V##N##T##_MulS,        v##N##T, T)
#define DEFINE_VECTOR_DIV(N, T)       EXPORT(v##N##T,   V##N##T##_Div,         v##N##T, v##N##T)
#define DEFINE_VECTOR_DIVS(N, T)      EXPORT(v##N##T,   V##N##T##_DivS,        v##N##T, T)
#define DEFINE_VECTOR_VOLUME(T, TN)   EXPORT(T,         V##3##T##_Volume,      v##3##T)
#define DEFINE_VECTOR_DOT(N, T)       EXPORT(T,         V##N##T##_Dot,         v##N##T, v##N##T)
#define DEFINE_VECTOR_CROSS_2(T)      EXPORT(T,         V##2##T##_Cross,       v##2##T, v##2##T)
#define DEFINE_VECTOR_CROSS_3(T)      EXPORT(v##3##T,   V##3##T##_Cross,       v##3##T, v##3##T)
#define DEFINE_VECTOR_CAST(N, FT, TT) EXPORT(v##N##TT,  V##N##FT##_ToV##N##TT, v##N##FT)
#define DEFINE_VECTOR_CLAMP(N, T, TN) EXPORT(v##N##T,   V##N##T##_Clamp,       v##N##T, T, T)
#define DEFINE_VECTOR_LERP(N, T, TN)  EXPORT(v##N##T,   V##N##T##_Lerp,        v##N##T, v##N##T, r32)
#define DEFINE_VECTOR_EQUAL(N, T)     EXPORT(b08,       V##N##T##_IsEqual,     v##N##T, v##N##T)
#define DEFINE_VECTOR_ISZERO(N, T)    EXPORT(b08,       V##N##T##_IsZero,      v##N##T)
#define DEFINE_VECTOR_LEN(N, T)       EXPORT(r32,       V##N##T##_Len,         v##N##T)
#define DEFINE_VECTOR_NORM(N, T)      EXPORT(v##N##r32, V##N##T##_Norm,        v##N##T)
