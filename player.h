#ifndef PLAYER__H
#define PLAYER__H
#include <string>
#include <vector>
#include "drawable.h"
#include "collisionStrategy.h"
#include "explodingSprite.h"
#include "healthBar.h"
#include "gun.h"

class Player : public Drawable {
public:
    Player(const std::string&);
    Player(const Player&);
    virtual ~Player(); 

    virtual void draw() const;
    virtual void update(Uint32 ticks);
    virtual const Frame* getFrame() const {
        return frames[currentFrame];
    }
    bool collidedWith(Drawable*);
    bool hurt(int);
    void right(bool);
    void left(bool);
    void shift(bool);
    void shoot(bool);
    void changeGun();
    void up();
    void toggleGodMode();

protected:
    Player operator=(const Player&);
    void explode();
    std::vector<Frame *> frames;
    ExplodingSprite* explosion;
    int worldWidth;
    int worldHeight;

    unsigned currentFrame;
    unsigned numberOfFrames;
    unsigned frameInterval;
    float timeSinceLastFrame;
    int frameWidth;
    int frameHeight;

    void advanceFrame(Uint32 ticks);

    int ground;

    int maxXSpeed;
    int maxYSpeed;
    int gravity;
    int traction;

    int xMove;

    int face;
    int jumpFrame;
    int standFrame;

    float speedMultiplier;
    float frameMultiplier;

    std::vector<Gun*> guns;
    int currentGun;
    bool shooting;
    int health;
    bool godMode;
    HealthBar healthBar;
};

#endif
