#ifndef BP_DEFINES_H
#define BP_DEFINES_H

#include "Version.h"
#include "Export.h"

#ifndef BP_BEGIN_CDECLS
#   ifdef __cplusplus
#       define BP_BEGIN_CDECLS extern "C" {
#   else
#       define BP_BEGIN_CDECLS
#   endif
#endif // !BP_BEGIN_CDECLS

#ifndef BP_END_CDECLS
#   ifdef __cplusplus
#       define BP_END_CDECLS }
#   else
#       define BP_END_CDECLS
#   endif
#endif // !BP_END_CDECLS

#ifndef MAX_PATH
#define MAX_PATH 260
#endif

#ifndef __cplusplus
#include <stdbool.h>
#endif

#include <stdint.h>

#define BP_ENABLE_IMGUI 1

#define BP_DEFAULT_NAME "Game"
#define BP_DEFAULT_WIDTH 640
#define BP_DEFAULT_HEIGHT 480
#define BP_DEFAULT_BPP 32

#define BP_DESCRIPTION "The brand-new player for Ballance"

#endif // BP_DEFINES_H
