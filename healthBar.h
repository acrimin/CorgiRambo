#ifndef HEALTHBAR__H
#define HEALTHBAR__H

#include "vector2f.h"
#include "ioManager.h"
#include "aaline.h"

class HealthBar {
public:
    HealthBar(int);
    HealthBar(const HealthBar&);
    void draw(Vector2f, int) const;
private:
    int totalHealth;
    SDL_Surface* screen;
};

#endif
