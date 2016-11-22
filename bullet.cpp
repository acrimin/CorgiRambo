#include <cmath>
#include "bullet.h"
#include "gamedata.h"
#include "frameFactory.h"


Bullet::Bullet(const string& n, const Vector2f& pos, const Vector2f& vel, int dis):
    Drawable(n, pos, vel),
    frames( FrameFactory::getInstance().getFrames(n) ),
    frameWidth(frames[0]->getWidth()),
    frameHeight(frames[0]->getHeight()),

    maxDistance(dis),
    distance(0),
    done(false),
    strategy(new Collision)
{ }

Bullet::Bullet(const Bullet& s) :
    Drawable(s),
    frames(s.frames),
    frameWidth(s.getFrame()->getWidth()),
    frameHeight(s.getFrame()->getHeight()),

    maxDistance(s.maxDistance),
    distance(s.distance),
    done(s.done),
    strategy(s.strategy)
{ }

void Bullet::draw() const {
    Uint32 x = static_cast<Uint32>(X());
    Uint32 y = static_cast<Uint32>(Y());
    if (velocityX() > 0) {
        frames[0]->draw(x, y);
    } else {
        frames[1]->draw(x, y);
    }
}

void Bullet::update(Uint32 ticks) {
    Vector2f incr = getVelocity() * static_cast<float>(ticks) * 0.001;
    distance += abs(incr[0]);
    if (distance > maxDistance) {
        done = true;
    }
    setPosition(getPosition() + incr);
}

bool Bullet::collidedWith(Drawable* d) const {
    return Collision::getInstance().executePerPixel(*this, *d);
}

void Bullet::reset(const Vector2f& pos, const Vector2f& vel) {
    setPosition(pos);
    setVelocity(vel);
    distance = 0;
    done = false;
}

void Bullet::explode() {
    done = true;
}
