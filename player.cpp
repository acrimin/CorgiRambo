#include "player.h"
#include "gamedata.h"
#include "frameFactory.h"
#include <cmath>

void Player::advanceFrame(Uint32 ticks) {
    timeSinceLastFrame += ticks;
    if (timeSinceLastFrame > frameInterval * frameMultiplier) {
        currentFrame = (currentFrame+2) % numberOfFrames;
        timeSinceLastFrame = 0;
    }
}

Player::Player( const std::string& name) :
    Drawable(name,
             Vector2f(Gamedata::getInstance().getXmlInt(name+"/startLoc/x"),
                      Gamedata::getInstance().getXmlInt(name+"/startLoc/y")),
             Vector2f(0, 0)),
    frames( FrameFactory::getInstance().getFrames(name) ),
    worldWidth(Gamedata::getInstance().getXmlInt("world/width")),
    worldHeight(Gamedata::getInstance().getXmlInt("world/height")),

    currentFrame(0),
    numberOfFrames( Gamedata::getInstance().getXmlInt(name+"/frames") ),
    frameInterval( Gamedata::getInstance().getXmlInt(name+"/frameInterval") ),
    timeSinceLastFrame( 0 ),
    frameWidth(frames[0]->getWidth()),
    frameHeight(frames[0]->getHeight()),

    ground(Gamedata::getInstance().getXmlInt(name+"/startLoc/y")),
    maxXSpeed(Gamedata::getInstance().getXmlInt(name+"/speedX")),
    maxYSpeed(Gamedata::getInstance().getXmlInt(name+"/speedY")),
    gravity(Gamedata::getInstance().getXmlInt("world/gravity")),
    traction(Gamedata::getInstance().getXmlInt(name+"/traction")),
    xMove(0),
    face(0),
    jumpFrame(0),
    standFrame(4),
    speedMultiplier(1),
    frameMultiplier(1)
{ 
}

Player::Player(const Player& s) :
    Drawable(s),
    frames(s.frames),
    worldWidth( s.worldWidth ),
    worldHeight( s.worldHeight ),
    currentFrame(s.currentFrame),
    numberOfFrames( s.numberOfFrames ),
    frameInterval( s.frameInterval ),
    timeSinceLastFrame( s.timeSinceLastFrame ),
    frameWidth( s.frameWidth ),
    frameHeight( s.frameHeight ),
    ground(s.ground),
    maxXSpeed(s.maxXSpeed),
    maxYSpeed(s.maxYSpeed),
    gravity(s.gravity),
    traction(s.traction),
    xMove(s.xMove),
    face(s.face),
    jumpFrame(s.jumpFrame),
    standFrame(s.standFrame),
    speedMultiplier(s.speedMultiplier),
    frameMultiplier(s.frameMultiplier)
{ }

void Player::draw() const {
    Uint32 x = static_cast<Uint32>(X());
    Uint32 y = static_cast<Uint32>(Y());
    frames[currentFrame + face]->draw(x, y);
}

void Player::update(Uint32 ticks) {

    if (abs(velocityX()) > 0 && xMove == 0) {
        velocityX(velocityX() - // traction * abs(velocityX())/velocityX()); 
        round(static_cast<float>(traction) * 
        static_cast<float>(abs(velocityX())/velocityX()) * 
        static_cast<float>(ticks) * 0.001));   
    } else {
        velocityX(maxXSpeed * xMove * speedMultiplier);
    }
    if (Y() < ground) {
        velocityY(velocityY() + 
        round(static_cast<float>(gravity) * static_cast<float>(ticks) * 0.001));
    }

    if ( Y() > ground) {
        Y(ground);
        velocityY(0);
    }

    if ( X() < 0 && velocityX() < 0) {
        velocityX(0);
    }
    if ( X() > worldWidth-frameWidth && velocityX() > 0) {
        velocityX(0);
    }

    advanceFrame(ticks);

    if (xMove == 0) {
        currentFrame = standFrame;
    }
    if (Y() < ground) {
        currentFrame = jumpFrame;
    }

    Vector2f incr = getVelocity() * static_cast<float>(ticks) * 0.001;
    setPosition(getPosition() + incr);

}

void Player::left(bool down) {
    xMove = 0;
    if (down) {
        xMove = -1;
        face = 1;
    }
}

void Player::right(bool down) {
    xMove = 0;
    if (down) {
        xMove = 1; 
        face = 0;
    }
}

void Player::up() {
    if (Y() == ground)
        velocityY(maxYSpeed);
}

void Player::shift(bool down) {
    speedMultiplier = 1;
    frameMultiplier = 1;
    if (down) {
        speedMultiplier = 2;
        frameMultiplier = 0.5;
    } 
}

