/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
**                                                                         **
**  Author: Aria Seiler                                                    **
**                                                                         **
**  This program is in the public domain. There is no implied warranty,    **
**  so use it at your own risk.                                            **
**                                                                         **
\* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifdef INCLUDE_HEADER

#define R32_SIGN_MASK      0x80000000
#define R32_EXPONENT_MASK  0x7F800000
#define R32_MANTISSA_MASK  0x007fffff
#define R32_MANTISSA_BITS  23
#define R32_SIGN_SHIFT     31
#define R32_EXPONENT_SHIFT 23
#define R32_EXPONENT_BIAS  127
#define R32_MAX            LITERAL_CAST(u32, 0x7F7FFFFF, r32)
#define R32_NAN(Payload)   LITERAL_CAST(u32, R32_EXPONENT_MASK | (Payload & R32_MANTISSA_MASK), r32)
#define R32_NNAN(Payload)  LITERAL_CAST(u32, R32_SIGN_MASK | R32_EXPONENT_MASK | (Payload & R32_MANTISSA_MASK), r32)
#define R32_INF            R32_NAN(0)
#define R32_NINF           R32_NNAN(0)
#define R32_PI             3.14159265359f

#define R64_SIGN_MASK      0x8000000000000000
#define R64_EXPONENT_MASK  0x7FF0000000000000
#define R64_MANTISSA_MASK  0x000FFFFFFFFFFFFF
#define R64_QUIET_MASK     0x0008000000000000
#define R64_EXPONENT_BITS  11
#define R64_MANTISSA_BITS  52
#define R64_SIGN_SHIFT     63
#define R64_EXPONENT_SHIFT 52
#define R64_EXPONENT_BIAS  1023
#define R64_EXPONENT_MAX   ((R64_EXPONENT_MASK>>R64_EXPONENT_SHIFT)-R64_EXPONENT_BIAS)
#define R64_NAN(Payload)   LITERAL_CAST(u64, R64_EXPONENT_MASK | (Payload & R64_MANTISSA_MASK), r64)
#define R64_NNAN(Payload)  LITERAL_CAST(u64, R64_SIGN_MASK | R64_EXPONENT_MASK | (Payload & R64_MANTISSA_MASK), r64)
#define R64_INF            R64_NAN(0)
#define R64_NINF           R64_NNAN(0)

#define S08_MIN (s08)(u08)0x80
#define S08_MAX (s08)(u08)0x7F
#define S16_MIN (s16)(u16)0x8000
#define S16_MAX (s16)(u16)0x7FFF
#define S32_MIN (s32)(u32)0x80000000
#define S32_MAX (s32)(u32)0x7FFFFFFF
#define S64_MIN (s64)(u64)0x8000000000000000
#define S64_MAX (s64)(u64)0x7FFFFFFFFFFFFFFF

#define U08_MAX 0xFF
#define U16_MAX 0xFFFF
#define U32_MAX 0xFFFFFFFF
#define U64_MAX 0xFFFFFFFFFFFFFFFF

#define RAND_POLYNOMIAL_MASK32 0xB4BCD35C
#define RAND_POLYNOMIAL_MASK31 0x7A5BC2E3

typedef struct random {
	u32 LFSR32;
	u32 LFSR31;
} random;

#define SCALAR_FUNCS \
   EXPORT(r32, R32_Floor, r32 N) \
   EXPORT(r32, R32_Ceil, r32 N) \
   EXPORT(r32, R32_Abs, r32 N) \
   EXPORT(r32, R32_Sign, r32 N) \
   EXPORT(r32, R32_Round, r32 N) \
   EXPORT(s32, R32_Exponent, r32 N) \
   EXPORT(r32, R32_Max, r32 A, r32 B) \
   EXPORT(r32, R32_Min, r32 A, r32 B) \
   EXPORT(r32, R32_Median, r32 A, r32 B, r32 C) \
   EXPORT(r32, R32_Lerp, r32 S, r32 E, r32 t) \
   EXPORT(r32, R32_Remainder, r32 Dividend, r32 Divisor) \
   EXPORT(b08, R32_Within, r32 Num, r32 Start, r32 End, r32 Epsilon) \
   EXPORT(r32, R32_Clamp, r32 N, r32 S, r32 E) \
   EXPORT(r32, R32_sqrt, r32 N) \
   EXPORT(r32, R32_cbrt, r32 N) \
   EXPORT(r32, R32_sin, r32 R) \
   EXPORT(r32, R32_cos, r32 R) \
   EXPORT(r32, R32_tan, r32 R) \
   EXPORT(r32, R32_arccos, r32 N) \
   EXPORT(u32, R32_SolveLinear, r32 C1, r32 C0, r32 *Roots) \
   EXPORT(u32, R32_SolveQuadratic, r32 C2, r32 C1, r32 C0, r32 *Roots) \
   EXPORT(u32, R32_SolveCubic, r32 C3, r32 C2, r32 C1, r32 C0, r32 *Roots) \
   EXPORT(s32, S32_Sign, s32 N) \
   EXPORT(s32, S32_Abs, s32 N) \
   EXPORT(s32, S32_Clamp, s32 N, s32 S, s32 E) \
   EXPORT(u08, U08_Max, u08 A, u08 B) \
   EXPORT(u08, U08_Min, u08 A, u08 B) \
   EXPORT(u08, U08_Median, u08 A, u08 B, u08 C) \
   EXPORT(u08, U08_Lerp, u08 A, u08 B, r32 T) \
   EXPORT(u32, U32_Min, u32 A, u32 B) \
   EXPORT(u32, U32_Max, u32 A, u32 B) \
   EXPORT(u32, U32_RoundUpPow2, u32 N) \
   EXPORT(random, Rand_Init, u32 Seed) \
   EXPORT(u32, Rand_Next, random *Random) \
   EXPORT(s32, S32_RandRange, random *Random, s32 Min, s32 Max) \
   EXPORT(r32, R32_Random, random *Random) \
   EXPORT(r32, R32_RandRange, random *Random, r32 Min, r32 Max)

#endif

#ifdef INCLUDE_SOURCE

global r32 SinLookupTable[]
	= {0.00000000000, 0.01745240644, 0.03489949670, 0.05233595626, 0.06975647374, 0.08715574277, 0.1045284632,
	   0.1218693435,  0.1391731010,	 0.1564344651,	0.1736481777,  0.1908089954,  0.2079116909,	 0.2249510544,
	   0.2419218956,  0.2588190451,	 0.2756373559,	0.2923717048,  0.3090169944,  0.3255681547,	 0.3420201433,
	   0.3583679497,  0.3746065934,	 0.3907311286,	0.4067366430,  0.4226182618,  0.4383711467,	 0.4539904998,
	   0.4694715630,  0.4848096203,	 0.5000000000,	0.5150380749,  0.5299192644,  0.5446390350,	 0.5591929036,
	   0.5735764363,  0.5877852524,	 0.6018150234,	0.6156614754,  0.6293203912,  0.6427876097,	 0.6560590291,
	   0.6691306063,  0.6819983602,	 0.6946583704,	0.7071067810,  0.7071067810};

global r32 CosLookupTable[]
	= {1.0000000000, 0.9998476952, 0.9993908270, 0.9986295348, 0.9975640503, 0.9961946981, 0.9945218954,
	   0.9925461516, 0.9902680687, 0.9876883406, 0.9848077530, 0.9816271835, 0.9781476007, 0.9743700648,
	   0.9702957263, 0.9659258263, 0.9612616959, 0.9563047559, 0.9510565163, 0.9455185755, 0.9396926208,
	   0.9335804264, 0.9271838546, 0.9205048534, 0.9135454577, 0.9063077870, 0.8987940464, 0.8910065242,
	   0.8829475927, 0.8746197071, 0.8660254040, 0.8571673007, 0.8480480961, 0.8386705680, 0.8290375725,
	   0.8191520443, 0.8090169943, 0.7986355099, 0.7880107536, 0.7771459613, 0.7660444431, 0.7547095801,
	   0.7431448255, 0.7313537015, 0.7193398004, 0.7071067810, 0.7071067810};

global r32 ArccosLookupTable[]
	= {1.570796327,	 1.565240743,  1.559684987,	 1.554128888,  1.548572275,	 1.543014976,  1.537456818,
	   1.531897629,	 1.526337237,  1.520775470,	 1.515212154,  1.509647114,	 1.504080178,  1.498511171,
	   1.492939917,	 1.487366240,  1.481789964,	 1.476210912,  1.470628906,	 1.465043766,  1.459455312,
	   1.453863365,	 1.448267743,  1.442668261,	 1.437064737,  1.431456986,	 1.425844821,  1.420228054,
	   1.414606497,	 1.408979959,  1.403348248,	 1.397711170,  1.392068532,	 1.386420136,  1.380765783,
	   1.375105274,	 1.369438406,  1.363764975,	 1.358084776,  1.352397599,	 1.346703235,  1.341001470,
	   1.335292090,	 1.329574877,  1.323849612,	 1.318116072,  1.312374030,	 1.306623261,  1.300863531,
	   1.295094608,	 1.289316254,  1.283528228,	 1.277730288,  1.271922187,	 1.266103673,  1.260274492,
	   1.254434387,	 1.248583095,  1.242720350,	 1.236845882,  1.230959417,	 1.225060676,  1.219149374,
	   1.213225223,	 1.207287930,  1.201337197,	 1.195372719,  1.189394187,	 1.183401286,  1.177393695,
	   1.171371087,	 1.165333129,  1.159279481,	 1.153209796,  1.147123721,	 1.141020895,  1.134900951,
	   1.128763510,	 1.122608191,  1.116434599,	 1.110242335,  1.104030988,	 1.097800138,  1.091549357,
	   1.085278204,	 1.078986232,  1.072672979,	 1.066337974,  1.059980734,	 1.053600762,  1.047197551,
	   1.040770580,	 1.034319313,  1.027843202,	 1.021341682,  1.014814174,	 1.008260082,  1.001678794,
	   0.9950696789, 0.9884320889, 0.9817653565, 0.9750687942, 0.9683416934, 0.9615833238, 0.9547929322,
	   0.9479697414, 0.9411129491, 0.9342217268, 0.9272952180, 0.9203325377, 0.9133327704, 0.9062949682,
	   0.8992181502, 0.8921012994, 0.8849433622, 0.8777432452, 0.8704998140, 0.8632118901, 0.8558782490,
	   0.8484976176, 0.8410686705, 0.8335900284, 0.8260602531, 0.8184778457, 0.8108412411, 0.8031488054,
	   0.7953988302, 0.7875895284, 0.7797190290, 0.7717853706, 0.7637864964, 0.7557202462, 0.7475843497,
	   0.7393764180, 0.7310939353, 0.7227342478, 0.7142945546, 0.7057718946, 0.6971631336, 0.6884649502,
	   0.6796738189, 0.6707859928, 0.6617974828, 0.6527040359, 0.6435011088, 0.6341838407, 0.6247470206,
	   0.6151850501, 0.6054919033, 0.5956610782, 0.5856855435, 0.5755576750, 0.5652691841, 0.5548110330,
	   0.5441733370, 0.5333452489, 0.5223148217, 0.5110688475, 0.4995926610, 0.4878699065, 0.4758822496,
	   0.4636090277, 0.4510268118, 0.4381088610, 0.4248244275, 0.4111378622, 0.3970074493, 0.3823838531,
	   0.3672080207, 0.3514082698, 0.3348961586, 0.3175604293, 0.2992578186, 0.2797984760, 0.2589215419,
	   0.2362513061, 0.2112108803, 0.1828287169, 0.1492095735, 0.1054581174, 0.0000000000, 0.0000000000};

internal r32
R32_Floor(r32 N)
{
	if (N > 0) return (r32) ((s32) N);
	if (N - (s32) N == 0) return N;
	return (r32) ((s32) N - 1);
}

internal r32
R32_Ceil(r32 N)
{
	if (N < 0) return (r32) ((s32) N);
	if (N - (s32) N == 0) return N;
	return (r32) ((s32) N + 1);
}

internal r32
R32_Abs(r32 N)
{
	u32 Binary	= FORCE_CAST(u32, N);
	Binary	   &= 0x7FFFFFFF;
	return FORCE_CAST(r32, Binary);
}

internal r32
R32_Sign(r32 N)
{
	u32 Binary	= FORCE_CAST(u32, N);
	Binary	   &= 0x80000000;
	Binary	   |= 0x3F800000;
	return FORCE_CAST(r32, Binary);
}

internal r32
R32_Round(r32 N)
{
	return (r32) (s32) (N + R32_Sign(N) * 0.5);
}

internal s32
R32_Exponent(r32 N)
{
	u32 Binary	 = FORCE_CAST(u32, N);
	Binary		&= 0x7F800000;
	Binary	   >>= 23;
	s32 Signed	 = (s32) Binary - 127;
	return Signed;
}

// TODO make defines for these (like with the vectors)
internal r32
R32_Max(r32 A, r32 B)
{
	return (A < B) ? B : A;
}

internal r32
R32_Min(r32 A, r32 B)
{
	return (A < B) ? A : B;
}

internal r32
R32_Median(r32 A, r32 B, r32 C)
{
	return R32_Max(R32_Min(A, B), R32_Min(R32_Max(A, B), C));
}

internal r32
R32_Lerp(r32 S, r32 E, r32 t)
{
	return S + t * (E - S);
}

internal r32
R32_Remainder(r32 Dividend, r32 Divisor)
{
	return Dividend - ((s32) (Dividend / Divisor) * Divisor);
}

internal b08
R32_Within(r32 Num, r32 Start, r32 End, r32 Epsilon)
{
	return Start - Epsilon < Num && Num < End + Epsilon;
}

internal r32
R32_Clamp(r32 N, r32 S, r32 E)
{
	if (N < S) return S;
	if (N > E) return E;
	return N;
}

internal r32
R32_sqrt(r32 N)
{
	return Intrin_Sqrt_R32(N);
}

#define R32_CBRT_ITERATIONS 10
internal r32
R32_cbrt(r32 N)
{
	u32 Binary	 = FORCE_CAST(u32, N);
	s32 Exponent = (R32_Exponent(N) / 3) + 127;
	Binary		 = (Binary & ~R32_EXPONENT_MASK) | (Exponent << 23);
	r32 X		 = FORCE_CAST(r32, Binary);
	for (u32 I = 0; I < R32_CBRT_ITERATIONS; I++) X = (N / (X * X) + 2 * X) / 3;
	return X;
}

internal r32
R32_sin(r32 R)
{
	r32 D, F, R1, R2, Result, *LUT;
	u32 I;
	b08 N = FALSE;

	D = R * 180 / R32_PI;
	if (D < 0) {
		N = !N;
		D = -D;
	}
	if (D >= 360) D = (u32) D % 360;
	if (D >= 270) {
		N = !N;
		D = 360 - D;
	}
	if (D >= 180) {
		N  = !N;
		D -= 180;
	}
	if (D > 90) D = 180 - D;
	if (D > 45) {
		D	= 90 - D;
		LUT = CosLookupTable;
	} else LUT = SinLookupTable;

	I	   = (u32) D;
	F	   = D - (r32) I;
	R1	   = LUT[I];
	R2	   = LUT[I + 1];
	Result = R32_Lerp(R1, R2, F);

	return (N) ? -Result : Result;
}

internal r32
R32_cos(r32 R)
{
	r32 D, F, R1, R2, Result, *LUT;
	u32 I;
	b08 N = FALSE;

	D = R * 180 / R32_PI;
	if (D < 0) D = -D;
	if (D >= 360) D = (u32) D % 360;
	if (D >= 270) D = 360 - D;
	if (D >= 180) {
		N  = TRUE;
		D -= 180;
	}
	if (D > 90) {
		N = TRUE;
		D = 180 - D;
	}
	if (D > 45) {
		D	= 90 - D;
		LUT = SinLookupTable;
	} else LUT = CosLookupTable;

	I	   = (u32) D;
	F	   = D - (r32) I;
	R1	   = LUT[I];
	R2	   = LUT[I + 1];
	Result = R32_Lerp(R1, R2, F);

	return (N) ? -Result : Result;
}

internal r32
R32_tan(r32 R)
{
	// TODO: Make this use a lookup table
	return R32_sin(R) / R32_cos(R);
}

internal r32
R32_arccos(r32 N)
{
	if (N > 1) return R32_INF;
	if (N < -1) return -R32_INF;

	r32 F, R1, R2, Result;
	u32 I;
	b08 S = FALSE;

	if (N < 0) {
		S = TRUE;
		N = -N;
	}

	N	   *= sizeof(ArccosLookupTable) / sizeof(ArccosLookupTable[0]) - 1;
	I		= (u32) N;
	F		= N - (r32) I;
	R1		= ArccosLookupTable[I];
	R2		= ArccosLookupTable[I + 1];
	Result	= R32_Lerp(R1, R2, F);

	return (S) ? R32_PI - Result : Result;
}

internal u32
R32_SolveLinear(r32 C1, r32 C0, r32 *Roots)
{
	if (C1 == 0) return 0;
	C0 /= C1;

	Roots[0] = -C0;
	return 1;
}

internal u32
R32_SolveQuadratic(r32 C2, r32 C1, r32 C0, r32 *Roots)
{
	if (C2 == 0) return R32_SolveLinear(C1, C0, Roots);
	C0 /= C2;
	C1 /= C2;

	r32 C1C1 = C1 * C1;
	r32 D	 = C1C1 - 4 * C0;
	if (D == 0) {
		Roots[0] = -C1 / 2;
		return 1;
	}
	if (D > 0) {
		r32 sqrtD = R32_sqrt(D);
		Roots[0]  = (-C1 + sqrtD) / 2;
		Roots[1]  = (-C1 - sqrtD) / 2;
		return 2;
	}
	return 0;
}

internal u32
R32_SolveCubic(r32 C3, r32 C2, r32 C1, r32 C0, r32 *Roots)
{
	if (C3 == 0) return R32_SolveQuadratic(C2, C1, C0, Roots);
	C0 /= C3;
	C1 /= C3;
	C2 /= C3;

	r32 C2C2 = C2 * C2;
	r32 Q	 = (3 * C1 - C2C2) / 9;
	r32 R	 = (9 * C2 * C1 - 27 * C0 - 2 * C2C2 * C2) / 54;
	r32 QQQ	 = Q * Q * Q;
	r32 D	 = QQQ + R * R;
	r32 C2d3 = C2 / 3;
	if (D > 0) {
		r32 S	 = R32_cbrt(R + R32_sqrt(D));
		r32 T	 = R32_cbrt(R - R32_sqrt(D));
		Roots[0] = S + T - C2d3;
		return 1;
	}
	if (D == 0) {
		r32 SpT	 = 2 * R32_cbrt(R);
		Roots[0] = SpT - C2d3;
		if (SpT == 0) return 1;
		Roots[1] = -SpT / 2 - C2d3;
		return 2;
	}
	r32 Theta	 = R32_arccos(R / R32_sqrt(-QQQ));
	r32 sqrtnQt2 = 2 * R32_sqrt(-Q);
	Roots[0]	 = sqrtnQt2 * R32_cos(Theta / 3) - C2d3;
	Roots[1]	 = sqrtnQt2 * R32_cos((Theta + 2 * R32_PI) / 3) - C2d3;
	Roots[2]	 = sqrtnQt2 * R32_cos((Theta + 4 * R32_PI) / 3) - C2d3;
	return 3;
}

internal s32
S32_Sign(s32 N)
{
	return 1 + 2 * (N >> 31);
}

internal s32
S32_Abs(s32 N)
{
	return N * S32_Sign(N);
}

internal s32
S32_Clamp(s32 N, s32 S, s32 E)
{
	if (N < S) return S;
	if (N > E) return E;
	return N;
}

internal u08
U08_Max(u08 A, u08 B)
{
	return (A < B) ? B : A;
}

internal u08
U08_Min(u08 A, u08 B)
{
	return (A < B) ? A : B;
}

internal u08
U08_Median(u08 A, u08 B, u08 C)
{
	return U08_Max(U08_Min(A, B), U08_Min(U08_Max(A, B), C));
}

internal u08
U08_Lerp(u08 A, u08 B, r32 T)
{
	return A + (u08) (T * ((r32) B - A));
}

internal u32
U32_Max(u32 A, u32 B)
{
	return (A > B) ? A : B;
}

internal u32
U32_Min(u32 A, u32 B)
{
	return (A < B) ? A : B;
}

internal u32
U32_Abs(u32 N)
{
	return N;
}

internal u32
U32_RoundUpPow2(u32 N)
{
	N--;
	N |= N >> 1;
	N |= N >> 2;
	N |= N >> 4;
	N |= N >> 8;
	N |= N >> 16;
	N++;
	return N;
}

internal random
Rand_Init(u32 Seed)
{
	random Random;
	Random.LFSR32 = Seed % U32_MAX;						  // 32 Bit
	Random.LFSR31 = (Seed + 0x2F2F2F2F) % (U32_MAX / 2);  // 31 Bit
	return Random;
}

// TODO: Rework this

#define RAND_MAX 0xFFFF

internal u32
Rand_Next(random *Random)
{
	s32 FeedBack;
	u32 LFSR32 = Random->LFSR32;

	FeedBack   = LFSR32 & 1;
	LFSR32	 >>= 1;
	if (FeedBack == 1) LFSR32 ^= RAND_POLYNOMIAL_MASK32;
	FeedBack   = LFSR32 & 1;
	LFSR32	 >>= 1;
	if (FeedBack == 1) LFSR32 ^= RAND_POLYNOMIAL_MASK32;
	Random->LFSR32 = LFSR32;

	u32 LFSR31	 = Random->LFSR31;
	FeedBack	 = LFSR31 & 1;
	LFSR31	   >>= 1;
	if (FeedBack == 1) LFSR31 ^= RAND_POLYNOMIAL_MASK31;
	Random->LFSR31 = LFSR31;

	return (LFSR32 ^ LFSR31) & 0xFFFF;
}

// NOTE: Max is exclusive, so Min <= Result < Max
internal s32
S32_RandRange(random *Random, s32 Min, s32 Max)
{
	Assert(Min < Max);
	return Rand_Next(Random) % (Max - Min) + Min;
}

internal r32
R32_Random(random *Random)
{
	u32 Value = Rand_Next(Random);
	return (r32) Value / RAND_MAX;
}

// NOTE: Max is exclusive, so Min <= Result < Max
internal r32
R32_RandRange(random *Random, r32 Min, r32 Max)
{
	Assert(Min < Max);
	return R32_Random(Random) * (Max - Min) + Min;
}

#endif
