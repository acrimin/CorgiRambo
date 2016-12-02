#ifndef COLLISION__H
#define COLLISION__H

#include <cmath>
#include "drawable.h"

class Collision {
public:
  static Collision& getInstance();
  bool executePerPixel(const Drawable&, const Drawable&) const;
  bool executePerPixel(const Drawable&, const Drawable&, Vector2f& hitLoc) const;
  bool executeRectangle(const Drawable&, const Drawable&) const;
private:
  bool isVisible(Uint32, SDL_Surface*) const;
};

#endif
