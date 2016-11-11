#ifndef GUN__H 
#define GUN__H
#include <SDL.h>
#include <vector>

class Gun {
public:
    Gun();
    void shoot(float, float);

private:
    Gun(const Gun&);
    Gun operator=(const Gun&);
    std::vector<SDL_Surface*> outBullets;
    std::vector<SDL_Surface*> freeBullets;

    SDL_Surface* gun;
    SDL_Surface* bullet;
    
    int bulletSpeed;
    int bulletDistance;

};

#endif
