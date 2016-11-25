#ifndef NPC__H
#define NPC__H
#include "drawable.h"
#include "explodingSprite.h"
#include "twowaysprite.h"
#include "healthBar.h"

class NPC : public TwoWaySprite {
public:
    NPC(const std::string&);
    NPC(const NPC&);
    ~NPC();
    void update(Uint32);
    void draw() const;
    bool collidedWith(Drawable*);
    bool hurt(int);
    bool doneExploding();

private:
    NPC& operator=(const NPC&);
    void explode();
    int health;
    ExplodingSprite* explosion;
    HealthBar healthBar;
    unsigned int healthEndTime;
};

#endif
