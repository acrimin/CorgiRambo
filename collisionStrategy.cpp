#include <vector>
#include <algorithm>
#include "ioManager.h"
#include "collisionStrategy.h"
#include "viewport.h"

Collision& Collision::getInstance() {
    static Collision instance;
    return instance;
}

bool Collision::executeRectangle(
      const Drawable& obj1, const Drawable& obj2) const {
  float left1 = obj1.X();
  float left2 = obj2.X();
  float right1 = left1+obj1.getFrame()->getWidth();
  float right2 = left2+obj2.getFrame()->getWidth();
  if ( right1 < left2 ) return false;
  if ( left1 > right2 ) return false;
  float top1 = obj1.Y();
  float top2 = obj2.Y();
  float bottom1 = top1+obj1.getFrame()->getHeight();
  float bottom2 = top2+obj2.getFrame()->getHeight();
  if ( bottom1 < top2 ) return false;
  if ( bottom2 < top1 ) return false;
  return true;
}

// The next function was contributed by Mike Kabbani.
// The function handles transparency for both sprite
// images that explicity assign a color key and for sprite
// images that use an alpha channel.
bool Collision::
isVisible(Uint32 pixel, SDL_Surface *surface) const {
	Uint32 temp;
	SDL_PixelFormat* fmt = surface->format;
	if(fmt->BitsPerPixel == 32){
		temp=pixel&fmt->Amask; /* Isolate alpha component */
		temp=temp>>fmt->Ashift;/* Shift it down to 8-bit */
		temp=temp<<fmt->Aloss; /* Expand to a full 8-bit number */
		if(temp == 0) return false;
	}
	if(pixel == surface->format->colorkey) return false;
	return true;
}

bool Collision::executePerPixel(
    const Drawable& obj1, const Drawable& obj2) const {
   
    Vector2f vec = Vector2f(0,0); 
    bool val = executePerPixel(obj1, obj2, vec);
    return val;
}

bool Collision::executePerPixel(
      const Drawable& obj1, const Drawable& obj2, Vector2f& hitLoc) const {

  if ( not executeRectangle(obj1, obj2) ) return false;
  // If we got this far, we know that the sprite rectangles intersect!

  Vector2f p1 = obj1.getPosition() - Viewport::getInstance().getPosition();
  Vector2f p2 = obj2.getPosition() - Viewport::getInstance().getPosition();

  const Frame * const frame1 = obj1.getFrame();
  const Frame * const frame2 = obj2.getFrame();
  Uint16 width1 = frame1->getWidth();
  Uint16 width2 = frame2->getWidth();
  Uint16 height1 = frame1->getHeight();
  Uint16 height2 = frame2->getHeight();

  int o1Left = p1[0]; 
  int o1Right = o1Left+width1;

  int o2Left = p2[0]; 
  int o2Right = o2Left+width2;
  std::vector<int> sides;
  sides.reserve(4);
  sides.push_back( o1Left );
  sides.push_back( o1Right );
  sides.push_back( o2Left );
  sides.push_back( o2Right );
  std::sort( sides.begin(), sides.end() );


  int o1Up = p1[1];
  int o1Down = o1Up+height1;
  int o2Up = p2[1];
  int o2Down = o2Up+height2;
  std::vector<int> lids;
  lids.reserve(4);
  lids.push_back( o1Up );
  lids.push_back( o1Down );
  lids.push_back( o2Up );
  lids.push_back( o2Down );
  std::sort( lids.begin(), lids.end() );

  SDL_Surface* surface1 = frame1->getSurface();
  SDL_Surface* surface2 = frame2->getSurface();
  SDL_LockSurface(surface1);
  SDL_LockSurface(surface2);
  Uint32 *pixels1 = static_cast<Uint32 *>(surface1->pixels);
  Uint32 *pixels2 = static_cast<Uint32 *>(surface2->pixels);
  unsigned pixel1;
  unsigned pixel2;
  for (int x = sides[1]; x < sides[2]; ++x) {
    for (int y = lids[1]; y < lids[2]; ++y) {
      // check pixels in surface for obj1 and surface for obj2!
      int i = x - p1[0];
      int j = y - p1[1];
      pixel1 = pixels1[ (j * width1) + i ];
      i = x - p2[0];
      j = y - p2[1];
      pixel2 = pixels2[ (j * width2) + i ];

      if ( isVisible(pixel1,surface1) && isVisible(pixel2,surface2) ) {
        SDL_UnlockSurface(surface1);
        SDL_UnlockSurface(surface2);
        hitLoc[0] = x-p1[0];
        hitLoc[1] = y-p1[1];
        return true;
      }
    }
  }
  SDL_UnlockSurface(surface1);
  SDL_UnlockSurface(surface2);

  return false;
}

