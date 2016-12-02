#include "player.h"
#include "gamedata.h"
#include "frameFactory.h"
#include "gun.h"
#include "ioManager.h"
#include "clock.h"
#include "sound.h"
#include <cmath>

void Player::advanceFrame(Uint32 ticks) {
    timeSinceLastFrame += ticks;
    if (timeSinceLastFrame > frameInterval * frameMultiplier) {
        currentFrame = (currentFrame+2) % numberOfFrames;
        timeSinceLastFrame = 0;
    }
}

Player::Player(const std::string& name) :
    Drawable(name,
             Vector2f(Gamedata::getInstance().getXmlInt(name+"/startLoc/x"),
                      Gamedata::getInstance().getXmlInt(name+"/startLoc/y")),
             Vector2f(0, 0)),
    frames(FrameFactory::getInstance().getFrames(name)),
    explosion(NULL),
    worldWidth(Gamedata::getInstance().getXmlInt("world/width")),
    worldHeight(Gamedata::getInstance().getXmlInt("world/height")),

    currentFrame(0),
    numberOfFrames( Gamedata::getInstance().getXmlInt(name+"/frames") ),
    frameInterval( Gamedata::getInstance().getXmlInt(name+"/frameInterval") ),
    timeSinceLastFrame( 0 ),
    frameWidth(frames[0]->getWidth()),
    frameHeight(frames[0]->getHeight()),

    ground(Gamedata::getInstance().getXmlInt(name+"/startLoc/y")),
    laying(false),
    maxXSpeed(Gamedata::getInstance().getXmlInt(name+"/speedX")),
    maxYSpeed(Gamedata::getInstance().getXmlInt(name+"/speedY")),
    gravity(Gamedata::getInstance().getXmlInt("world/gravity")),
    traction(Gamedata::getInstance().getXmlInt(name+"/traction")),
    xMove(0),
    face(0),
    jumpFrame(0),
    standFrame(4),
    speedMultiplier(1),
    frameMultiplier(1),
    guns(),
    currentGun(0),
    shooting(false),
    health(Gamedata::getInstance().getXmlInt(name+"/health")),
    maxHealth(health),
    healthBar(health),
    healthEndTime(0),
    godMode(false)
{
    guns.push_back( new Gun("AK47", Vector2f(
                    Gamedata::getInstance().getXmlInt(name+"/AK47/displacement/x"),
                    Gamedata::getInstance().getXmlInt(name+"/AK47/displacement/y"))));
    guns.push_back( new Gun("Bazooka", Vector2f(
                    Gamedata::getInstance().getXmlInt(name+"/Bazooka/displacement/x"),
                    Gamedata::getInstance().getXmlInt(name+"/Bazooka/displacement/y"))));
    frames = FrameFactory::getInstance().getFrames(name + "/" + guns[currentGun]->getName());
}

Player::Player(const Player& s) :
    Drawable(s),
    frames(s.frames),
    explosion(s.explosion),
    worldWidth( s.worldWidth ),
    worldHeight( s.worldHeight ),
    currentFrame(s.currentFrame),
    numberOfFrames( s.numberOfFrames ),
    frameInterval( s.frameInterval ),
    timeSinceLastFrame( s.timeSinceLastFrame ),
    frameWidth( s.frameWidth ),
    frameHeight( s.frameHeight ),
    ground(s.ground),
    laying(s.laying),
    maxXSpeed(s.maxXSpeed),
    maxYSpeed(s.maxYSpeed),
    gravity(s.gravity),
    traction(s.traction),
    xMove(s.xMove),
    face(s.face),
    jumpFrame(s.jumpFrame),
    standFrame(s.standFrame),
    speedMultiplier(s.speedMultiplier),
    frameMultiplier(s.frameMultiplier),
    guns(s.guns),
    currentGun(s.currentGun),
    shooting(s.shooting),
    health(s.health),
    maxHealth(s.maxHealth),
    healthBar(s.healthBar),
    healthEndTime(s.healthEndTime),
    godMode(s.godMode)
{ }

Player::~Player() {
    if (explosion)
        delete explosion;

    for (unsigned int i = 0; i < guns.size(); ++i) {
        delete guns[i];
    }
}

void Player::draw() const {
    if (explosion) {
        explosion->draw();
        return;
    }
    Uint32 x = static_cast<Uint32>(X());
    Uint32 y = static_cast<Uint32>(Y());

    frames[currentFrame + face]->draw(x, y);

    guns[currentGun]->draw();

    if (Clock::getInstance().getTicks() < healthEndTime)
        healthBar.draw(getPosition() + Vector2f(frameWidth/2, -30), health);
}

void Player::update(Uint32 ticks) {
    if (explosion) {
        explosion->update(ticks);
        if (explosion->chunkCount() == 0) {
            delete explosion;
            explosion = NULL;
            health = maxHealth;
            setPosition(Vector2f(Gamedata::getInstance().getXmlInt(getName()+"/startLoc/x"), 50));
        } 
        return;
    }

    if (laying) {
        guns[currentGun]->update(ticks);
        return;
    }
    
    if (std::abs(velocityX()) > 0 && xMove == 0) {
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

    if (shooting) 
        guns[currentGun]->shoot(getPosition() + Vector2f(face ? frameWidth : 0, 0), 
                                                         face ? -1 : 1);
    guns[currentGun]->update(ticks);

    ground = 820;
}

bool Player::collidedWith(Drawable* d) {
    if (explosion)
        return false;
    Vector2f hitLoc = Vector2f(0,0);
    if (Collision::getInstance().executePerPixel(*this, *d, hitLoc)) {
        Y(Y()-3);
        ground = Y();
        return false;
    }
    return guns[currentGun]->collidedWith(d);
}

void Player::explode() {
    if (godMode) 
        return;
    Sprite sprite = Sprite(getName(), 
                           getPosition(), 
                           Vector2f(0,0), 
                           frames[currentFrame + face]);
    explosion = new ExplodingSprite(sprite);
}

bool Player::hurt(int damage) {
    if (explosion)
        return false;

    healthEndTime = Clock::getInstance().getTicks() + 2000;
    std::cout << "ouch " << damage << std::endl;

    if (godMode)
        return false;
    health -= damage;
    if (health <= 0) {
        SDLSound::getInstance()[7];
        explode();
        return true;
    }
    return false;
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

void Player::down(bool down) {
    if (down && Y() == ground) {
        frames = FrameFactory::getInstance().getFrames(getName());
        currentFrame = 6;
        Y(ground+20);
        laying = true;
    } else if (!down) {
        frames = FrameFactory::getInstance().getFrames(getName() + "/" + 
                                                       guns[currentGun]->getName());
        laying = false;
    }
}

void Player::changeGun() {
    if (laying)
        return;
    currentGun = (currentGun + 1) % guns.size();
    frames = FrameFactory::getInstance().getFrames(getName() + "/" + 
                                                   guns[currentGun]->getName());
}

void Player::shift(bool down) {
    speedMultiplier = 1;
    frameMultiplier = 1;
    if (down) {
        speedMultiplier = 2;
        frameMultiplier = 0.5;
    } 
}

void Player::shoot(bool down) {
    shooting = down;
}

void Player::toggleGodMode() {
    godMode = !godMode;
}
