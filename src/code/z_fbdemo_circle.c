#include "global.h"

typedef enum {
    /* 0 */ TRANS_CIRCLE_DIR_IN,
    /* 1 */ TRANS_CIRCLE_DIR_OUT
} TransitionCircleDirection;

// unused
Gfx sTransCircleEmptyDL[] = {
    gsSPEndDisplayList(),
};

#include "assets/code/fbdemo_circle/z_fbdemo_circle.c"

Gfx sTransCircleDL[] = {
    gsDPPipeSync(),
    gsSPClearGeometryMode(G_ZBUFFER | G_SHADE | G_CULL_BOTH | G_FOG | G_LIGHTING | G_TEXTURE_GEN |
                          G_TEXTURE_GEN_LINEAR | G_LOD | G_SHADING_SMOOTH),
    gsSPSetGeometryMode(G_SHADE | G_SHADING_SMOOTH),
    gsDPSetOtherMode(G_AD_DISABLE | G_CD_MAGICSQ | G_CK_NONE | G_TC_FILT | G_TF_BILERP | G_TT_NONE | G_TL_TILE |
                         G_TD_CLAMP | G_TP_PERSP | G_CYC_1CYCLE | G_PM_NPRIMITIVE,
                     G_AC_NONE | G_ZS_PIXEL | G_RM_XLU_SURF | G_RM_XLU_SURF2),
    gsDPSetCombineMode(G_CC_BLENDPEDECALA, G_CC_BLENDPEDECALA),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPLoadTextureBlock(0x08000000, G_IM_FMT_I, G_IM_SIZ_8b, 16, 64, 0, G_TX_NOMIRROR | G_TX_WRAP,
                         G_TX_NOMIRROR | G_TX_CLAMP, 4, 6, G_TX_NOLOD, G_TX_NOLOD),
    gsSPDisplayList(0x09000000),
    gsSPVertex(sTransCircleVtx, 32, 0),
    gsSP2Triangles(0, 1, 2, 0, 1, 3, 4, 0),
    gsSP2Triangles(3, 5, 6, 0, 5, 7, 8, 0),
    gsSP2Triangles(7, 9, 10, 0, 9, 11, 12, 0),
    gsSP2Triangles(11, 13, 14, 0, 13, 15, 16, 0),
    gsSP2Triangles(15, 17, 18, 0, 17, 19, 20, 0),
    gsSP2Triangles(19, 21, 22, 0, 21, 23, 24, 0),
    gsSP2Triangles(23, 25, 26, 0, 25, 27, 28, 0),
    gsSP1Triangle(27, 29, 30, 0),
    gsSPVertex(&sTransCircleVtx[31], 3, 0),
    gsSP1Triangle(0, 1, 2, 0),
    gsSPEndDisplayList(),
};

void TransitionCircle_Start(TransitionInstance* this) {
    this->circle.isDone = false;

    switch (this->circle.appearanceType) {
        case TCA_WAVE:
            this->circle.texture = sTransCircleWaveTex;
            break;

        case TCA_RIPPLE:
            this->circle.texture = sTransCircleRippleTex;
            break;

        case TCA_STARBURST:
            this->circle.texture = sTransCircleStarburstTex;
            break;

        default:
            this->circle.texture = sTransCircleNormalTex;
            break;
    }

    if (this->circle.speedType == TCS_FAST) {
        this->circle.speed = 20;
    } else {
        this->circle.speed = 10;
    }

    if (this->circle.colorType == TCC_BLACK) {
        this->circle.color.rgba = RGBA8(0, 0, 0, 255);
    } else if (this->circle.colorType == TCC_WHITE) {
        this->circle.color.rgba = RGBA8(160, 160, 160, 255);
    } else if (this->circle.colorType == TCC_GRAY) {
        this->circle.color.r = 100;
        this->circle.color.g = 100;
        this->circle.color.b = 100;
        this->circle.color.a = 255;
    } else {
        this->circle.speed = 40;
        this->circle.color.rgba =
            (this->circle.appearanceType == TCA_WAVE) ? RGBA8(0, 0, 0, 255) : RGBA8(160, 160, 160, 255);
    }

    if (this->circle.direction != TRANS_CIRCLE_DIR_IN) {
        this->circle.texY = (s32)(0.0f * (1 << 2));
        if (this->circle.colorType == TCC_SPECIAL) {
            this->circle.texY = (s32)(62.5f * (1 << 2));
        }
    } else {
        this->circle.texY = (s32)(125.0f * (1 << 2));
        if (this->circle.appearanceType == TCA_RIPPLE) {
            Audio_PlaySfxGeneral(NA_SE_OC_SECRET_WARP_OUT, &gSfxDefaultPos, 4, &gSfxDefaultFreqAndVolScale,
                                 &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
        }
    }

    guPerspective(&this->circle.projection, &this->circle.normal, 60.0f, (4.0f / 3.0f), 10.0f, 12800.0f, 1.0f);
    guLookAt(&this->circle.lookAt, 0.0f, 0.0f, 400.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
}

void* TransitionCircle_Init(TransitionInstance* this) {
    bzero(this, sizeof(TransitionCircle));
    return this;
}

void TransitionCircle_Destroy(TransitionInstance* this) {
}

void TransitionCircle_Update(TransitionInstance* this, s32 updateRate) {
    if (this->circle.direction != TRANS_CIRCLE_DIR_IN) {
        if (this->circle.texY == 0) {
            if (this->circle.appearanceType == TCA_RIPPLE) {
                Audio_PlaySfxGeneral(NA_SE_OC_SECRET_WARP_IN, &gSfxDefaultPos, 4, &gSfxDefaultFreqAndVolScale,
                                     &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
            }
        }
        this->circle.texY += this->circle.speed * 3 / updateRate;
        if (this->circle.texY >= (s32)(125.0f * (1 << 2))) {
            this->circle.texY = (s32)(125.0f * (1 << 2));
            this->circle.isDone = true;
        }
    } else {
        this->circle.texY -= this->circle.speed * 3 / updateRate;
        if (this->circle.colorType != TCC_SPECIAL) {
            if (this->circle.texY <= (s32)(0.0f * (1 << 2))) {
                this->circle.texY = (s32)(0.0f * (1 << 2));
                this->circle.isDone = true;
            }
        } else {
            if (this->circle.texY <= (s32)(62.5f * (1 << 2))) {
                this->circle.texY = (s32)(62.5f * (1 << 2));
                this->circle.isDone = true;
            }
        }
    }
}

void TransitionCircle_Draw(TransitionInstance* this, Gfx** gfxP) {
    Gfx* gfx = *gfxP;
    Mtx* modelView;
    Gfx* texScroll;
    s32 pad;
    // These variables are a best guess based on the other transition types.
    f32 tPos = 0.0f;
    f32 rot = 0.0f;
    f32 scale = 14.8f;

    modelView = this->circle.modelView[this->circle.frame];

    this->circle.frame ^= 1;
    gDPPipeSync(gfx++);
    texScroll = Gfx_BranchTexScroll(&gfx, this->circle.texX, this->circle.texY, 16, 64);
    gSPSegment(gfx++, 9, texScroll);
    gSPSegment(gfx++, 8, this->circle.texture);
    gDPSetColor(gfx++, G_SETPRIMCOLOR, this->circle.color.rgba);
    gDPSetColor(gfx++, G_SETENVCOLOR, this->circle.color.rgba);
    gSPMatrix(gfx++, &this->circle.projection, G_MTX_PROJECTION | G_MTX_LOAD);
    gSPPerspNormalize(gfx++, this->circle.normal);
    gSPMatrix(gfx++, &this->circle.lookAt, G_MTX_PROJECTION | G_MTX_NOPUSH | G_MTX_MUL);

    if (scale != 1.0f) {
        guScale(&modelView[0], scale, scale, 1.0f);
        gSPMatrix(gfx++, &modelView[0], G_MTX_LOAD);
    }

    if (rot != 0.0f) {
        guRotate(&modelView[1], rot, 0.0f, 0.0f, 1.0f);
        gSPMatrix(gfx++, &modelView[1], G_MTX_NOPUSH | G_MTX_MUL | G_MTX_MODELVIEW);
    }

    if ((tPos != 0.0f) || (tPos != 0.0f)) {
        guTranslate(&modelView[2], tPos, tPos, 0.0f);
        gSPMatrix(gfx++, &modelView[2], G_MTX_NOPUSH | G_MTX_MUL | G_MTX_MODELVIEW);
    }
    gSPDisplayList(gfx++, sTransCircleDL);
    gDPPipeSync(gfx++);
    *gfxP = gfx;
}

s32 TransitionCircle_IsDone(TransitionInstance* this) {
    return this->circle.isDone;
}

void TransitionCircle_SetType(TransitionInstance* this, s32 type) {
    if (type & TC_SET_PARAMS) {
        // SetType is called twice for circles, the actual direction value will be set on the second call.
        // The direction set here will be overwritten on that second call.
        this->circle.direction = (type >> 5) & 1;

        this->circle.colorType = (type >> 3) & 3;
        this->circle.speedType = type & 1;
        this->circle.appearanceType = (type >> 1) & 3;
    } else if (type == TRANS_INSTANCE_TYPE_FILL_OUT) {
        this->circle.direction = TRANS_CIRCLE_DIR_OUT;
    } else {
        this->circle.direction = TRANS_CIRCLE_DIR_IN;
    }
}

void TransitionCircle_SetColor(TransitionInstance* this, u32 color) {
    this->circle.color.rgba = color;
}

void TransitionCircle_SetUnkColor(TransitionInstance* this, u32 color) {
    this->circle.unkColor.rgba = color;
}
