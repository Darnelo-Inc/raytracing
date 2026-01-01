#include <stdio.h>
#include <SDL.h>
#include <stdbool.h>

#define WIN_WIDTH  1300
#define WIN_HEIGHT 900
#define WHITE 0xffffff
#define BLACK 0x000000
#define GREY 0xefefefef
#define RAYS_NUMBER 100
#define YELLOW 0xFFEC30
#define GREEN 0x2ECF19

struct Circle {
    double x;
    double y;
    double r;
};

struct Ray {
    double x0, y0;
    double angle;
    double xn, yn;
};

bool isInsideCircle(struct Circle circle, double x, double y) {
    double r_squared = pow(circle.r, 2);
    double d_squared = pow(x - circle.x, 2) + pow(y - circle.y, 2);
    return d_squared <= r_squared;
}

void FillCircle(SDL_Surface *surface, struct Circle circle, Uint32 color) {
    for(double x = circle.x - circle.r; x <= circle.x + circle.r; x++) {
        for(double y = circle.y - circle.r; y <= circle.y + circle.r; y++) {
            if(isInsideCircle(circle, x, y)) {
                SDL_Rect px = (SDL_Rect){x, y, 1, 1};
                SDL_FillRect(surface, &px, color);
            };
        }
    }
}

void GenerateRays(struct Circle circle, struct Ray rays[RAYS_NUMBER]) {
    for(int i = 0; i < RAYS_NUMBER; i++) {
        double angle = 2 * M_PI * ((double) i / RAYS_NUMBER);
        struct Ray ray = {circle.x, circle.y, angle};
        rays[i] = ray;
    }
}

void FillRays(SDL_Surface *surface, struct Circle shadow_circle, struct Ray rays[RAYS_NUMBER], Uint32 color) {
   for(int i = 0; i < RAYS_NUMBER; i++) {
        struct Ray ray = rays[i];
    
        bool end_of_screen = false;
        bool object_hit = false;

        double step = 1;
        double render_x = ray.x0;
        double render_y = ray.y0;
        
        while(!end_of_screen && !object_hit) {
            render_x += step * cos(ray.angle);
            render_y += step * sin(ray.angle);

            SDL_Rect px = (SDL_Rect){render_x, render_y, 1, 1};
            SDL_FillRect(surface, &px, color);
            
            if(render_x < 0 || render_x > WIN_WIDTH || render_y < 0 || render_y > WIN_HEIGHT) {
                end_of_screen = true;
            }

            if(isInsideCircle(shadow_circle, render_x, render_y)) {
                object_hit = true;
            }
        }
   } 
}

double ShadowCircleBouncing(struct Circle *circle, double speed) {
    double r = circle->r;
    circle->y += speed;

    if(circle->y - r < 0 || circle->y + r > WIN_HEIGHT) {
        speed = -speed;
    }

    return speed;
}

int main() {
    SDL_Window *window = SDL_CreateWindow(
        "Ray tracing",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WIN_WIDTH,
        WIN_HEIGHT,
        SDL_WINDOW_SHOWN
    );
    SDL_Surface *surface = SDL_GetWindowSurface(window);
    SDL_Rect bg_rect = (SDL_Rect) {0, 0, WIN_WIDTH, WIN_HEIGHT}; 
    struct Circle circle = {160, 160, 40};
    struct Circle shadow_circle = {650, 300, 140};
    struct Ray rays[RAYS_NUMBER];
    double shadow_bouncing_speed = 3;

    GenerateRays(circle, rays);
    
    bool running = true;
    SDL_Event event;
    
    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
            
            if(event.type == SDL_MOUSEMOTION && event.motion.state != 0) {
                circle.x = event.motion.x;
                circle.y = event.motion.y;
                GenerateRays(circle, rays);
            }
        }
        SDL_FillRect(surface, &bg_rect, BLACK);
        FillRays(surface, shadow_circle, rays, YELLOW);
        FillCircle(surface, shadow_circle, GREEN);
        FillCircle(surface, circle, YELLOW);
     
        shadow_bouncing_speed = ShadowCircleBouncing(&shadow_circle, shadow_bouncing_speed);

        SDL_UpdateWindowSurface(window);
        SDL_Delay(10);
    }

    SDL_DestroyWindow(window);
    SDL_Quit();
    
    return 0;
}
