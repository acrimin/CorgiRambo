#include <vector>
#include <SDL.h>
#include "ioManager.h"
#include "clock.h"
#include "world.h"
#include "viewport.h"
#include "player.h"
#include "npc.h"
#include "scaledtwowaysprite.h"

class Manager {
public:
    Manager ();
    ~Manager ();
    bool play();
    void switchSprite();

private:
    const bool env;
    const IOManager& io;
    Clock& clock;

    SDL_Surface * const screen;
    std::vector<World*> world;
    Viewport& viewport;

    std::vector<Drawable*> sprites;
    std::vector<NPC*> dying;
    std::vector<ScaledTwoWaySprite*> paintedSprites;
    Player* player;
    int currentSprite;

    bool makeVideo;
    int frameCount;
    const std::string username;
    const std::string title;
    const int frameMax;

    bool hud;
    bool two;

    void draw() const;
    void update();

    Manager(const Manager&);
    Manager& operator=(const Manager&);
    void makeFrame();
};
