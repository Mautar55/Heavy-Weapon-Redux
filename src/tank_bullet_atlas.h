//////////////////////////////////////////////////////////////////////////////////
//                                                                              //
// rTexpacker v5.5 Atlas Descriptor Code exporter v5.0                          //
//                                                                              //
// more info and bugs-report:  github.com/raylibtech/rtools                     //
// feedback and support:       ray[at]raylibtech.com                            //
//                                                                              //
// Copyright (c) 2019-2025 raylib technologies (@raylibtech)                    //
//                                                                              //
//////////////////////////////////////////////////////////////////////////////////

#define ATLAS_ATLAS_IMAGE_PATH      "atlas.png"
#define ATLAS_ATLAS_SPRITE_COUNT    5

// Atlas sprite properties
typedef struct rtpAtlasSprite {
    const char *nameId;
    const char *tag;
    int originX, originY;
    int positionX, positionY;
    int sourceWidth, sourceHeight;
    int padding;
    bool trimmed;
    int trimRecX, trimRecY, trimRecWidth, trimRecHeight;
    int colliderType;
    int colliderPosX, colliderPosY, colliderSizeX, colliderSizeY;
} rtpAtlasSprite;

// Atlas sprites array
static rtpAtlasSprite BulletAtlas[5] = {
    { "tank_bullet_1", "", 0, 0, 0, 0, 25, 25, 0, false, 7, 4, 11, 16, 0, 0, 0, 0, 0 },
    { "tank_bullet_2", "", 0, 0, 25, 0, 25, 25, 0, false, 5, 0, 15, 25, 0, 0, 0, 0, 0 },
    { "tank_bullet_3", "", 0, 0, 50, 0, 25, 25, 0, false, 5, 0, 15, 24, 0, 0, 0, 0, 0 },
    { "tank_bullet_4", "", 0, 0, 75, 0, 25, 25, 0, false, 5, 0, 15, 24, 0, 0, 0, 0, 0 },
    { "tank_bullet_5", "", 0, 0, 100, 0, 25, 25, 0, false, 5, 0, 15, 25, 0, 0, 0, 0, 0 },
};

enum bullet_atlas {
    TankBullet1 = 0,
    TankBullet2 = 1,
    TankBullet3 = 2,
    TankBullet4 = 3,
    TankBullet5 = 4,
};