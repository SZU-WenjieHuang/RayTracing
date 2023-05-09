#ifndef RAY_H
#define RAY_H

#include "vec3.h"

class ray {
public:
    //构造函数 point 和 vec 传给 orig 和 dir
    ray() {}
    ray(const point3& origin, const vec3& direction)
        : orig(origin), dir(direction)
    {}

    point3 origin() const { return orig; }
    vec3 direction() const { return dir; }

    //计算传播了t的点
    point3 at(double t) const {
        return orig + t * dir;
    }

public:
    point3 orig;
    vec3 dir;
};

#endif