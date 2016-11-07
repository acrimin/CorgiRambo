#include "twowaysprite.h"
#include "gamedata.h"
#include "frameFactory.h"

void TwoWaySprite::advanceFrame(Uint32 ticks) {
    timeSinceLastFrame += ticks;
    if (timeSinceLastFrame > frameInterval) {
        currentFrame = (currentFrame+2) % numberOfFrames;
        timeSinceLastFrame = 0;
    }
}

TwoWaySprite::TwoWaySprite( const std::string& name) :
    Drawable(name,
             Vector2f(Gamedata::getInstance().getXmlInt(name+"/startLoc/x"),
                      Gamedata::getInstance().getXmlInt(name+"/startLoc/y")),
             Vector2f(Gamedata::getInstance().getXmlInt(name+"/speedX"),
                      Gamedata::getInstance().getXmlInt(name+"/speedY"))
            ),
    frames( FrameFactory::getInstance().getFrames(name) ),
    worldWidth(Gamedata::getInstance().getXmlInt("world/width")),
    worldHeight(Gamedata::getInstance().getXmlInt("world/height")),

    currentFrame(0),
    numberOfFrames( Gamedata::getInstance().getXmlInt(name+"/frames") ),
    frameInterval( Gamedata::getInstance().getXmlInt(name+"/frameInterval") ),
    timeSinceLastFrame( 0 ),
    frameWidth(frames[0]->getWidth()),
    frameHeight(frames[0]->getHeight())
{ }

TwoWaySprite::TwoWaySprite(const TwoWaySprite& s) :
    Drawable(s),
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

void TwoWaySprite::draw() const {
    Uint32 x = static_cast<Uint32>(X());
    Uint32 y = static_cast<Uint32>(Y());
    if (velocityX() > 0) 
        frames[currentFrame]->draw(x, y);
    else
        frames[currentFrame+1]->draw(x,y);
}

void TwoWaySprite::update(Uint32 ticks) {
    advanceFrame(ticks);

    Vector2f incr = getVelocity() * static_cast<float>(ticks) * 0.001;
    setPosition(getPosition() + incr);

    if ( Y() < 0) {
        velocityY( abs( velocityY() ) );
    }
    if ( Y() > worldHeight-frameHeight) {
        velocityY( -abs( velocityY() ) );
    }

    if ( X() < 0) {
        velocityX( abs( velocityX() ) );
    }
    if ( X() > worldWidth-frameWidth) {
        velocityX( -abs( velocityX() ) );
    }

}
