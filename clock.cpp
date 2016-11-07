#include <cmath>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include "clock.h"
#include "gamedata.h"
#include "ioManager.h"

Clock& Clock::getInstance() {
    if ( SDL_WasInit(SDL_INIT_VIDEO) == 0) {
        throw std::string("Must init SDL before Clock");
    }
    static Clock clock;
    return clock;
}

Clock::Clock() :
    started(false),
    paused(false),
    sloMo(false),
    framesAreCapped(Gamedata::getInstance().getXmlBool("framesAreCapped")),
    frameCap(Gamedata::getInstance().getXmlInt("frameCap")),
    frames(0),
    tickSum(0),
    sumOfAllTicks(0),
    timeAtStart(0), timeAtPause(0),
    currTicks(0), prevTicks(0), ticks(0),
    fpsFrames(Gamedata::getInstance().getXmlInt("frameMax"))
{
    start();
}

Clock::Clock(const Clock& c) :
    started(c.started),
    paused(c.paused),
    sloMo(c.sloMo),
    framesAreCapped(c.framesAreCapped),
    frameCap(c.frameCap),
    frames(c.frames),
    tickSum(c.tickSum),
    sumOfAllTicks(c.sumOfAllTicks),
    timeAtStart(c.timeAtStart), timeAtPause(c.timeAtPause),
    currTicks(c.currTicks), prevTicks(c.prevTicks), ticks(c.ticks),
    fpsFrames(c.fpsFrames)
{
    start();
}

void Clock::display() const {
    static unsigned int seconds = getSeconds();

    if ( getSeconds() > seconds ) {
        seconds = getSeconds();
    }
    

    std::ostringstream buff;
    buff << seconds;
    string num = buff.str();
    IOManager::getInstance()
    .printInHud("Seconds: " + num, 0);

    std::ostringstream bufff;
    bufff << getFPS();
    num = bufff.str();
    IOManager::getInstance()
    .printInHud("FPS: " + num, 25);
}

unsigned int Clock::getFPS() const {
    static std::vector<int> frameTime;
    frameTime.resize(fpsFrames);
    static unsigned int frameCount = 0;
    static unsigned int fpsSum = 0;
    static unsigned int prevTicks = 0;

    unsigned int fps = 0;

    unsigned int ticksPerFrame = (getTicks() - prevTicks);
    prevTicks = getTicks();

    fpsSum += ticksPerFrame;
    fpsSum -= frameTime[frameCount%fpsFrames];
    frameTime[frameCount%fpsFrames] = ticksPerFrame;

    frameCount++;
    if (frameCount < fpsFrames)
        fps = round((float)(1000 * frameCount) / fpsSum);
    else
        fps = round((float)(1000 * fpsFrames) / fpsSum);
    return fps;
}

void Clock::toggleSloMo() {
    if( started && !sloMo ) {
        timeAtPause = SDL_GetTicks() - timeAtStart;
        sloMo = true;
    }
    else if ( started && sloMo ) {
        timeAtStart = SDL_GetTicks() - timeAtPause;
        sloMo = false;
    }
}

unsigned int Clock::getTicks() const {
    if (paused) return timeAtPause;
    else if ( sloMo ) return 1;
    else return SDL_GetTicks() - timeAtStart;
}

unsigned int Clock::getElapsedTicks() {
    if (paused) return 0;
    else if ( sloMo ) return 1;

    currTicks = getTicks();
    ticks = currTicks-prevTicks;
    unsigned int delay = capFrameRate();
    prevTicks = currTicks + delay;
    ticks += delay;
    sumOfAllTicks += ticks;
    return ticks;
}

unsigned int Clock::capFrameRate() const {
    if ( !framesAreCapped ) return 0u;
    unsigned int delay = std::max(0.0,1000.0/frameCap+0.5 - ticks);
    SDL_Delay( delay );
    return delay;
}

Clock& Clock::operator++() {
    if ( !paused ) {
        ++frames;
    }
    return *this;
}

void Clock::start() {
    started = true;
    paused = false;
    frames = 0;
    timeAtPause = timeAtStart = SDL_GetTicks();
}
void Clock::pause() {
    if( started && !paused ) {
        timeAtPause = SDL_GetTicks() - timeAtStart;
        paused = true;
    }
}
void Clock::unpause() {
    if( started && paused ) {
        timeAtStart = SDL_GetTicks() - timeAtPause;
        paused = false;
    }
}

