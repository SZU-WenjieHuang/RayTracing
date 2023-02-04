#include "color.h"
#include "ray.h"
#include "vec3.h"

#include <iostream>

double hit_sphere(const point3& center, double radius, const ray& r) { //����center ��radius��ray
    //�����Ǵ���֮ǰ���t���Ƶ���һԪ���η���
    //a = b*b b = 2b(A-C) c= (A-C)(A-C) - r*r
    vec3 oc = r.origin() - center; 
    auto a = dot(r.direction(), r.direction());
    auto b = 2.0 * dot(oc, r.direction());
    auto c = dot(oc, oc) - radius * radius;
    auto discriminant = b * b - 4 * a * c; //��� ����0�����н���
    if (discriminant < 0) {
        return -1.0; //û�������-1
    }
    else {
        return (-b - sqrt(discriminant)) / (2.0 * a); //�н���ͷ��ؽ����x����
    }
}

//�����ray �Ǵ�ԭ�㵽��Ӱ���ray��z= focal_length ����=1 �� y��Χ��[-1,1], x��Χ����aspect_ration ������ 16:9, ������[-16/9.16/9]
//��ɵ�λ����֮�� ����yֵ����(-16/9,1,1) ����(16/9,1,1)�ϣ���Ϊ�����yռ�Ȳ�����󣬶���Ӧ����xռ����С�ĵط�(0,1,1)��(0,-1,1)��������
//Ȼ�������ֶ�y�ķ�Χ������һ����һ������[-1,1]��[0,1](��ʵ�����Ѿ����ܵ�-1��1�ˣ���Ϊ��ɱ�׼������) Ȼ������tֵ��͵ĵ���ԭ����(0,-1,1)�ǽ��ư�ɫ��tֵ���ĵ���ԭ����(0,1,1)�ǽ�����ɫ
color ray_color(const ray& r) {
    auto t = hit_sphere(point3(0, 0, -1), 0.5, r);
    if (t > 0.0) {
        vec3 N = unit_vector(r.at(t) - vec3(0, 0, -1)); //���ĵ�����������(��λ����)
        return 0.5 * color(N.x() + 1, N.y() + 1, N.z() + 1); //(���color��remap��0-1)
    }
    vec3 unit_direction = unit_vector(r.direction()); //����direction�ĵ�λ����
    t = 0.5 * (unit_direction.y()+1.0); //��y�ķ�Χ��(-1,1) remap��(0,1);
    return (1.0 - t) * color(1.0, 1.0, 1.0) + t * color(0.5, 0.7, 1.0); //��ֵ t=0 �Ͱ�ɫ t=1 ����ɫ�������t�������ߵ�t���Ǹ��ݵ�λdirection��yֵremap������t��
}

int main() {

    // Image
    const auto aspect_ratio = 16.0 / 9.0;
    const int image_width = 400;
    const int image_height = static_cast<int>(image_width / aspect_ratio); //����width�Ϳ����ˣ�height��aspect_ratio��ȷ��

    // Camera

    auto viewport_height = 2.0; 
    auto viewport_width = aspect_ratio * viewport_height; //viewport�Ŀ��, viewport �൱�ڳ�Ӱ��
    auto focal_length = 1.0; //����

    auto origin = point3(0, 0, 0); //cameraԭ��
    auto horizontal = vec3(viewport_width, 0, 0);
    auto vertical = vec3(0, viewport_height, 0);
    auto lower_left_corner = origin - horizontal / 2 - vertical / 2 - vec3(0, 0, focal_length); //���½ǵ�

    // Render

    std::cout << "P3\n" << image_width << " " << image_height << "\n255\n";

    for (int j = image_height - 1; j >= 0; --j) {
        std::cerr << "\rScanlines remaining: " << j << ' ' << std::flush;
        for (int i = 0; i < image_width; ++i) {
            auto u = double(i) / (image_width - 1); //UV remap��0-1
            auto v = double(j) / (image_height - 1);
            ray r(origin, lower_left_corner + u * horizontal + v * vertical - origin); //ţ�ƣ���low_left_corner ����uv�ֱ��horizontal �� vertical���ܵõ�ray; ray �Ǵ��ڳ�Ӱ���ϵ�
            color pixel_color = ray_color(r); //�õ�color
            write_color(std::cout, pixel_color);
        }
    }

    std::cerr << "\nDone.\n";
}