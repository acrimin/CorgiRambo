#include "npc.h"

NPC::NPC(const std::string& name) :
    TwoWaySprite(name),
    health(Gamedata::getInstance().getXmlInt(name+"/health")),
    explosion(NULL)
{}

NPC::NPC(const NPC& n) :
    TwoWaySprite(n),
    health(n.health),
    explosion(n.explosion)
{}

NPC::~NPC() {
    if (explosion)
        delete explosion;
}

void NPC::draw() const {
    if (explosion) {
        explosion->draw();
        return;
    }
    TwoWaySprite::draw();
}

void NPC::update(Uint32 ticks) {
    if (explosion) {
        explosion->update(ticks);
        return;
    }
    TwoWaySprite::update(ticks);
}

void NPC::explode() {
    int face;
    if (velocityX() > 0) 
        face = 0;        
    else
        face = 1;

    Sprite sprite = Sprite(getName(), 
                           getPosition(), 
                           Vector2f(0,0), 
                           frames[currentFrame + face]);
    explosion = new ExplodingSprite(sprite);
}

bool NPC::doneExploding() {
    if (!explosion)
        return false;
    return (explosion->chunkCount() == 0);
}

bool NPC::hurt(int damage) {
    if (explosion)
        return false;
    health -= damage;
    std::cout << health << std::endl;
    if (health <= 0) {
        explode();
        return true;
    }
    return false;
}
/*
bool NPC::collidedWith(Drawable* d) {
    if (explosion)
        return false;
    return false;
}
*/
