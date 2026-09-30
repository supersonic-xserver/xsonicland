#include "dixevents.h"
#include "dix/input_priv.h"

CursorPtr
GetSpriteCursor(DeviceIntPtr pDev)
{
    return InputDevGetSpriteCursor(pDev);
}
