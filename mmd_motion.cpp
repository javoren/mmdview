
#include <GL/gl.h>
#include <GL/glut.h>
#include <GL/glx.h>
#define GL_GLEXT_PROTOTYPES     
#include <GL/glext.h>

#include <math.h>
#include <string>
#include <stdio.h>
#include <memory.h>

#include "mmd_motion.h"

VMD_Header::VMD_Header()
{
    memset(VmdHeader, 0, 30);
    memset(VmdModelName, 0, 20);
    motion_count = 0;
}

VMD_Header::~VMD_Header()
{
};

void VMD_Header::read(FILE* fp)
{
    fread(VmdHeader, 1, 30, fp);
    fread(VmdModelName, 1, 20, fp);
    fread(&motion_count, 1, 4, fp);
    printf("VmdHeader = %s\n", VmdHeader);
    printf("VmdModelName = %s\n", VmdModelName);
    printf("motion count = %d\n", motion_count);
}

VMD_Motion::VMD_Motion()
{
}

VMD_Motion::~VMD_Motion()
{
}

void VMD_Motion::read(FILE* fp)
{
    // 以下のモーションデータを読み込むコードを記述する
    fread(&BoneName, 1, 15, fp);        // ボーン名
    fread(&FrameNumber, 4, 1, fp);      // フレーム番号
    fread(&px, 4, 1, fp);               // 位置
    fread(&py, 4, 1, fp);
    fread(&pz, 4, 1, fp);
    fread(&rq, 4, 4, fp);               // 回転角(クォータニオン)
    fread(Interpolation, 1, 4*4*4, fp); // 補完
}



VMD_File::VMD_File()
{
}

VMD_File::~VMD_File()
{
}

void VMD_File::read(const char* filename)
{
    FILE* fp = fopen(filename, "rb");
    vmd_header.read(fp);

    // モーションの数に合わせて、メモリ領域を確保する
    uint32_t motion_count = vmd_header.get_motion_count();
    vmd_motion = new VMD_Motion[motion_count];

    // すべてのモーションを読み込む
    // TODO : 今後の高速化ポイント
    for(uint32_t i = 0;i < motion_count; i++){
        vmd_motion[i].read(fp);
        printf("vmdmotion readed %d \n");
    }

}

