#ifndef BULLET__H
#define BULLET__H
#include <string>
#include <vector>
#include "drawable.h"
#include "collisionStrategy.h"

class Bullet : public Drawable {
public:
    Bullet(const std::string&, const Vector2f&, const Vector2f&, int);
    Bullet(const Bullet& s);
    virtual ~Bullet() { }

    virtual const Frame* getFrame() const {
        return velocityX() > 0 ? frames[0] : frames[1];
    }
    
    bool isDone() const {
        return done;
    }

    virtual void draw() const;
    void reset(const Vector2f&, const Vector2f&);
    void explode();
    bool collidedWith(Drawable*) const;

    virtual void update(Uint32 ticks);

private:
    Bullet& operator=(const Bullet& rhs);

    std::vector<Frame*> frames;
    bool exploding;
    bool explodes;
    unsigned explodeTime;
    int frameWidth;
    int frameHeight;

    int maxDistance;
    int distance;
    bool done;
    Collision* strategy;
};
#endif
