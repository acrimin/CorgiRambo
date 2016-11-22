#include <iostream>
#include <string>
#include <iomanip>
#include <algorithm>
#include <cmath>
#include "multisprite.h"
#include "twowaysprite.h"
#include "scaledtwowaysprite.h"
#include "sprite.h"
#include "npc.h"
#include "gamedata.h"
#include "manager.h"

class ScaledSpriteCompare {
public:
  bool operator()(const ScaledTwoWaySprite* lhs, const ScaledTwoWaySprite* rhs) {
    return lhs->getScale() < rhs->getScale();
  }
};

Manager::~Manager() {
    // These deletions eliminate "definitely lost" and
    // "still reachable"s in Valgrind.
    for (unsigned i = 0; i < world.size(); ++i) {
        delete world[i];
    }
    for (unsigned i = 0; i < sprites.size(); ++i) {
        delete sprites[i];
    }

    for (unsigned i = 0; i < paintedSprites.size(); ++i) {
        delete paintedSprites[i];
    }

    for (unsigned i = 0; i < dying.size(); ++i) {
        delete dying[i];
    }
}

Manager::Manager() :
    env( SDL_putenv(const_cast<char*>("SDL_VIDEO_CENTERED=center")) ),
    io( IOManager::getInstance() ),
    clock( Clock::getInstance() ),
    screen( io.getScreen() ),
    world(),
    viewport( Viewport::getInstance() ),
    sprites(),
    dying(),
    paintedSprites(),
    player(),
    currentSprite(0),

    makeVideo( false ),
    frameCount( 0 ),
    username(  Gamedata::getInstance().getXmlStr("username") ),
    title( Gamedata::getInstance().getXmlStr("screenTitle") ),
    frameMax( Gamedata::getInstance().getXmlInt("frameMax") ),
    hud(true),
    two(false)
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        throw string("Unable to initialize SDL: ");
    }
    SDL_WM_SetCaption(title.c_str(), NULL);
    atexit(SDL_Quit);
    world.push_back(new World("back", Gamedata::getInstance().getXmlInt("back/factor")));
    world.push_back(new World("mountains", Gamedata::getInstance().getXmlInt("mountains/factor")));
    world.push_back(new World("drive", Gamedata::getInstance().getXmlInt("drive/factor")));

    sprites.push_back( new Player("corgi") );
    sprites.push_back( new NPC("tank") );
    player = (Player*) sprites[0];
    
    for (int i = 0; i < Gamedata::getInstance().getXmlInt("birds/count"); i++) {
        paintedSprites.push_back( new ScaledTwoWaySprite("birds") );
    }

    sort(paintedSprites.begin(), paintedSprites.end(), ScaledSpriteCompare());    
    viewport.setObjectToTrack(sprites[currentSprite]);
}

void Manager::draw() const {
    for (unsigned i = 0; i < world.size(); ++i) {
        world[i]->draw();
        for (unsigned n = i * paintedSprites.size()/world.size(); 
                      n < ceil((i+1) * paintedSprites.size()/world.size());
                      n++) {
            paintedSprites[n]->draw();
        }
    }

    for (unsigned i = 0; i < sprites.size(); ++i) {
        sprites[i]->draw();
    }

    for (unsigned i = 0; i < dying.size(); ++i) {
        dying[i]->draw();
    }

    if (hud) {
        io.drawHud();
        clock.display();
        io.printHudMessages();
    }
    viewport.draw();

    SDL_Flip(screen);
}

void Manager::makeFrame() {
    std::stringstream strm;
    strm << "frames/" << username<< '.'
         << std::setfill('0') << std::setw(4)
         << frameCount++ << ".bmp";
    std::string filename( strm.str() );
    std::cout << "Making frame: " << filename << std::endl;
    SDL_SaveBMP(screen, filename.c_str());
}

void Manager::switchSprite() {
    currentSprite = (currentSprite+1) % sprites.size();
    viewport.setObjectToTrack(sprites[currentSprite]);
}

void Manager::update() {
    IOManager::getInstance().resetHudMessages();

    ++(clock);
    Uint32 ticks = clock.getElapsedTicks();

    // Check collisions
    std::vector<Drawable*>::iterator iter = sprites.begin();
    ++iter; // increment off player
    while (iter != sprites.end()) {
        if (player->collidedWith(*iter)) {
            std::cout << "killed" << std::endl;
            dying.push_back(static_cast<NPC*>(*iter));
            iter = sprites.erase(iter);
        } else
            ++iter;
    }

    static unsigned int lastSeconds = clock.getSeconds();
    if ( clock.getSeconds() - lastSeconds == 5 ) {
        lastSeconds = clock.getSeconds();
        //switchSprite();
    }

    for (unsigned int i = 0; i < sprites.size(); ++i) {
        sprites[i]->update(ticks);
    }

    std::vector<NPC*>::iterator it = dying.begin();
    while (it != dying.end()) {
        (*it)->update(ticks);
        if ((*it)->doneExploding())
            it = dying.erase(it);
        else
            ++it;
    }

    for (unsigned int i = 0; i < paintedSprites.size(); ++i) {
        paintedSprites[i]->update(ticks);
    }
    if ( makeVideo && frameCount < frameMax ) {
        makeFrame();
    }
    for (unsigned i = 0; i < world.size(); ++i) {
        world[i]->update();
    }
    viewport.update(); // always update viewport last
    
    if (clock.getTotalTicks() > 2000 && !two) {
        two = true;
        hud = false;
    }
    
    IOManager::getInstance().addHudMessage("Press T to switch");
    IOManager::getInstance().addHudMessage("sprites");
    IOManager::getInstance().addHudMessage("#10");
    IOManager::getInstance().addHudMessage("Tracking "+sprites[currentSprite]->getName());
    IOManager::getInstance().addHudMessage("#10");
    IOManager::getInstance().addHudMessage("F1 toggles HUD");
    IOManager::getInstance().addHudMessage("F2 Slo-Mo");
    IOManager::getInstance().addHudMessage("F4 Record");
    IOManager::getInstance().addHudMessage("P pause");
    IOManager::getInstance().addHudMessage("R restart");
    IOManager::getInstance().addHudMessage("#10");
    IOManager::getInstance().addHudMessage("A/D Left Right");
    IOManager::getInstance().addHudMessage("W Jump");
    IOManager::getInstance().addHudMessage("Left Shift Sprint");
    IOManager::getInstance().addHudMessage("Space Shoots");
    IOManager::getInstance().addHudMessage("E Toggles Gun");
}

bool Manager::play() {
    SDL_Event event;
    bool done = false;

    while ( not done ) {
        while ( SDL_PollEvent(&event) ) {
            Uint8 *keystate = SDL_GetKeyState(NULL);
            if (event.type ==  SDL_QUIT) {
                done = true;
                break;
            }
            if(event.type == SDL_KEYDOWN) {
                if (keystate[SDLK_ESCAPE] || keystate[SDLK_q]) {
                    done = true;
                    break;
                }
                if ( keystate[SDLK_t] ) {
                    switchSprite();
                }
                if ( keystate[SDLK_F2] ) {
                    clock.toggleSloMo();
                }
                if ( keystate[SDLK_F1] ) {
                    hud = !hud;
                }
                if ( keystate[SDLK_p] ) {
                    if ( clock.isPaused() ) clock.unpause();
                    else clock.pause();
                }
                if ( keystate[SDLK_r] ) {
                    return true;
                }
                if (keystate[SDLK_F4] && !makeVideo) {
                    std::cout << "Making video frames" << std::endl;
                    makeVideo = true;
                }

                /* Player Movement Data */
                if (keystate[SDLK_a]) {
                    player->left(true);
                }
                if (keystate[SDLK_d]) {
                    player->right(true);
                }
                if (keystate[SDLK_w]) {
                    player->up();
                }
                if (keystate[SDLK_e]) {
                    player->changeGun();
                }
                if (keystate[SDLK_LSHIFT]) {
                    player->shift(true);
                }
                if (keystate[SDLK_SPACE]) {
                    player->shoot(true);
                }

            }
            if (event.type == SDL_KEYUP) {
                if (event.key.keysym.sym == SDLK_a) {
                    player->left(false);
                }
                if (event.key.keysym.sym == SDLK_d) {
                    player->right(false);
                }
                if (event.key.keysym.sym == SDLK_LSHIFT) {
                    player->shift(false);
                }
                if (event.key.keysym.sym == SDLK_SPACE) {
                    player->shoot(false);
                }
            }


        }
        draw();
        update();
        
    }
    return false;
}
