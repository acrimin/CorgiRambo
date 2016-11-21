#ifndef PLAYER__H
#define PLAYER__H
#include <string>
#include <vector>
#include "drawable.h"
#include "gun.h"

class Player : public Drawable {
public:
    Player(const std::string&, Gun*);
    Player(const Player&);
    virtual ~Player() { }

    virtual void draw() const;
    virtual void update(Uint32 ticks);
    virtual const Frame* getFrame() const {
        return frames[currentFrame];
    }
    void right(bool);
    void left(bool);
    void shift(bool);
    void shoot(bool);
    void changeGun(Gun* gun);
    void up();

protected:
    Player operator=(const Player&);
    const std::vector<Frame *> frames;
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

    Gun* gun;
    bool shooting;
};

#endif
