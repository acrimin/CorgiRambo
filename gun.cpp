#include "gamedata.h"
#include "frameFactory.h"
#include "gun.h"
#include "clock.h"

Gun::Gun(const std::string& name) :
    name(name),
    bulletVelocity(Gamedata::getInstance().getXmlInt(name+"/speed"),0),
    displacement(Gamedata::getInstance().getXmlInt(name+"/displacement/x"),
                 Gamedata::getInstance().getXmlInt(name+"/displacement/y")), 
    bulletDistance(Gamedata::getInstance().getXmlInt(name+"/distance")),
    fireRate(Gamedata::getInstance().getXmlInt(name+"/fireRate")), 
    canShoot(true),
    outBullets(),
    freeBullets()
{ 
}

void Gun::draw() const {
    for (unsigned int i = 0; i < outBullets.size(); ++i) {
        outBullets[i]->draw();
    }
}

void Gun::update(Uint32 ticks) {
    std::vector<Bullet*>::iterator iter = outBullets.begin();
    while (iter != outBullets.end()) {
        (*iter)->update(ticks);
        if ((*iter)->isDone()) {
            freeBullets.push_back(*iter);
            iter = outBullets.erase(iter);
        } else {
            ++iter;
        }
    }

    std::cout << outBullets.size() << " " << freeBullets.size() << std::endl;
}

void Gun::shoot(const Vector2f& pos, const int sign) {
    static unsigned int prevTicks = 0;
    unsigned int ticks = (Clock::getInstance()).getTicks() - prevTicks; 

    if (ticks < fireRate) {
        return;
    }
    
    prevTicks = (Clock::getInstance()).getTicks();

    Bullet* bullet;

    Vector2f startPos(pos[0] + (displacement[0] * sign),
                      pos[1] + displacement[1]);

    if (freeBullets.size() > 0) {
        bullet = freeBullets[freeBullets.size() -1];
        freeBullets.pop_back();
        bullet->reset(startPos, bulletVelocity * sign); 
    } else {
        bullet = new Bullet(name + "Bullet",
                            startPos,
                            bulletVelocity * sign,
                            bulletDistance);
    }

    outBullets.push_back(bullet);
}
