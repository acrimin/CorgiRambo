#ifndef GUN__H 
#define GUN__H
#include <SDL.h>
#include <vector>
#include <string>
#include "bullet.h"

class Gun : public Drawable {
public:
    Gun(const std::string&);
    void shoot(const Vector2f& pos, const int sign);
    const std::vector<Bullet*> getBullets() const { return outBullets; }

    virtual ~Gun() { }

    virtual const Frame* getFrame() const {
        return frame;
    }
    virtual void draw() const;

    virtual void update(Uint32 ticks);



private:
    Gun(const Gun&);
    Gun& operator=(const Gun&);
    
    const Frame* frame;
    
    Vector2f bulletVelocity;
    Vector2f displacement;
    int bulletDistance;
    
    bool canShoot;

    std::vector<Bullet*> outBullets;
    std::vector<Bullet*> freeBullets;
};

#endif
