#ifndef LENSOR_OS_MATH_H
#define LENSOR_OS_MATH_H

struct Vector2 {
	unsigned int x;
	unsigned int y;

	Vector2() {
		x = 0;
		y = 0;
	}

	Vector2(unsigned int _x, unsigned int _y) {
		x = _x;
		y = _y;
	}
};

#endif
