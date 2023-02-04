#include "color.h"
#include "ray.h"
#include "vec3.h"

#include <iostream>

double hit_sphere(const point3& center, double radius, const ray& r) { //传入center ，radius，ray
    //下面是代入之前求解t的推导的一元二次方程
    //a = b*b b = 2b(A-C) c= (A-C)(A-C) - r*r
    vec3 oc = r.origin() - center; 
    auto a = dot(r.direction(), r.direction());
    auto b = 2.0 * dot(oc, r.direction());
    auto c = dot(oc, oc) - radius * radius;
    auto discriminant = b * b - 4 * a * c; //求解 大于0就是有交点
    if (discriminant < 0) {
        return -1.0; //没交点就是-1
    }
    else {
        return (-b - sqrt(discriminant)) / (2.0 * a); //有交点就返回交点的x坐标
    }
}

//这里的ray 是从原点到承影面的ray，z= focal_length 焦距=1 ， y范围是[-1,1], x范围根据aspect_ration 这里是 16:9, 这里是[-16/9.16/9]
//变成单位向量之后 最大的y值不在(-16/9,1,1) 或者(16/9,1,1)上，因为这里的y占比不是最大，而是应该在x占比最小的地方(0,1,1)和(0,-1,1)这两个点
//然后下面又对y的范围进行了一个归一化，从[-1,1]到[0,1](其实这里已经不能到-1和1了，因为变成标准向量了) 然后现在t值最低的点在原本的(0,-1,1)是近似白色，t值最大的点在原本的(0,1,1)是近似蓝色
color ray_color(const ray& r) {
    auto t = hit_sphere(point3(0, 0, -1), 0.5, r);
    if (t > 0.0) {
        vec3 N = unit_vector(r.at(t) - vec3(0, 0, -1)); //球心到球面点的向量(单位向量)
        return 0.5 * color(N.x() + 1, N.y() + 1, N.z() + 1); //(变成color并remap到0-1)
    }
    vec3 unit_direction = unit_vector(r.direction()); //返回direction的单位向量
    t = 0.5 * (unit_direction.y()+1.0); //把y的范围从(-1,1) remap到(0,1);
    return (1.0 - t) * color(1.0, 1.0, 1.0) + t * color(0.5, 0.7, 1.0); //插值 t=0 就白色 t=1 就蓝色；这里的t不是射线的t，是根据单位direction的y值remap出来的t；
}

int main() {

    // Image
    const auto aspect_ratio = 16.0 / 9.0;
    const int image_width = 400;
    const int image_height = static_cast<int>(image_width / aspect_ratio); //给定width就可以了，height由aspect_ratio来确定

    // Camera

    auto viewport_height = 2.0; 
    auto viewport_width = aspect_ratio * viewport_height; //viewport的宽高, viewport 相当于承影面
    auto focal_length = 1.0; //焦距

    auto origin = point3(0, 0, 0); //camera原点
    auto horizontal = vec3(viewport_width, 0, 0);
    auto vertical = vec3(0, viewport_height, 0);
    auto lower_left_corner = origin - horizontal / 2 - vertical / 2 - vec3(0, 0, focal_length); //左下角点

    // Render

    std::cout << "P3\n" << image_width << " " << image_height << "\n255\n";

    for (int j = image_height - 1; j >= 0; --j) {
        std::cerr << "\rScanlines remaining: " << j << ' ' << std::flush;
        for (int i = 0; i < image_width; ++i) {
            auto u = double(i) / (image_width - 1); //UV remap到0-1
            auto v = double(j) / (image_height - 1);
            ray r(origin, lower_left_corner + u * horizontal + v * vertical - origin); //牛逼，用low_left_corner 加上uv分别乘horizontal 和 vertical就能得到ray; ray 是打在承影面上的
            color pixel_color = ray_color(r); //得到color
            write_color(std::cout, pixel_color);
        }
    }

    std::cerr << "\nDone.\n";
}