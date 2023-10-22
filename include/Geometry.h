#ifndef GEOMETRY_H
#define GEOMETRY_H

#define _USE_MATH_DEFINES
#include <cmath>

namespace Geom {

using coord_t = double;
using rad_t = double;

struct vec2 {
	coord_t x, y;
};

/* Compare vec2s */
bool operator==(const vec2& l, const vec2& r);

/* Calculate L2 Norm of v */
double norm(vec2 v);

/* Calculate normalized vector */
vec2 normalized(vec2 v);

/* Calculate cosine similarity between u and v */
double cosine_similarity(vec2 u, vec2 v);

/* Calculate cosine similarity between u and v,
 * assuming that u is a unit vector.
 */
double cosine_normed(vec2 u, vec2 v);

/* Calculate dot product between u and v */
double dot(vec2 u, vec2 v);

/* Linearize (get absolute CCW angle in radians from positive x-axis) */
rad_t linearize(vec2 u);

/* Get distance between two points on a circle by angle in radians */
rad_t circular_distance(double a, double b);

/* Calculate cosine distance between two points */
double cosine_distance(vec2 u, vec2 v);

/* Calculate cosine distance between u and v,
 * assuming that u is a unit vector.
 */
double cosine_distance_normed(vec2 u, vec2 v);

/* Check if there is an intersection between an origin ray and and AABB */
bool intersection(vec2 ray, vec2 boxMin, vec2 boxMax);

}

#endif // GEOMETRY_H