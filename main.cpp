#include <iostream>
#include <fstream>
#include <cstdlib>
#include "sphere.cpp"
#include "float.h"
#include "hitable_list.cpp"
#include "camera.cpp"
#include "material.cpp"
#define MAXFLOAT 3.40282347e+38F

vec3 color(const ray& r, hitable *world, int depth){
    hit_record rec;
    if (world->hit(r, 0.001, MAXFLOAT, rec)){
        ray scattered;
        vec3 attenuation;
        if(depth < 50 && rec.mat_ptr->scatter(r, rec, attenuation, scattered)){
            return attenuation*color(scattered, world, depth+1);
        }else{
            return vec3(0,0,0);
        }
    }else{
        vec3 unit_direction = unit_vector(r.direction());
        float t = 0.5*(unit_direction.y() + 1.0);
        return (1.0-t)*vec3(1.0, 1.0, 1.0) + t*vec3(0.5, 0.7, 1.0);
    }
}

hitable *random_scene(){
    int n = 500; hitable **list = new hitable*[n+1];
    list[0] = new sphere(vec3(0,-1000, 0), 1000, new lambertian(vec3(0.5, 0.5, 0.5)));
    int i = 1;
    for(int a = -11; a < 11; a++){
        for(int b = -11; b < 11; b++){
            float choose_mat = std::rand() / (RAND_MAX + 1.0);
            vec3 center(a+0.9*std::rand() / (RAND_MAX + 1.0), 0.2, b+0.9*std::rand() / (RAND_MAX + 1.0));
            if ((center - vec3(4, 0.2, 0)).length() > 0.9) {
                if(choose_mat < 0.8){
                    list[i++] = new sphere(center, 0.2, new lambertian(vec3(std::rand() / (RAND_MAX + 1.0) * std::rand() / (RAND_MAX + 1.0),
                    std::rand() / (RAND_MAX + 1.0)*std::rand() / (RAND_MAX + 1.0),
                    std::rand() / (RAND_MAX + 1.0)*std::rand() / (RAND_MAX + 1.0))));
                }else if(choose_mat < 0.95){
                    list[i++] = new sphere(center, 0.2, new metal(vec3(0.5*(1 + std::rand() / (RAND_MAX + 1.0)),
                    0.5*(1 + std::rand() / (RAND_MAX + 1.0)), 
                    0.5*(1+std::rand() / (RAND_MAX + 1.0))), 
                    0.5*std::rand() / (RAND_MAX + 1.0)));
                }else{
                    list[i++] = new sphere(center, 0.2, new dielectric(1.5));
                }
            }
        }
    }

    list[i++] = new sphere(vec3(0,1,0), 1.0, new dielectric(1.5));
    list[i++] = new sphere(vec3(-4,1,0), 1.0, new lambertian(vec3(0.4, 0.2, 0.1)));
    list[i++] = new sphere(vec3(4,1,0), 1.0, new metal(vec3(0.7, 0.6, 0.5), 0.0));

    return new hitable_list(list, i);
}

int main() {
    int nx = 700;
    int ny = 350;
    int ns = 150;

    // Cria o arquivo PPM e escreve o cabeçalho
    std::ofstream ppm("chapter-12.ppm");
    ppm << "P3\n" << nx << " " << ny << "\n255\n";
 
    hitable *world = random_scene();
    vec3 lookfrom(13,2,3);
    vec3 lookat(0,0,0);
    float dist_to_focus = (lookfrom-lookat).length();
    float aperture = 0.1;
    camera cam(lookfrom, lookat, vec3(0,1,0), 25, float(nx)/float(ny), aperture, 10.0);
    // Preenche a matriz de pixels com as cores geradas pelo código e escreve cada pixel no arquivo PPM
    for (int j = ny - 1; j >= 0; j--) {
        for (int i = 0; i < nx; i++) {
            vec3 col(0, 0, 0);
            for(int s=0; s < ns; s++){
                float u = (i + std::rand() / (RAND_MAX + 1.0)) / float(nx);
                float v = (j + std::rand() / (RAND_MAX + 1.0)) / float(ny);
                ray r = cam.get_ray(u, v);
                col += color(r, world, 40);
            }
            col /= float(ns);
            col = vec3(sqrt(col[0]), sqrt(col[1]), sqrt(col[2]));
            int ir = int(255.99*col[0]);
            int ig = int(255.99*col[1]);
            int ib = int(255.99*col[2]);

            ppm << ir << " " << ig << " " << ib << "\n";
        }
    }

    // Fecha o arquivo PPM
    ppm.close();

    return 0;
}