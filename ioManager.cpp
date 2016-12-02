#include <iostream>
#include <iomanip>
#include "ioManager.h"
#include "aaline.h"
using std::string;

IOManager& IOManager::getInstance() {
    static IOManager io;
    return io;
}
    
IOManager::~IOManager() {
    TTF_CloseFont(font);

//    std::vector<std::string>::iterator iter = hudMessages.begin();
//    while (hudMessages.begin() != hudMessages.end())
//        iter = hudMessages.erase(iter);
}

IOManager::IOManager( ) :
    hudMessages(),
    gdata( Gamedata::getInstance() ),
    viewWidth( gdata.getXmlInt("view/width") ),
    viewHeight( gdata.getXmlInt("view/height") ),
    MAX_STRING_SIZE( gdata.getXmlInt("maxStringSize") ),
    // The 3rd and 4th parameters are just as important as the first 2!
    screen(SDL_SetVideoMode(viewWidth, viewHeight, 32, SDL_DOUBLEBUF)),
    font( NULL ),
    color(),
    inputString("")
{
    if (screen == NULL) {
        throw string("Unable to set video mode; screen is NULL in IOMAnager");
    }
    if ( TTF_Init() == -1 ) {
        throw string("TTF_Init failed: ") + TTF_GetError();
    }
    font = TTF_OpenFont(
               gdata.getXmlStr("font/file").c_str(),
               gdata.getXmlInt("font/size")
           );
    if ( !font ) {
        throw string("TTF_OpenFont failed: ") + TTF_GetError();
    }
    color.r = gdata.getXmlInt("font/red");
    color.g = gdata.getXmlInt("font/green");
    color.b = gdata.getXmlInt("font/blue");
    color.unused = gdata.getXmlInt("font/unused");
    SDL_EnableUNICODE( SDL_ENABLE );
    atexit(TTF_Quit);
}

SDL_Surface* IOManager::loadAndSet(const string& filename, bool setcolorkey) const {
    SDL_Surface *tmp = IMG_Load(filename.c_str());
    if (tmp == NULL) {
        throw string("Unable to load bitmap ")+filename;
    }
    if ( setcolorkey ) {
        Uint32 colorkey = SDL_MapRGB(tmp->format, 255, 0, 255);
        SDL_SetColorKey(tmp, SDL_SRCCOLORKEY|SDL_RLEACCEL, colorkey);
    }
    // Optimize the strip for fast display
    SDL_Surface *image = SDL_DisplayFormatAlpha(tmp);
    if (image == NULL) {
        image = tmp;
    }
    else {
        SDL_FreeSurface(tmp);
    }
    return image;
}

void IOManager::printMessageAt(const string& msg, Sint16 x, Sint16 y) const {
    Uint16 zero = 0;
    SDL_Rect dest = {x,y,zero,zero};
    SDL_Surface * stext = TTF_RenderText_Blended(font, msg.c_str(), color);
    if (stext) {
        SDL_BlitSurface( stext, NULL, screen, &dest );
        SDL_FreeSurface(stext);
    }
    else {
        throw
        string("Couldn't allocate text sureface in printMessageAt");
    }
}

void IOManager::printMessageCenteredAt( const string& msg, Sint16 y) const {
    SDL_Surface *stext = TTF_RenderText_Blended(font, msg.c_str(), color);
    if (stext) {
        Sint16 x = ( viewWidth - stext->w ) / 2;
        SDL_Rect dest = {x,y,0,0};
        SDL_BlitSurface( stext, NULL, screen, &dest );
        SDL_FreeSurface(stext);
    }
    else {
        throw
        string("Couldn't allocate text sureface in printMessageCenteredAt");
    }
}

void IOManager::drawHud() const {
    Uint32 RED = SDL_MapRGB(screen->format, 153, 0, 0); 
    Uint32 REDL = SDL_MapRGBA(screen->format, 255, 255, 150, 255); 
   
    short x1 = 20, y1 = 20;
    short unsigned height = 400;
    short unsigned width = 170; 
    //                  x0  y0  x1 y1  thick color
    Draw_AALine(screen, x1, y1, x1, y1+height, 2, RED);
    Draw_AALine(screen, x1+width, y1, x1+width, y1+height, 2, RED);
    Draw_AALine(screen, x1, y1, x1+width, y1, 2, RED);
    Draw_AALine(screen, x1, y1+height, x1+width, y1+height, 2, RED);

    SDL_Surface* s = SDL_CreateRGBSurface(0, width, height, 32, 255, 255, 255, 255);
    SDL_Rect dest = {x1, y1, width, height};
    SDL_FillRect( s, NULL, REDL);
    SDL_BlitSurface( s, NULL, screen, &dest );

    SDL_FreeSurface(s);
}

void IOManager::printInHud(const std::string& msg, short y) const {
    Uint16 zero = 0;
    y += 25;
    SDL_Rect dest = {25, y,zero,zero};
    SDL_Surface * stext = TTF_RenderText_Blended(font, msg.c_str(), color);
    if (stext) {
        SDL_BlitSurface( stext, NULL, screen, &dest );
        SDL_FreeSurface(stext);
    }
    else {
        throw
        string("Couldn't allocate text sureface in printMessageAt");
    }
}

void IOManager::addHudMessage(const string& msg) {
    hudMessages.push_back(msg);
}

void IOManager::resetHudMessages() {
    hudMessages.erase(hudMessages.begin(), hudMessages.end());
}

void IOManager::printHudMessages() const {
    int space = 30;
    for (unsigned int i = 0; i < hudMessages.size(); ++i) {
        if (hudMessages[i][0] == '#') {
            space += atoi(hudMessages[i].substr(1).c_str());
        } else
            printInHud(hudMessages[i], space = space + 20);
    }
}



void IOManager::printStringAfterMessage( const string& msg,
        Uint32 x, Uint32 y ) const {
    printMessageAt(msg+inputString, x, y);
}

void IOManager::buildString(SDL_Event event) {
    if( inputString.size() <= MAX_STRING_SIZE) {
        unsigned ch = event.key.keysym.sym;
        if ( isalpha(ch) || isdigit(ch) || ch == ' ') {
            inputString += char(event.key.keysym.unicode);
        }
    }
    if( event.key.keysym.sym == SDLK_BACKSPACE
            && inputString.length() > 0 ) {
        // remove a character from the end
        int length = inputString.size();
        inputString.erase( length - 1 );
    }
}

