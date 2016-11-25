#include "healthBar.h"
#include "viewport.h"

HealthBar::HealthBar(int maxHealth) :
    totalHealth(maxHealth),
    screen(IOManager::getInstance().getScreen())
{ }

HealthBar::HealthBar(const HealthBar& h) :
    totalHealth(h.totalHealth),
    screen(IOManager::getInstance().getScreen())
{ }

void HealthBar::draw(Vector2f pos, int health) const {
    short unsigned length = 200;
    short unsigned height = 20;
    short unsigned thick = 1;

    pos -= Viewport::getInstance().getPosition();
    pos += Vector2f(-length/2,0);

    Uint32 BLACK = SDL_MapRGB(screen->format, 0, 0, 0);
    Uint32 GREEN = SDL_MapRGB(screen->format, 0, 255, 0);
    Uint32 YELLOW = SDL_MapRGB(screen->format, 255, 255, 0);
    Uint32 RED = SDL_MapRGB(screen->format, 255, 0, 0);

    short unsigned startX = pos[0];
    short unsigned startY = pos[1];

    Draw_AALine(screen, startX, startY, startX+length, startY, thick, BLACK);
    Draw_AALine(screen, startX, startY+height, startX+length, startY+height, thick, BLACK);
    Draw_AALine(screen, startX, startY, startX, startY+height, thick, BLACK);
    Draw_AALine(screen, startX+length, startY, startX+length, startY+height, thick, BLACK);
    
    Uint32 healthColor;
    float ratio = static_cast<float>(health)/totalHealth;
    if (ratio > static_cast<float>(2)/3) {
        healthColor = GREEN;
    }
    else if (ratio > static_cast<float>(1)/3) {
        healthColor = YELLOW;
    }
    else {
        healthColor = RED;
    }

    int healthLength = (length-1) * health/totalHealth;
    Draw_AALine(screen, startX+thick, startY+height/2, startX+healthLength, startY+height/2, height-2*thick, healthColor);
}
