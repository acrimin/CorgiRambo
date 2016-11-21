#ifndef GUN__H 
#define GUN__H
#include <SDL.h>
#include <vector>
#include <string>
#include "bullet.h"

class Gun {
public:
    Gun(const std::string&);
    void shoot(const Vector2f&, const int);
    const std::vector<Bullet*> getBullets() const { return outBullets; }

    const std::string& getName() const {
        return name;
    }
    
    ~Gun() { }
    void draw() const;
    void update(Uint32 ticks);



private:
    Gun(const Gun&);
    Gun& operator=(const Gun&);

    std::string name;
    
    Vector2f bulletVelocity;
    Vector2f displacement;
    int bulletDistance;
    unsigned int fireRate;
    
    bool canShoot;

    std::vector<Bullet*> outBullets;
    std::vector<Bullet*> freeBullets;
};

#endif
