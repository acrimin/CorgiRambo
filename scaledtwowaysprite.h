#ifndef SCALEDTWOWAYSPRITE__H
#define SCALEDTWOWAYSPRITE__H
#include <string>
#include <vector>
#include "drawable.h"

class ScaledTwoWaySprite : public Drawable {
public:
    ScaledTwoWaySprite(const std::string&);
    ScaledTwoWaySprite(const ScaledTwoWaySprite&);
    virtual ~ScaledTwoWaySprite() { }

    bool operator<(const ScaledTwoWaySprite& rhs) const {
        return scale < rhs.scale;
    }

    float getScale() const {
        return scale;
    }

    virtual void draw() const;
    virtual void update(Uint32 ticks);
    virtual const Frame* getFrame() const {
        return frames[currentFrame];
    }

protected:
    float minScale;
    float scale;

    std::vector<Frame *> frames;
    int worldWidth;
    int worldHeight;

    unsigned currentFrame;
    unsigned numberOfFrames;
    unsigned frameInterval;
    float timeSinceLastFrame;
    int frameWidth;
    int frameHeight;

    void advanceFrame(Uint32 ticks);
};

#endif
