#ifndef HITBOX_H
#define HITBOX_H

#include <stdint.h>
#include <math.h>
//
#define OVERLAP_NONE 0
#define OVERLAP_POSITIVE_X 0b00000001
#define OVERLAP_NEGATIVE_X 0b00000010
#define OVERLAP_POSITIVE_Y 0b00000100
#define OVERLAP_NEGATIVE_Y 0b00001000
#define OVERLAP_PX_PY 0b00010000
#define OVERLAP_PX_NY 0b00100000
#define OVERLAP_NX_PY 0b01000000
#define OVERLAP_NX_NY 0b10000000

typedef struct _circle Circle;
typedef struct _area Area;
typedef struct _world World;
struct _circle {
	double x, y;
	double r;
	uint32_t area_index;
	uint32_t id;
};
struct _area {
	double x, y, w, h;
};
struct _world {
	// Total width, height of the world, narea is the number of areas, narea_w is the number of areas in one row
	uint32_t w, h, narea, narea_width; 
	Area* areas;
};

inline uint32_t get_area_id(World* w, Circle* c) {
	for(int i = 0; i < w->narea; i++) {
		if(w->areas[i].x < c->x && w->areas[i].x + w->areas[i].h > c->x && w->areas[i].y < c->y && w->areas[i].y + w->areas[i].h > c->y){
			c->area_index = i;
			return i;
		}
	}
}

inline uint8_t collide(Circle* c1, Circle* c2) {
	double dx = c1->x - c2->x;
	double dy = c1->y - c2->y;
	return sqrt(dx*dx + dy*dy);
}
/**
* Retunrs a number representing the neighbouring areas, that the circle is overlapping with.
*
* The result is built with the directives OVERLAP_*.
*/
uint8_t touches_neighbours(World* w, Circle* c){
	uint8_t result = 0;
	Area a = w->areas[c->area_index];
	// "Left"
	// circle - radius is smaller than the area
	if(a.x <= (c->x - c->r)){
		result |= OVERLAP_NEGATIVE_X;
	}
	// "Right"
	// circle + radius is bigger than area + width
	if((a.x + a.w) >= (c->x + c->r)) {
		result |= OVERLAP_POSITIVE_X;
	}
	// "Down"
	//
	if((a.h + a.y) >= (c->y + c->r)) {
		result |= OVERLAP_POSITIVE_Y;
	}
	// "Up"
	//
	if(a.y <= (c->y - c->r)) {
		result |= OVERLAP_NEGATIVE_Y;
	}
	// For diagonal overlaps, calculate the distance between the corners of the area and the circle
	// "Up-left"
	// Only if up and left are true
	if(result & (OVERLAP_NEGATIVE_X + OVERLAP_NEGATIVE_Y) == (OVERLAP_NEGATIVE_X + OVERLAP_NEGATIVE_Y)){
		double x = a.x - c->x;
		double y = a.y - c->y;
		if(sqrt(x*x + y*y) <= c->r) {
			result |= OVERLAP_NX_NY;
		}
	}
	// "Up-right"
	// Only if up and right are true
	if(result & (OVERLAP_POSITIVE_X + OVERLAP_NEGATIVE_Y) == (OVERLAP_POSITIVE_X + OVERLAP_NEGATIVE_Y)){
		double x = a.x + a.w - c->x;
		double y = a.y - c->y;
		if(sqrt(x*x + y*y) <= c->r) {
			result |= OVERLAP_PX_NY;
		}
	}
	// "Down-right"
	// Only if down and right are true
	if(result & (OVERLAP_POSITIVE_X + OVERLAP_POSITIVE_Y) == (OVERLAP_POSITIVE_X + OVERLAP_POSITIVE_Y)){
		double x = a.x + a.w - c->x;
		double y = a.y + a.h - c->y;
		if(sqrt(x*x + y*y) <= c->r) {
			result |= OVERLAP_PX_PY;
		}
	}
	// "Down-left"
	// Only if down and left are true
	if(result & (OVERLAP_NEGATIVE_X + OVERLAP_POSITIVE_Y) == (OVERLAP_NEGATIVE_X + OVERLAP_POSITIVE_Y)){
		double x = a.x - c->x;
		double y = a.y + a.h - c->y;
		if(sqrt(x*x + y*y) <= c->r) {
			result |= OVERLAP_NX_PY;
		}
	}
}


#endif /* HITBOX_H */
