// View and Camera zoom

struct WGlobals {
    Vector2 ReferenceWindowSize;// = {800, 480};
    float refW;// = 1.0f;
    float refH;// = 1.0f;
    float rw;// = 1.0f;
    float rh;// = 1.0f;
    float viewScale;// = 1.0f;
};

struct WGlobals w;

// Font loading and use

struct WFonts {
    Font Console;
    Font Regular;
};

struct WFonts wfonts;

static void LoadFonts(float viewScale) {
    wfonts.Console = LoadFontEx(ass_fonts_FiraCode_Regular_ttf,14*viewScale,0,0);
    wfonts.Regular = LoadFontEx(ass_fonts_Georama_SemiCondensed_Bold_ttf,22*viewScale,0,0);
}

static void UnloadFonts(void) {
    UnloadFont(wfonts.Console);
    UnloadFont(wfonts.Regular);
}