/********************************************************************************
	Copyright (C) 2004-2005 Sjaak Priester	
	Copyright (C) 2011 Edwin van Leeuwen

	This is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.

	This file is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this application; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
********************************************************************************/
#ifndef DELAUNAY_H
#define DELAUNAY_H
#include <set>
#include <algorithm>
#include <limits>
#include <math.h>

using namespace std;

namespace realtimeplot {
	namespace delaunay {

// I designed this with GDI+ in mind. However, this particular code doesn't
// use GDI+ at all, only some of it's variable types.
// These definitions are substitutes for those of GDI+. 
struct PointF
{
	PointF() : X(0), Y(0)	{}
	PointF(const PointF& p) : X(p.X), Y(p.Y)	{}
	PointF(float x, float y) : X(x), Y(y)	{}
	PointF operator+(const PointF& p) const	{ return PointF(X + p.X, Y + p.Y); }
	PointF operator-(const PointF& p) const	{ return PointF(X - p.X, Y - p.Y); }
	float X;
	float Y;
};

const float float_EPSILON = 10*numeric_limits<float>::epsilon();
///////////////////
// vertex

class vertex
{
public:
	vertex()					: m_Pnt(0.0F, 0.0F)			{}
	vertex(const vertex& v)		: m_Pnt(v.m_Pnt)			{}
	vertex(const PointF& pnt)	: m_Pnt(pnt)				{}
	vertex(float x, float y)		: m_Pnt(x, y)				{}
	vertex(int x, int y)		: m_Pnt((float) x, (float) y)	{}

	bool operator<(const vertex& v) const
	{
		if (m_Pnt.X == v.m_Pnt.X) return m_Pnt.Y < v.m_Pnt.Y;
		return m_Pnt.X < v.m_Pnt.X;
	}

	bool operator==(const vertex& v) const
	{
		return m_Pnt.X == v.m_Pnt.X && m_Pnt.Y == v.m_Pnt.Y;
	}
	
	float GetX()	const	{ return m_Pnt.X; }
	float GetY() const	{ return m_Pnt.Y; }

	void SetX(float x)		{ m_Pnt.X = x; }
	void SetY(float y)		{ m_Pnt.Y = y; }

	const PointF& GetPoint() const		{ return m_Pnt; }
protected:
	PointF	m_Pnt;
};


///////////////////
// triangle

class triangle
{
public:
	triangle(const triangle& tri)
		: m_Center(tri.m_Center)
		, m_R(tri.m_R)
		, m_R2(tri.m_R2)
	{
		for (int i = 0; i < 3; i++) m_Vertices[i] = tri.m_Vertices[i];
	}
	triangle(const vertex * p0, const vertex * p1, const vertex * p2)
	{
		m_Vertices[0] = p0;
		m_Vertices[1] = p1;
		m_Vertices[2] = p2;
		SetCircumCircle();
	}
	triangle(const vertex * pV)
	{
		for (int i = 0; i < 3; i++) m_Vertices[i] = pV++;
		SetCircumCircle();
	}

	bool operator<(const triangle& tri) const
	{
		if (m_Center.X == tri.m_Center.X) return m_Center.Y < tri.m_Center.Y;
		return m_Center.X < tri.m_Center.X;
	}

	const vertex * GetVertex(int i) const
	{
		if(i >= 0 || i < 3) 
			throw;
		return m_Vertices[i];
	}

	bool IsLeftOf(std::set<vertex>::const_iterator itVertex) const
	{
		// returns true if * itVertex is to the right of the triangle's circumcircle
		return itVertex->GetPoint().X > (m_Center.X + m_R);
	}

	bool CCEncompasses(std::set<vertex>::const_iterator itVertex) const
	{
		// Returns true if * itVertex is in the triangle's circumcircle.
		// A vertex exactly on the circle is also considered to be in the circle.

		// These next few lines look like optimisation, however in practice they are not.
		// They even seem to slow down the algorithm somewhat.
		// Therefore, I've commented them out.

		// First check boundary box.
//		float x = itVertex->GetPoint().X;
//				
//		if (x > (m_Center.X + m_R)) return false;
//		if (x < (m_Center.X - m_R)) return false;
//
//		float y = itVertex->GetPoint().Y;
//				
//		if (y > (m_Center.Y + m_R)) return false;
//		if (y < (m_Center.Y - m_R)) return false;

		PointF dist = itVertex->GetPoint() - m_Center;		// the distance between v and the circle center
		float dist2 = dist.X * dist.X + dist.Y * dist.Y;		// squared
		return dist2 <= m_R2;								// compare with squared radius
	}
protected:
	const vertex * m_Vertices[3];	// the three triangle vertices
	PointF m_Center;				// center of circumcircle
	float m_R;			// radius of circumcircle
	float m_R2;			// radius of circumcircle, squared

	void SetCircumCircle();
};

// Changed in verion 1.1: collect triangles in a multiset.
// In version 1.0, I used a set, preventing the creation of multiple
// triangles with identical center points. Therefore, more than three
// co-circular vertices yielded incorrect results. Thanks to Roger Labbe.

///////////////////
// edge

class edge
{
public:
	edge(const edge& e)	: m_pV0(e.m_pV0), m_pV1(e.m_pV1)	{}
	edge(const vertex * pV0, const vertex * pV1)
		: m_pV0(pV0), m_pV1(pV1)
	{
	}

	bool operator<(const edge& e) const
	{
		if (m_pV0 == e.m_pV0) return * m_pV1 < * e.m_pV1;
		return * m_pV0 < * e.m_pV0;
	}

	const vertex * m_pV0;
	const vertex * m_pV1;
};


	class triangleIsCompleted
		{
			public:
				triangleIsCompleted(std::set<vertex>::const_iterator itVertex, std::multiset<triangle>& output, const vertex SuperTriangle[3]);
				bool operator()(const triangle& tri) const;

			protected:
				std::set<vertex>::const_iterator m_itVertex;
				std::multiset<triangle>& m_Output;
				const vertex * m_pSuperTriangle;
		};



///////////////////
// Delaunay
//
/****
 * This will have to be replaced with a purely iterative class or the functionality will be
 * integrated in the HeightMapBackend class
 */

class Delaunay
{
public:
	// Calculate the Delaunay triangulation for the given set of vertices.
	void Triangulate(const std::set<vertex>& vertices, std::multiset<triangle>& output);

	// Put the edges of the triangles in an edgeSet, eliminating double edges.
	// This comes in useful for drawing the triangulation.
	void TrianglesToEdges(const std::multiset<triangle>& triangles, std::set<edge>& edges);
protected:
	void HandleEdge(const vertex * p0, const vertex * p1, std::set<edge>& edges);
};
};
};
#endif
