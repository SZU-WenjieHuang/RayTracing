#ifndef MATERIAL_H
#define MATERIAL_H

#include "rtweekend.h"
#include "hittable.h"

struct hit_record;

/*
    父类定义了纯虚函数"scatter"
    r_in 是入射光线
    rec 是撞击点信息
    attenuation 是散射之后的衰减系数
    scattered 是ray格式的，表示散射之后的光线
*/
class material {
public:
    virtual bool scatter(
        const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered
    ) const = 0;
};

//下面的子类都是对父类的实现

//兰伯特漫反射类
class lambertian : public material {
public:
    lambertian(const color& a) : albedo(a) {}

    virtual bool scatter(
        const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered
    ) const override {
        auto scatter_direction = rec.normal + random_unit_vector();

        // Catch degenerate scatter direction
        if (scatter_direction.near_zero())
            scatter_direction = rec.normal; //要是接近0向量，那就直接用法向量代替

        scattered = ray(rec.p, scatter_direction);
        attenuation = albedo; //表示反射率
        return true;
    }

public:
    color albedo;
};

//金属
class metal : public material {
public:
    metal(const color& a, double f) : albedo(a), fuzz(f < 1 ? f : 1) {}

    virtual bool scatter(
        const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered
    ) const override {
        vec3 reflected = reflect(unit_vector(r_in.direction()), rec.normal);//反射方向
        scattered = ray(rec.p, reflected + fuzz * random_in_unit_sphere());//磨砂其实就是给了一个小范围的漫反射
        attenuation = albedo;
        return (dot(scattered.direction(), rec.normal) > 0);
    }

public:
    color albedo;
    double fuzz;
};

//电介质
class dielectric : public material {
public:
    dielectric(double index_of_refraction) : ir(index_of_refraction) {}

    virtual bool scatter(
        const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered
    ) const override {
        attenuation = color(1.0, 1.0, 1.0); //没有衰减的
        double refraction_ratio = rec.front_face ? (1.0 / ir) : ir;

        vec3 unit_direction = unit_vector(r_in.direction());
        vec3 refracted = refract(unit_direction, rec.normal, refraction_ratio);

        scattered = ray(rec.p, refracted);
        return true;
    }

public:
    double ir; // Index of Refraction
};

#endif
