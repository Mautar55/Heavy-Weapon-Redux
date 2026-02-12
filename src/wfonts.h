#include "generated_assets.h"
#include "raylib.h"

struct Wfonts {
    Font Console;
    Font Regular;
};

static struct Wfonts Wfonts = {0};

void LoadFonts(float viewScale) {
    Wfonts.Console = LoadFontEx(ass_fonts_FiraCode_Regular_ttf,14*viewScale,0,0);
    Wfonts.Regular = LoadFontEx(ass_fonts_Georama_SemiCondensed_Bold_ttf,22*viewScale,0,0);
}

void UnloadFonts(void) {
    UnloadFont(Wfonts.Console);
    UnloadFont(Wfonts.Regular);
}