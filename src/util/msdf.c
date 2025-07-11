/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
**                                                                         **
**  Author: Aria Seiler                                                    **
**                                                                         **
**  This program is in the public domain. There is no implied warranty,    **
**  so use it at your own risk.                                            **
**                                                                         **
\* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifdef INCLUDE_HEADER

#define MSDF_PIXEL_HAS_ERROR 1

typedef struct msdf_dist {
	r32 Distance;
	r32 Orthogonality;
} msdf_dist;

typedef struct msdf_segment {
	v2r32 P1;
	v2r32 P2;
	v2r32 C1;
	u08	  CPCount;
} msdf_segment;

typedef struct msdf_edge {
	msdf_segment *Segments;
	u32			  SegmentCount;
	u08			  Color;
} msdf_edge;

typedef struct msdf_contour {
	msdf_edge *Edges;
	u32		   EdgeCount;
} msdf_contour;

typedef struct msdf_shape {
	msdf_contour *Contours;
	msdf_edge	 *Edges;
	msdf_segment *Segments;
	u32			  ContourCount;
	u32			  EdgeCount;
	u32			  SegmentCount;

	v4s16 Bounds;
} msdf_shape;

#define MSDF_FUNCS \
    EXPORT(void, MSDF_DrawShape, msdf_shape Shape, v2u32 *_SlotPos, v2u32 *_SlotSize, v4u08 *Bitmap, v2u32 BitmapOffset, u32 BitmapIndex, v2u32 BitmapSize)

#endif

#ifdef INCLUDE_SOURCE

internal v2r32
MSDF_SegmentDir(msdf_segment Segment, r32 t)
{
	v2r32 P1 = Segment.P1;
	v2r32 P2 = Segment.P2;
	v2r32 C1 = Segment.C1;
	switch (Segment.CPCount) {
		case 0: {
			return V2r32_Norm(V2r32_Sub(P2, P1));
		}
		case 1: {
			v2r32 C1mP1t2	= V2r32_MulS(V2r32_Sub(C1, P1), 2);
			v2r32 P2m2C1pP1 = V2r32_Add(V2r32_Add(P2, V2r32_MulS(C1, -2)), P1);
			return V2r32_Norm(V2r32_Add(V2r32_MulS(P2m2C1pP1, 2 * t), C1mP1t2));
		}
	}
	return (v2r32) {0};
}

internal v2r32
MSDF_SegmentPoint(msdf_segment Segment, r32 t)
{
	v2r32 P1 = Segment.P1;
	v2r32 P2 = Segment.P2;
	v2r32 C1 = Segment.C1;
	switch (Segment.CPCount) {
		case 0: {
			return V2r32_Add(P1, V2r32_MulS(V2r32_Sub(P2, P1), t));
		}
		case 1: {
			v2r32 Part1 = P1;
			v2r32 Part2 = V2r32_MulS(V2r32_Sub(C1, P1), 2 * t);
			v2r32 Part3 = V2r32_MulS(V2r32_Add(V2r32_Sub(P2, V2r32_MulS(C1, 2)), P1), t * t);
			return V2r32_Add(V2r32_Add(Part1, Part2), Part3);
		}
	}
	return (v2r32) {0};
}

internal s08
MSDF_Cmp(msdf_dist A, msdf_dist B)
{
	r32 ADist = R32_Abs(A.Distance);
	r32 BDist = R32_Abs(B.Distance);
	if (ADist < BDist) return LESS;
	if (ADist > BDist) return GREATER;
	if (A.Orthogonality > B.Orthogonality) return LESS;
	if (A.Orthogonality < B.Orthogonality) return GREATER;
	return EQUAL;
}

internal u32
MSDF_LinearSegmentDistance(v2r32 P, msdf_segment Segment, r32 *ts)
{
	v2r32 PmP1	= V2r32_Sub(P, Segment.P1);
	v2r32 P2mP1 = V2r32_Sub(Segment.P2, Segment.P1);
	r32	  Numer = V2r32_Dot(PmP1, P2mP1);
	r32	  Denom = V2r32_Dot(P2mP1, P2mP1);
	ts[0]		= Numer / Denom;
	return 1;
}

internal u32
MSDF_SegmentDistances(v2r32 P, msdf_segment Segment, r32 *ts)
{
	v2r32 P1 = Segment.P1;
	v2r32 P2 = Segment.P2;
	v2r32 C1 = Segment.C1;
	switch (Segment.CPCount) {
		case 0: {
			return MSDF_LinearSegmentDistance(P, Segment, ts);
		}
		case 1: {
			v2r32 p	 = V2r32_Sub(P, P1);
			v2r32 p1 = V2r32_Sub(C1, P1);
			v2r32 p2 = V2r32_Add(V2r32_Sub(P2, V2r32_MulS(C1, 2)), P1);
			r32	  A	 = V2r32_Dot(p2, p2);
			r32	  B	 = 3 * V2r32_Dot(p1, p2);
			r32	  C	 = 2 * V2r32_Dot(p1, p1) - V2r32_Dot(p2, p);
			r32	  D	 = -V2r32_Dot(p1, p);

			return R32_SolveCubic(A, B, C, D, ts);
		}
	}
	Assert(FALSE, "CPCount greater than 1");
	return 0;
}

internal msdf_dist
MSDF_EdgeSignedDistance(v2r32 P, msdf_edge Edge, msdf_segment *SegmentOut)
{
	msdf_dist MinDist = {R32_MAX, 0};
	for (u32 S = 0; S < Edge.SegmentCount; S++) {
		msdf_segment Segment = Edge.Segments[S];
		r32			 ts[5]	 = {0, 1};
		u32			 Count	 = MSDF_SegmentDistances(P, Segment, ts + 2);
		for (u32 D = 0; D < Count + 2; D++) {
			r32 t = ts[D];
			if (t < 0 || t > 1) continue;
			v2r32 Dir		   = MSDF_SegmentDir(Segment, t);
			v2r32 Point		   = MSDF_SegmentPoint(Segment, t);
			v2r32 DistVec	   = V2r32_Sub(P, Point);
			r32	  Dist		   = V2r32_Len(DistVec);
			r32	  Ortho		   = R32_Abs(V2r32_Cross(Dir, V2r32_DivS(DistVec, Dist)));
			r32	  Sign		   = V2r32_Cross(Dir, V2r32_Sub(Point, P));
			Dist			  *= R32_Sign(Sign);
			msdf_dist NewDist  = {Dist, Ortho};
			if (MSDF_Cmp(NewDist, MinDist) == LESS) {
				MinDist		= NewDist;
				*SegmentOut = Segment;
			}
		}
	}
	return MinDist;
}

internal r32
_MSDF_SignedPseudoDistance(v2r32 P, msdf_segment Segment, r32 t)
{
	v2r32 Dir	  = MSDF_SegmentDir(Segment, t);
	v2r32 Point	  = MSDF_SegmentPoint(Segment, t);
	v2r32 EdgeToP = V2r32_Sub(P, Point);
	r32	  Dist	  = V2r32_Cross(EdgeToP, Dir);
	return Dist;
}

internal r32
MSDF_SignedPseudoCubicDistances(v2r32 P, msdf_segment Segment, r32 *Dists)
{
	persist r32 LowEpsilon	= -0.00002;
	persist r32 HighEpsilon = 1.00002;

	u32 Count = 0;
	r32 Roots[3];

	u32 CubicCount = MSDF_SegmentDistances(P, Segment, Roots);
	for (u32 R = 0; R < CubicCount; R++)
		if (LowEpsilon <= Roots[R] && Roots[R] <= HighEpsilon)
			Dists[Count++] = _MSDF_SignedPseudoDistance(P, Segment, Roots[R]);

	msdf_segment LinearSegment = {0};
	LinearSegment.P1		   = Segment.P1;
	LinearSegment.P2		   = Segment.C1;
	MSDF_LinearSegmentDistance(P, LinearSegment, Roots);
	if (Roots[0] < LowEpsilon) Dists[Count++] = _MSDF_SignedPseudoDistance(P, LinearSegment, Roots[0]);

	LinearSegment.P1 = Segment.C1;
	LinearSegment.P2 = Segment.P2;
	MSDF_LinearSegmentDistance(P, LinearSegment, Roots);
	if (Roots[0] > HighEpsilon) Dists[Count++] = _MSDF_SignedPseudoDistance(P, LinearSegment, Roots[0]);

	if (Count == 0)
		for (u32 R = 0; R < CubicCount; R++)
			Dists[Count++] = _MSDF_SignedPseudoDistance(P, Segment, Roots[R]);

	return Count;
}

internal r32
MSDF_SignedPseudoDistance(v2r32 P, msdf_segment Segment)
{
	r32 MinDist = R32_MAX;
	r32 Dists[5];
	u32 Count;

	if (Segment.CPCount == 0) {
		r32 t;
		Count	 = MSDF_LinearSegmentDistance(P, Segment, &t);
		Dists[0] = _MSDF_SignedPseudoDistance(P, Segment, t);
	} else {
		Count = MSDF_SignedPseudoCubicDistances(P, Segment, Dists);
	}

	for (u32 D = 0; D < Count; D++)
		if (R32_Abs(Dists[D]) < R32_Abs(MinDist)) MinDist = Dists[D];

	return MinDist;
}

internal v4u08
MSDF_DistanceColor(v4r32 Dists, r32 Range)
{
	Dists = V4r32_DivS(Dists, Range);
	Dists = V4r32_AddS(Dists, 0.5);
	Dists = V4r32_Clamp(Dists, 0, 1);
	Dists = V4r32_MulS(Dists, 255);
	return (v4u08) {(u08) Dists.X, (u08) Dists.Y, (u08) Dists.Z, (u08) Dists.W};
}

internal b08
MSDF_HasSideError(v4r32 A, v4r32 B, r32 Threshold)
{
	u32 Count = 0;
	for (u32 C = 0; C < 4; C++) {
		r32 CA = A.E[C];
		r32 CB = B.E[C];

		if (R32_Abs(CA) < Threshold && R32_Abs(CB) < Threshold) continue;
		if (R32_Sign(CA) != R32_Sign(CB)) Count++;
	}

	return (Count >= 2) ? TRUE : FALSE;
}

internal void
MSDF_FindErrors(u08 *ErrorMap, v4r32 *FloatMap, v2u32 Size, r32 Threshold)
{
	for (u32 Y = 0; Y < Size.Y; Y++) {
		for (u32 X = 0; X < Size.X; X++) {
			v4r32 P, L, B, R, T, LB, LT, RB, RT;
			b08	  HasL, HasB, HasR, HasT, HasLB, HasLT, HasRB, HasRT;
			b08	  LE, BE, RE, TE, LBE, LTE, RBE, RTE, E;

			HasL  = X > 0;
			HasB  = Y > 0;
			HasR  = X < Size.X - 1;
			HasT  = Y < Size.Y - 1;
			HasLB = HasL && HasB;
			HasLT = HasL && HasT;
			HasRB = HasR && HasB;
			HasRT = HasR && HasT;

			P = FloatMap[INDEX_2D(X, Y, Size.X)];

			L  = HasL ? FloatMap[INDEX_2D(X - 1, Y, Size.X)] : (v4r32) {0};
			B  = HasB ? FloatMap[INDEX_2D(X, Y - 1, Size.X)] : (v4r32) {0};
			R  = HasR ? FloatMap[INDEX_2D(X + 1, Y, Size.X)] : (v4r32) {0};
			T  = HasT ? FloatMap[INDEX_2D(X, Y + 1, Size.X)] : (v4r32) {0};
			LB = HasLB ? FloatMap[INDEX_2D(X - 1, Y - 1, Size.X)] : (v4r32) {0};
			LT = HasLT ? FloatMap[INDEX_2D(X - 1, Y + 1, Size.X)] : (v4r32) {0};
			RB = HasRB ? FloatMap[INDEX_2D(X + 1, Y - 1, Size.X)] : (v4r32) {0};
			RT = HasRT ? FloatMap[INDEX_2D(X + 1, Y + 1, Size.X)] : (v4r32) {0};

			LE = HasL && MSDF_HasSideError(P, L, Threshold);
			BE = HasB && MSDF_HasSideError(P, B, Threshold);
			RE = HasR && MSDF_HasSideError(P, R, Threshold);
			TE = HasT && MSDF_HasSideError(P, T, Threshold);

			LBE = HasLB && MSDF_HasSideError(P, LB, Threshold);
			LTE = HasLT && MSDF_HasSideError(P, LT, Threshold);
			RBE = HasRB && MSDF_HasSideError(P, RB, Threshold);
			RTE = HasRT && MSDF_HasSideError(P, RT, Threshold);

			E								  = LE | BE | RE | TE | LBE | LTE | RBE | RTE;
			ErrorMap[INDEX_2D(X, Y, Size.X)] |= MSDF_PIXEL_HAS_ERROR * E;
		}
	}
}

internal void
MSDF_FixErrors(u08 *ErrorMap, v4r32 *FloatMap, v2u32 Size)
{
	for (u32 Y = 0; Y < Size.Y; Y++) {
		for (u32 X = 0; X < Size.X; X++) {
			u32	  I = INDEX_2D(X, Y, Size.X);
			v4r32 F = FloatMap[I];
			if (ErrorMap[I] & MSDF_PIXEL_HAS_ERROR) {
				r32 M		= R32_Median(F.X, F.Y, F.Z);
				FloatMap[I] = (v4r32) {M, M, M, F.W};
			}
		}
	}
}

internal void
MSDF_DrawShape(
	msdf_shape Shape,
	v2u32	  *_SlotPos,
	v2u32	  *_SlotSize,
	v4u08	  *Bitmap,
	v2u32	   BitmapOffset,
	u32		   BitmapIndex,
	v2u32	   BitmapSize
)
{
	/* TODO Possible things to add
	   - Short edge merging
	*/

	v2u32 SlotSize	= *_SlotSize;
	v4s16 Bounds	= Shape.Bounds;
	v2r32 Offset	= {1, 1};
	*_SlotPos		= V2u32_Add(*_SlotPos, (v2u32) {1, 1});
	*_SlotSize		= V2u32_Sub(*_SlotSize, (v2u32) {2, 2});
	v2r32 SlotSizeR = {SlotSize.X, SlotSize.Y};
	v2r32 Size		= V2r32_Sub(SlotSizeR, V2r32_MulS(Offset, 2));
	v2r32 MaxBounds = {Bounds.Z - Bounds.X, Bounds.W - Bounds.Y};
	v2r32 Scale		= V2r32_Div(Size, MaxBounds);
	r32	  Range		= 4 / (Scale.X + Scale.Y);

	v4r32 *FloatMap = Stack_Allocate(SlotSize.X * SlotSize.Y * sizeof(v4r32));

	for (u32 Y = 0; Y < SlotSize.Y; Y++) {
		for (u32 X = 0; X < SlotSize.X; X++) {
			v2r32 P = {(X - Offset.X + 0.5) / Scale.X + Bounds.X, (Y - Offset.Y + 0.5) / Scale.Y + Bounds.Y};

			msdf_dist Dists[3] = {
				{R32_MAX, 1},
				{R32_MAX, 1},
				{R32_MAX, 1}
			};
			msdf_edge Edges[3]
				= {Shape.Contours[0].Edges[0], Shape.Contours[0].Edges[0], Shape.Contours[0].Edges[0]};
			msdf_segment Segments[3] = {Edges[0].Segments[0], Edges[0].Segments[0], Edges[0].Segments[0]};

			for (u32 C = 0; C < Shape.ContourCount; C++) {
				msdf_contour Contour = Shape.Contours[C];
				for (u32 E = 0; E < Contour.EdgeCount; E++) {
					msdf_edge	 Edge = Contour.Edges[E];
					msdf_segment Segment;
					msdf_dist	 Dist = MSDF_EdgeSignedDistance(P, Edge, &Segment);
					if ((Edge.Color & 0b100) && MSDF_Cmp(Dist, Dists[0]) == LESS) {
						Dists[0]	= Dist;
						Edges[0]	= Edge;
						Segments[0] = Segment;
					}
					if ((Edge.Color & 0b010) && MSDF_Cmp(Dist, Dists[1]) == LESS) {
						Dists[1]	= Dist;
						Edges[1]	= Edge;
						Segments[1] = Segment;
					}
					if ((Edge.Color & 0b001) && MSDF_Cmp(Dist, Dists[2]) == LESS) {
						Dists[2]	= Dist;
						Edges[2]	= Edge;
						Segments[2] = Segment;
					}
				}
			}

			v4r32 FinalDists;
			FinalDists.X = MSDF_SignedPseudoDistance(P, Segments[0]);
			FinalDists.Y = MSDF_SignedPseudoDistance(P, Segments[1]);
			FinalDists.Z = MSDF_SignedPseudoDistance(P, Segments[2]);

			msdf_dist TrueDist = Dists[0];
			if (MSDF_Cmp(Dists[1], TrueDist) == LESS) TrueDist = Dists[1];
			if (MSDF_Cmp(Dists[2], TrueDist) == LESS) TrueDist = Dists[2];
			FinalDists.W = TrueDist.Distance;

			FloatMap[INDEX_2D(X, Y, (u32) SlotSize.X)] = FinalDists;
		}
	}

	u08 *ErrorMap = Stack_Allocate(SlotSize.X * SlotSize.Y);
	Mem_Set(ErrorMap, 0, SlotSize.X * SlotSize.Y);

	r32 ThresholdModifier = 0.72;
	r32 Threshold		  = Range * ThresholdModifier;
	MSDF_FindErrors(ErrorMap, FloatMap, SlotSize, Threshold);
	MSDF_FixErrors(ErrorMap, FloatMap, SlotSize);

	for (u32 Y = 0; Y < SlotSize.Y; Y++) {
		for (u32 X = 0; X < SlotSize.X; X++) {
			v4r32 Floats						 = FloatMap[INDEX_2D(X, Y, SlotSize.X)];
			v4u08 Color							 = MSDF_DistanceColor(Floats, Range);
			Bitmap[INDEX_2D(X, Y, BitmapSize.X)] = Color;
		}
	}
}

#endif
