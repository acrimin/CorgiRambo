#include "npc.h"
#include "clock.h"
#include "gamedata.h"
#include "frameFactory.h"
#include "sound.h"

void NPC::advanceFrame(Uint32 ticks) {
    timeSinceLastFrame += ticks;
    if (timeSinceLastFrame > frameInterval) {
        currentFrame = (currentFrame+2) % numberOfFrames;
        timeSinceLastFrame = 0;
    }
}

NPC::NPC(const std::string& name, Player& player) :
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
    frameHeight(frames[0]->getHeight()),

    enemy(player),
    health(Gamedata::getInstance().getXmlInt(name+"/health")),
    explosion(NULL),
    healthBar(health),
    healthEndTime(0),

    gun(new Gun(Gamedata::getInstance().getXmlStr(name+"/gun"), Vector2f(
                Gamedata::getInstance().getXmlInt(name+"/displacement/x"),
                Gamedata::getInstance().getXmlInt(name+"/displacement/y")))),
    xSpeed(Gamedata::getInstance().getXmlFloat(name+"/speedX")),
    ySpeed(Gamedata::getInstance().getXmlFloat(name+"/speedY")),
    face(0),
    attackDistance(Gamedata::getInstance().getXmlInt(name+"/attackDistance")),
    attackSpeed(Gamedata::getInstance().getXmlInt(name+"/attackSpeed")),
    ramDistance(Gamedata::getInstance().getXmlInt(name+"/ramDistance")),
    ramSpeed(Gamedata::getInstance().getXmlInt(name+"/ramSpeed")),
    fleeHealth(Gamedata::getInstance().getXmlInt(name+"/fleeHealth")),
    fleeSpeed(Gamedata::getInstance().getXmlInt(name+"/fleeSpeed")),
    chillDistance(Gamedata::getInstance().getXmlInt(name+"/chillDistance")),
    sound(Gamedata::getInstance().getXmlInt(name+"/sound"))
{}

NPC::NPC(const NPC& n) :
    Drawable(n),
    frames(n.frames),
    worldWidth(n.worldWidth),
    worldHeight(n.worldHeight),
    currentFrame(n.currentFrame),
    numberOfFrames(n.numberOfFrames),
    frameInterval(n.frameInterval),
    timeSinceLastFrame(n.timeSinceLastFrame),
    frameWidth(n.frameWidth),
    frameHeight(n.frameHeight),

    enemy(n.enemy),
    health(n.health),
    explosion(n.explosion),
    healthBar(n.healthBar),
    healthEndTime(n.healthEndTime),

    gun(n.gun),
    xSpeed(n.xSpeed),
    ySpeed(n.ySpeed),
    face(n.face),
    attackDistance(n.attackDistance),
    attackSpeed(n.attackSpeed),
    ramDistance(n.ramDistance),
    ramSpeed(n.ramSpeed),
    fleeHealth(n.fleeHealth),
    fleeSpeed(n.fleeSpeed),
    chillDistance(n.chillDistance),
    sound(n.sound)
{}

NPC::~NPC() {
    if (explosion)
        delete explosion;
}

void NPC::draw() const {
    if (explosion) {
        explosion->draw();
        return;
    }

    Uint32 x = static_cast<Uint32>(X());
    Uint32 y = static_cast<Uint32>(Y());
    frames[currentFrame+face]->draw(x,y);
    gun->draw();


    if (Clock::getInstance().getTicks() < healthEndTime)
        healthBar.draw(getPosition() + Vector2f(frameWidth/2, -30), health);
}

void NPC::update(Uint32 ticks) {
    if (explosion) {
        explosion->update(ticks);
        return;
    }

    advanceFrame(ticks);

    Vector2f incr = getVelocity() * static_cast<float>(ticks) * 0.001;
    setPosition(getPosition() + incr);

    control();
    gun->update(ticks);

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

    collidedWith(&enemy);
}

void NPC::shoot() {
//    std::cout << "Bang!" << std::endl;
}

void NPC::control() {
    float x= X()+getFrame()->getWidth()/2;
    float y= Y()+getFrame()->getHeight()/2;
    float ex= enemy.X()+enemy.getFrame()->getWidth()/2;
    float ey= enemy.Y()+enemy.getFrame()->getHeight()/2;
    float distanceToEnemy = distance( x, y, ex, ey );

    MODE currentMode;

    currentMode = ATTACK; 
    if (distanceToEnemy > chillDistance)
        currentMode = CHILL;
    if (health < fleeHealth && currentMode != CHILL)
        currentMode = FLEE;
    if (distanceToEnemy < ramDistance)
        currentMode = RAM;

    if (currentMode == ATTACK) {
        std::cout << "Attack" << std::endl;
        face = (x < ex) ? 0 : 1;
        gun->shoot(getPosition() + Vector2f(face ? frameWidth : 0, 0), 
                        face ? -1 : 1);
        if (distanceToEnemy > attackDistance) { // follow
            if ( x > ex ) 
                goLeft(attackSpeed);
            if ( x < ex ) 
                goRight(attackSpeed);
            if ( y > ey ) 
                goUp(attackSpeed);
            if ( y < ey ) 
                goDown(attackSpeed);
        } else {                                // avoid
            if ( x < ex ) 
                goLeft(attackSpeed);
            if ( x > ex ) 
                goRight(attackSpeed);
            if ( y < ey ) 
                goUp(attackSpeed);
            if ( y > ey ) 
                goDown(attackSpeed);
        }
    }

    if (currentMode == CHILL) {
//        std::cout << "Chill" << std::endl;
        goLeft(0);
        goRight(0);
        goUp(0);
        goDown(0);
    }

    if (currentMode == FLEE) {
//        std::cout << "Flee" << std::endl;
        face = (x > ex) ? 0 : 1;
            if ( x < ex ) 
                goLeft(fleeSpeed);
            if ( x > ex ) 
                goRight(fleeSpeed);
            if ( y < ey ) 
                goUp(fleeSpeed);
            if ( y > ey ) 
                goDown(fleeSpeed);
    }

    if (currentMode == RAM) {
//        std::cout << "Ram" << std::endl;
        face = (x < ex) ? 0 : 1;
            if ( x > ex ) 
                goLeft(ramSpeed);
            if ( x < ex ) 
                goRight(ramSpeed);
            if ( y > ey ) 
                goUp(ramSpeed);
            if ( y < ey ) 
                goDown(ramSpeed);
    }

}

void NPC::explode() {
    int face;
    if (velocityX() > 0) 
        face = 0;        
    else
        face = 1;

    Sprite sprite = Sprite(getName(), 
                           getPosition(), 
                           Vector2f(0,0), 
                           frames[currentFrame + face]);
    explosion = new ExplodingSprite(sprite);
}

bool NPC::doneExploding() {
    if (!explosion)
        return false;
    return (explosion->chunkCount() == 0);
}

bool NPC::hurt(int damage) {
    if (explosion)
        return false;
        
    healthEndTime = Clock::getInstance().getTicks() + 2000;

    health -= damage;
    if (health <= 0) {
        SDLSound::getInstance()[sound];
        explode();
        return true;
    }
    return false;
}

void NPC::goLeft(int speed) {
    velocityX(-abs(speed) * xSpeed); 
}

void NPC::goRight(int speed) {
    velocityX(abs(speed) * xSpeed); 
}

void NPC::goUp(int speed) {
    velocityY(-abs(speed) * ySpeed); 
}

void NPC::goDown(int speed) {
    velocityY(abs(speed) * ySpeed); 
}

float NPC::distance(float x1, float y1, float x2, float y2) {
    float x = x1-x2;
    float y = y1-y2;
    return hypot(x, y);
}

bool NPC::collidedWith(Drawable* d) {
    if (explosion)
        return false;

    Vector2f hitLoc = Vector2f(0,0);
    if (Collision::getInstance().executePerPixel(*this, *d, hitLoc)) {
        if (hitLoc[1] > 80)
            enemy.hurt(100000);
    }
    gun->collidedWith(d);

    return false;
}
