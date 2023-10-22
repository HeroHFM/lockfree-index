#include "Geometry.h"

namespace Geom {

/* Compare vec2s */
bool operator==(const vec2& l, const vec2& r)
{
    return (l.x == r.x) && (l.y == r.y);
}

/* Calculate L2 Norm of v */
double norm(vec2 v) {
	return sqrt(pow(v.x, 2) + pow(v.y, 2));
}

/* Calculate normalized vector */
vec2 normalized(vec2 v) {
	double n = norm(v);
	v.x /= n;
	v.y /= n;
	return v;
}

/* Calculate cosine similarity between u and v */
double cosine_similarity(vec2 u, vec2 v) {
	return (u.x * v.x + u.y * v.y) / (norm(u) * norm(v));
}

/* Calculate cosine similarity between u and v,
 * assuming that u is a unit vector.
 */
double cosine_normed(vec2 u, vec2 v) {
	return (u.x * v.x + u.y * v.y) / norm(v);
}

/* Calculate dot product between u and v */
double dot(vec2 u, vec2 v) {
	return (u.x * v.x + u.y * v.y);
}

/* Linearize (get absolute CCW angle in radians from positive x-axis) */
rad_t linearize(vec2 u) {
	return (u.y > 0) ? atan2(u.y, u.x) : 2 * M_PI + atan2(u.y, u.x);
}

/* Get distance between two points on a circle by angle in radians */
rad_t circular_distance(double a, double b) {
	return std::min(std::abs(a - b), 2 * M_PI - std::abs(a - b));
}

/* Calculate cosine distance between two points */
double cosine_distance(vec2 u, vec2 v) {
	return 1 - cosine_similarity(u, v);
}

/* Calculate cosine distance between u and v,
 * assuming that u is a unit vector.
 */
double cosine_distance_normed(vec2 u, vec2 v) {
	return 1 - cosine_normed(u, v);
}

/* Check if there is an intersection between an origin ray and and AABB */
bool intersection(vec2 ray, vec2 boxMin, vec2 boxMax) {
    double tx1 = boxMin.x / ray.x;
    double ty1 = boxMin.y / ray.y;
    double tx2 = boxMax.x / ray.x;
    double ty2 = boxMax.y / ray.y;

    double tmin = std::max(std::min(tx1, tx2), std::min(ty1, ty2));
    double tmax = std::min(std::max(tx1, tx2), std::max(ty1, ty2));

    return (tmax >= tmin) and (tmax >= 0);
}

}