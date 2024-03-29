#include "gamedata.h"
#include "frameFactory.h"
#include "gun.h"
#include "ioManager.h"
#include "clock.h"
#include "sound.h"

Gun::Gun(const std::string& name, const Vector2f& disp) :
    name(name),
    prevTicks(0),
    bulletVelocity(Gamedata::getInstance().getXmlInt(name+"/speed"),0),
    displacement(disp),
    bulletDistance(Gamedata::getInstance().getXmlInt(name+"/distance")),
    fireRate(Gamedata::getInstance().getXmlInt(name+"/fireRate")), 
    canShoot(true),
    outBullets(),
    freeBullets(),
    damage(Gamedata::getInstance().getXmlInt(name+"/damage")),
    sound(Gamedata::getInstance().getXmlInt(name+"/sound"))
{ }

Gun::~Gun() {
    for (unsigned int i = 0; i < outBullets.size(); ++i) {
        delete outBullets[i];
    }
    for (unsigned int i = 0; i < freeBullets.size(); ++i) {
        delete freeBullets[i];
    }
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
    char numstr[21];
    sprintf(numstr, "%lu", outBullets.size());
    std::string title = "Bullet List: ";
    IOManager::getInstance().addHudMessage(title + numstr);
    sprintf(numstr, "%lu", freeBullets.size());
    title = "Free List: ";
    IOManager::getInstance().addHudMessage(title + numstr);
//    std::cout << outBullets.size() << " " << freeBullets.size() << std::endl;
}

void Gun::shoot(const Vector2f& pos, const int sign) {
    unsigned ticks = (Clock::getInstance()).getTicks() - prevTicks; 

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

    SDLSound::getInstance()[sound];
}

bool Gun::collidedWith(Drawable* d) {
    for (unsigned int i = 0; i < outBullets.size(); ++i) {
        if (outBullets[i]->collidedWith(d)) {
            outBullets[i]->explode();
            if (d->hurt(damage)) //kill
                return true;
        }
    }
    return false;
}
