#ifndef DELAUNAY_H
#define DELAUNAY_H

/*
**  delaunay.c : compute 2D delaunay triangulation in the plane.
**  Copyright (C) 2005  Wael El Oraiby <wael.eloraiby@gmail.com>
**
**
**  This program is free software: you can redistribute it and/or modify
**  it under the terms of the GNU Affero General Public License as
**  published by the Free Software Foundation, either version 3 of the
**  License, or (at your option) any later version.
**
**  This program is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU Affero General Public License for more details.
**
**  You should have received a copy of the GNU Affero General Public License
**  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#if defined(_MSC_VER) || defined(__MACOSX__) || defined(__APPLE__)

#if defined(DEL_EXPORTS)
#define DEL_DECLSPEC __declspec(dllexport)
#elif defined(DEL_IMPORTS)
#define DEL_DECLSPEC __declspec(dllimport)
#else
#define DEL_DECLSPEC
#endif

#elif __GNUC__ >= 4

#if defined(DEL_EXPORTS)
#define DEL_DECLSPEC __attribute__ ((visibility("default")))
#elif defined(DEL_IMPORTS)
#define DEL_DECLSPEC
#else
#define DEL_DECLSPEC
#endif

#else

#define DEL_DECLSPEC
#pragma warning Unknown dyanamic library

#endif


#define DEL_CIRCLE



#ifdef __cplusplus
extern "C" {
#endif

	typedef double realt;

typedef struct {
	realt	x, y;
} del_point2d_t;

typedef struct {
	/** input points count */
	unsigned int	num_points;

	/** the input points */
	del_point2d_t*	points;

	/** number of returned faces */
	unsigned int	num_faces;

	/** the faces are given as a sequence: num verts, verts indices, num verts, verts indices...
	 * the first face is the external face */
	unsigned int*	faces;
} delaunay2d_t;

/*
 * build the 2D Delaunay triangulation given a set of points of at least 3 points
 *
 * @points: point set given as a sequence of tuple x0, y0, x1, y1, ....
 * @num_points: number of given point
 * @preds: the incircle predicate
 * @faces: the triangles given as a sequence: num verts, verts indices, num verts, verts indices.
 *	Note that the first face is the external face
 * @return: the created topology
 */
DEL_DECLSPEC delaunay2d_t*			delaunay2d_from(del_point2d_t *points, unsigned int num_points);

/*
 * release a delaunay2d object
 */
DEL_DECLSPEC void				delaunay2d_release(delaunay2d_t* del);


typedef struct {
	/** input points count */
	unsigned int	num_points;

	/** input points */
	del_point2d_t*	points;

	/** number of triangles */
	unsigned int	num_triangles;

	/** the triangles indices v0,v1,v2, v0,v1,v2 .... */
	unsigned int*	tris;
} tri_delaunay2d_t;

/**
 * build a tri_delaunay2d_t out of a delaunay2d_t object
 */
DEL_DECLSPEC tri_delaunay2d_t*		tri_delaunay2d_from(delaunay2d_t* del);

/**
 * release a tri_delaunay2d_t object
 */
DEL_DECLSPEC void				tri_delaunay2d_release(tri_delaunay2d_t* tdel);

#ifdef __cplusplus
}
#endif

#endif // DELAUNAY_H
