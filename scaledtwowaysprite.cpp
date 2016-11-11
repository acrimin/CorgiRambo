#include "scaledtwowaysprite.h"
#include "gamedata.h"
#include "frameFactory.h"
#include <cmath>

void ScaledTwoWaySprite::advanceFrame(Uint32 ticks) {
    timeSinceLastFrame += ticks;
    if (timeSinceLastFrame > frameInterval) {
        currentFrame = (currentFrame+2) % numberOfFrames;
        timeSinceLastFrame = 0;
    }
}

ScaledTwoWaySprite::ScaledTwoWaySprite( const std::string& name) :
    Drawable(name, Vector2f(0, 0), Vector2f(0, 0)),
    
    minScale(Gamedata::getInstance().getXmlFloat(name+"/scale")),
    scale(roundf((minScale + 
        ((((float) rand()) / (float) RAND_MAX) * 
        (1 - minScale))) * 10) / 10),
    frames( FrameFactory::getInstance().getFrames(name, scale) ),
    worldWidth(Gamedata::getInstance().getXmlInt("world/width")),
    worldHeight(Gamedata::getInstance().getXmlInt("world/height")),

    currentFrame(0),
    numberOfFrames( Gamedata::getInstance().getXmlInt(name+"/frames") ),
    frameInterval( Gamedata::getInstance().getXmlInt(name+"/frameInterval") ),
    timeSinceLastFrame( 0 ),
    frameWidth(frames[0]->getWidth()),
    frameHeight(frames[0]->getHeight())
{ 

    frameWidth = frames[0]->getWidth();
    frameHeight = frames[0]->getHeight();

    float x = .25 + (((float) rand()) / (float) RAND_MAX) * (.75 - .25);
    float y = sqrt(1 - (x * x));
    x *= (rand() % 2 ? 1 : -1);
    y *= (rand() % 2 ? 1 : -1);
    int speed = Gamedata::getInstance().getXmlInt(name+"/speed");
    velocityX(x * scale * speed);
    velocityY(y * scale * speed);

    X(rand() % (worldWidth-frameWidth));
    Y(rand() % (worldHeight-frameHeight));
}

ScaledTwoWaySprite::ScaledTwoWaySprite(const ScaledTwoWaySprite& s) :
    Drawable(s),
    minScale(s.minScale),
    scale(s.scale),
    frames(s.frames),
    worldWidth( s.worldWidth ),
    worldHeight( s.worldHeight ),
    currentFrame(s.currentFrame),
    numberOfFrames( s.numberOfFrames ),
    frameInterval( s.frameInterval ),
    timeSinceLastFrame( s.timeSinceLastFrame ),
    frameWidth( s.frameWidth ),
    frameHeight( s.frameHeight )
{ }

void ScaledTwoWaySprite::draw() const {
    Uint32 x = static_cast<Uint32>(X());
    Uint32 y = static_cast<Uint32>(Y());
    if (velocityX() > 0) 
        frames[currentFrame]->draw(x, y);
    else
        frames[currentFrame+1]->draw(x,y);
}

void ScaledTwoWaySprite::update(Uint32 ticks) {
    advanceFrame(ticks);

    Vector2f incr = getVelocity() * static_cast<float>(ticks) * 0.001;
    setPosition(getPosition() + incr);

    if ( Y() < 0) {
        velocityY( abs( velocityY() ) );
    }
    if ( Y() > worldHeight-frameHeight-125) {
        velocityY( -abs( velocityY() ) );
    }

    if ( X() < 0) {
        velocityX( abs( velocityX() ) );
    }
    if ( X() > worldWidth-frameWidth) {
        velocityX( -abs( velocityX() ) );
    }

}
