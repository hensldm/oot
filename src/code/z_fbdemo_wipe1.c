#include "global.h"

typedef enum {
    /* 0 */ TRANS_WIPE_DIR_IN,
    /* 1 */ TRANS_WIPE_DIR_OUT
} TransitionWipeDirection;

#include "assets/code/fbdemo_wipe1/z_fbdemo_wipe1.c"

Gfx sTransWipeDL[] = {
    gsDPPipeSync(),
    gsSPClearGeometryMode(G_ZBUFFER | G_SHADE | G_CULL_BOTH | G_FOG | G_LIGHTING | G_TEXTURE_GEN |
                          G_TEXTURE_GEN_LINEAR | G_LOD | G_SHADING_SMOOTH),
    gsSPSetGeometryMode(G_ZBUFFER | G_SHADE | G_SHADING_SMOOTH),
    gsDPSetOtherMode(G_AD_DISABLE | G_CD_MAGICSQ | G_CK_NONE | G_TC_FILT | G_TF_BILERP | G_TT_NONE | G_TL_TILE |
                         G_TD_CLAMP | G_TP_PERSP | G_CYC_2CYCLE | G_PM_1PRIMITIVE,
                     G_AC_NONE | G_ZS_PRIM | G_RM_PASS | G_RM_AA_ZB_TEX_EDGE2),
    gsDPSetCombineLERP(TEXEL1, TEXEL0, PRIM_LOD_FRAC, TEXEL0, TEXEL1, TEXEL0, PRIM_LOD_FRAC, TEXEL0, COMBINED, 0,
                       PRIMITIVE, 0, COMBINED, 0, PRIMITIVE, 0),
    gsDPSetPrimDepth(0, 0),
    gsDPLoadTextureBlock_4b(sTransWipeTex, G_IM_FMT_I, 64, 64, 0, G_TX_NOMIRROR | G_TX_WRAP, G_TX_MIRROR | G_TX_WRAP, 6,
                            6, 11, G_TX_NOLOD),
    gsDPLoadMultiBlock_4b(sTransWipeTex, 0x0100, 1, G_IM_FMT_I, 64, 64, 0, G_TX_NOMIRROR | G_TX_WRAP,
                          G_TX_MIRROR | G_TX_WRAP, 6, 6, 11, 1),
    gsDPSetTextureLUT(G_TT_NONE),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsSPDisplayList(0x08000000),
    gsSPVertex(sTransWipeVtx, 25, 0),
    gsSP2Triangles(0, 1, 2, 0, 1, 3, 4, 0),
    gsSP2Triangles(5, 6, 7, 0, 6, 8, 9, 0),
    gsSP2Triangles(8, 10, 11, 0, 10, 12, 13, 0),
    gsSP2Triangles(12, 14, 15, 0, 14, 16, 17, 0),
    gsSP2Triangles(16, 18, 19, 0, 18, 20, 21, 0),
    gsSP2Triangles(20, 22, 23, 0, 22, 0, 24, 0),
    gsSPEndDisplayList(),
};

// unused.
Gfx sTransWipeSyncDL[] = {
    gsDPPipeSync(),
    gsSPEndDisplayList(),
};

void TransitionWipe_Start(TransitionInstance* this) {
    this->wipe.isDone = false;

    if (this->wipe.direction != TRANS_WIPE_DIR_IN) {
        this->wipe.texY = (s32)(83.25f * (1 << 2));
    } else {
        this->wipe.texY = (s32)(153.0f * (1 << 2));
    }

    guPerspective(&this->wipe.projection, &this->wipe.normal, 60.0f, (4.0f / 3.0f), 10.0f, 12800.0f, 1.0f);
    guLookAt(&this->wipe.lookAt, 0.0f, 0.0f, 400.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
}

void* TransitionWipe_Init(TransitionInstance* this) {
    bzero(this, sizeof(TransitionWipe));
    return this;
}

void TransitionWipe_Destroy(TransitionInstance* this) {
}

void TransitionWipe_Update(TransitionInstance* this, s32 updateRate) {
    if (this->wipe.direction != TRANS_WIPE_DIR_IN) {
        this->wipe.texY += (((void)0, gSaveContext.transWipeSpeed) * 3) / updateRate;
        if (this->wipe.texY >= (s32)(153.0f * (1 << 2))) {
            this->wipe.texY = (s32)(153.0f * (1 << 2));
            this->wipe.isDone = true;
        }
    } else {
        this->wipe.texY -= (((void)0, gSaveContext.transWipeSpeed) * 3) / updateRate;
        if (this->wipe.texY <= (s32)(83.25f * (1 << 2))) {
            this->wipe.texY = (s32)(83.25f * (1 << 2));
            this->wipe.isDone = true;
        }
    }
}

void TransitionWipe_Draw(TransitionInstance* this, Gfx** gfxP) {
    Gfx* gfx = *gfxP;
    Mtx* modelView;
    s32 pad[5];
    Gfx* texScroll;

    modelView = this->wipe.modelView[this->wipe.frame];
    this->wipe.frame ^= 1;

    guScale(&modelView[0], 0.56f, 0.56f, 1.0f);
    guRotate(&modelView[1], 0.0f, 0.0f, 0.0f, 1.0f);
    guTranslate(&modelView[2], 0.0f, 0.0f, 0.0f);
    gDPPipeSync(gfx++);
    texScroll = Gfx_BranchTexScroll(&gfx, this->wipe.texX, this->wipe.texY, 0, 0);
    gSPSegment(gfx++, 8, texScroll);
    gDPSetPrimColor(gfx++, 0, 0x80, this->wipe.color.r, this->wipe.color.g, this->wipe.color.b, 255);
    gSPMatrix(gfx++, &this->wipe.projection, G_MTX_LOAD | G_MTX_PROJECTION);
    gSPPerspNormalize(gfx++, this->wipe.normal);
    gSPMatrix(gfx++, &this->wipe.lookAt, G_MTX_MUL | G_MTX_PROJECTION);
    gSPMatrix(gfx++, &modelView[0], G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPMatrix(gfx++, &modelView[1], G_MTX_NOPUSH | G_MTX_MUL | G_MTX_MODELVIEW);
    gSPMatrix(gfx++, &modelView[2], G_MTX_NOPUSH | G_MTX_MUL | G_MTX_MODELVIEW);
    gSPDisplayList(gfx++, sTransWipeDL);
    gDPPipeSync(gfx++);
    *gfxP = gfx;
}

s32 TransitionWipe_IsDone(TransitionInstance* this) {
    return this->wipe.isDone;
}

void TransitionWipe_SetType(TransitionInstance* this, s32 type) {
    if (type == TRANS_INSTANCE_TYPE_FILL_OUT) {
        this->wipe.direction = TRANS_WIPE_DIR_OUT;
    } else {
        this->wipe.direction = TRANS_WIPE_DIR_IN;
    }

    if (this->wipe.direction != TRANS_WIPE_DIR_IN) {
        this->wipe.texY = (s32)(83.25f * (1 << 2));
    } else {
        this->wipe.texY = (s32)(153.0f * (1 << 2));
    }
}

void TransitionWipe_SetColor(TransitionInstance* this, u32 color) {
    this->wipe.color.rgba = color;
}

void TransitionWipe_SetUnkColor(TransitionInstance* this, u32 color) {
    this->wipe.unkColor.rgba = color;
}
