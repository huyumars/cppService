#ifndef _DEFINES
#define _DEFINES


#define NOCOPY(cls) \
  cls(const cls &) = delete;

#define NOASSIGN(cls) \
  cls & operator=(const cls &) = delete;

#define  NOCOPYASS(cls) \
   NOCOPY(cls)          \
   NOASSIGN(cls)        \

#endif
