#include "global.h"

#include "assets/code/fbdemo_triforce/z_fbdemo_triforce.c"

void TransitionTriforce_Start(TransitionInstance* this) {
    switch (this->triforce.state) {
        case 1:
        case 2:
            this->triforce.transPos = 1.0f;
            return;

        default:
            this->triforce.transPos = 0.03f;
            return;
    }
}

void* TransitionTriforce_Init(TransitionInstance* this) {
    bzero(this, sizeof(TransitionTriforce));
    guOrtho(&this->triforce.projection, -160.0f, 160.0f, -120.0f, 120.0f, -1000.0f, 1000.0f, 1.0f);
    this->triforce.transPos = 1.0f;
    this->triforce.state = 2;
    this->triforce.step = 0.015f;
    this->triforce.type = TRANS_INSTANCE_TYPE_FILL_OUT;

    return this;
}

void TransitionTriforce_Destroy(TransitionInstance* this) {
}

void TransitionTriforce_Update(TransitionInstance* this, s32 updateRate) {
    s32 i;

    for (i = updateRate; i > 0; i--) {
        if (this->triforce.state == 1) {
            this->triforce.transPos = CLAMP_MIN(this->triforce.transPos * (1.0f - this->triforce.step), 0.03f);
        } else if (this->triforce.state == 2) {
            this->triforce.transPos = CLAMP_MIN(this->triforce.transPos - this->triforce.step, 0.03f);
        } else if (this->triforce.state == 3) {
            this->triforce.transPos = CLAMP_MAX(this->triforce.transPos / (1.0f - this->triforce.step), 1.0f);
        } else if (this->triforce.state == 4) {
            this->triforce.transPos = CLAMP_MAX(this->triforce.transPos + this->triforce.step, 1.0f);
        }
    }
}

void TransitionTriforce_SetColor(TransitionInstance* this, u32 color) {
    this->triforce.color.rgba = color;
}

void TransitionTriforce_SetType(TransitionInstance* this, s32 type) {
    this->triforce.type = type;
}

// unused
void TransitionTriforce_SetState(TransitionInstance* this, s32 state) {
    this->triforce.state = state;
}

void TransitionTriforce_Draw(TransitionInstance* this, Gfx** gfxP) {
    Gfx* gfx = *gfxP;
    Mtx* modelView;
    f32 scale;
    s32 pad[2];
    f32 rotation = this->triforce.transPos * 360.0f;

    modelView = this->triforce.modelView[this->triforce.frame];
    scale = this->triforce.transPos * 0.625f;
    this->triforce.frame ^= 1;
    PRINTF("rate=%f tx=%f ty=%f rotate=%f\n", this->triforce.transPos, 0.0f, 0.0f, rotation);
    guScale(&modelView[0], scale, scale, 1.0f);
    guRotate(&modelView[1], rotation, 0.0f, 0.0f, 1.0f);
    guTranslate(&modelView[2], 0.0f, 0.0f, 0.0f);
    gDPPipeSync(gfx++);
    gSPDisplayList(gfx++, sTransTriforceDL);
    gDPSetColor(gfx++, G_SETPRIMCOLOR, this->triforce.color.rgba);
    gDPSetCombineMode(gfx++, G_CC_PRIMITIVE, G_CC_PRIMITIVE);
    gSPMatrix(gfx++, &this->triforce.projection, G_MTX_LOAD | G_MTX_PROJECTION);
    gSPMatrix(gfx++, &modelView[0], G_MTX_LOAD);
    gSPMatrix(gfx++, &modelView[1], G_MTX_NOPUSH | G_MTX_MODELVIEW | G_MTX_MUL);
    gSPMatrix(gfx++, &modelView[2], G_MTX_NOPUSH | G_MTX_MODELVIEW | G_MTX_MUL);
    gSPVertex(gfx++, sTransTriforceVtx, 10, 0);

    if (!TransitionTriforce_IsDone(this)) {
        switch (this->triforce.type) {
            case TRANS_INSTANCE_TYPE_FILL_OUT:
                gSP2Triangles(gfx++, 0, 4, 5, 0, 4, 1, 3, 0);
                gSP1Triangle(gfx++, 5, 3, 2, 0);
                break;

            case TRANS_INSTANCE_TYPE_FILL_IN:
                gSP2Triangles(gfx++, 3, 4, 5, 0, 0, 2, 6, 0);
                gSP2Triangles(gfx++, 0, 6, 7, 0, 1, 0, 7, 0);
                gSP2Triangles(gfx++, 1, 7, 8, 0, 1, 8, 9, 0);
                gSP2Triangles(gfx++, 1, 9, 2, 0, 2, 9, 6, 0);
                break;
        }
    } else {
        switch (this->triforce.type) {
            case TRANS_INSTANCE_TYPE_FILL_OUT:
                break;

            case TRANS_INSTANCE_TYPE_FILL_IN:
                gSP1Quadrangle(gfx++, 6, 7, 8, 9, 0);
                break;
        }
    }
    gDPPipeSync(gfx++);
    *gfxP = gfx;
}

s32 TransitionTriforce_IsDone(TransitionInstance* this) {
    if (this->triforce.state == 1 || this->triforce.state == 2) {
        return this->triforce.transPos <= 0.03f;
    } else if (this->triforce.state == 3 || this->triforce.state == 4) {
        return this->triforce.transPos >= 1.0f;
    } else {
        return false;
    }
}
