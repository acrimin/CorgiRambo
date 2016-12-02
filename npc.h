#ifndef NPC__H
#define NPC__H
#include "drawable.h"
#include "explodingSprite.h"
#include "healthBar.h"
#include "player.h"
#include <string>
#include <vector>

class NPC : public Drawable {
public:
    NPC(const std::string&, Player& player);
    NPC(const NPC&);
    ~NPC();
    void update(Uint32);

    virtual const Frame* getFrame() const {
        return frames[currentFrame];
    }
    
    void draw() const;
    bool collidedWith(Drawable*);
    bool hurt(int);
    bool doneExploding();
    void goLeft(int);
    void goRight(int);
    void goUp(int);
    void goDown(int);

private:
    NPC& operator=(const NPC&);
    void explode();
    void control();
    void shoot();
    float distance(float, float, float, float);
    void advanceFrame(Uint32 ticks);

    std::vector<Frame *> frames;
    int worldWidth;
    int worldHeight;

    unsigned currentFrame;
    unsigned numberOfFrames;
    unsigned frameInterval;
    float timeSinceLastFrame;
    int frameWidth;
    int frameHeight;

    Player& enemy;
    int health;
    ExplodingSprite* explosion;
    HealthBar healthBar;
    unsigned healthEndTime;

    enum MODE {ATTACK, RAM, FLEE, CHILL};
    Gun* gun;
    float xSpeed;
    float ySpeed;
    int face;
    int attackDistance;
    int attackSpeed;
    int ramDistance;
    int ramSpeed;
    int fleeHealth;
    int fleeSpeed;
    int chillDistance;
    int sound;
};

#endif
