
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

//演算子のオーバーロード Quaternionの積
Quaternion & operator *( Quaternion &q1, Quaternion &q2 )
{
    Quaternion q0={
        q1.w * q2.w - q1.x * q2.x - q1.y * q2.y - q1.z * q2.z,
        q1.w * q2.x + q1.x * q2.w + q1.y * q2.z - q1.z * q2.y,
        q1.w * q2.y - q1.x * q2.z + q1.y * q2.w + q1.z * q2.x,
        q1.w * q2.z + q1.x * q2.y - q1.y * q2.x + q1.z * q2.w,
    };
    q1=q0;
    return q1;
}

//クォータニオンから回転行列を算出
// r = 16*16のopenGL座標系に対応した回転行列
void qtor(float *r , Quaternion q)
{
    double xx = q.x * q.x * 2.0;
    double yy = q.y * q.y * 2.0;
    double zz = q.z * q.z * 2.0;
    double xy = q.x * q.y * 2.0;
    double yz = q.y * q.z * 2.0;
    double zx = q.z * q.x * 2.0;
    double xw = q.x * q.w * 2.0;
    double yw = q.y * q.w * 2.0;
    double zw = q.z * q.w * 2.0;
    double r1[16]={ 1.0 - yy - zz, xy + zw, zx - yw, 0.0,
        xy - zw, 1.0 - zz - xx, yz + xw, 0.0,
        zx + yw, yz - xw, 1.0 - xx - yy, 0.0,
        0.0, 0.0, 0.0, 1.0};
    for (int i = 0;i < 16;i++) {
        r[i]=r1[i];
    }
}

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
//    printf("VmdHeader = %s\n", VmdHeader);
//    printf("VmdModelName = %s\n", VmdModelName);
//    printf("motion count = %d\n", motion_count);
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
    fread(BoneName, 1, 15, fp);        // ボーン名
    fread(&FrameNumber, 4, 1, fp);      // フレーム番号
    fread(&px, 4, 1, fp);               // 位置
    fread(&py, 4, 1, fp);
    fread(&pz, 4, 1, fp);
    fread(&rq, 4, 4, fp);               // 回転角(クォータニオン)
    fread(Interpolation, 1, 4*4*4, fp); // 補完

    printf("%d:BoneName = %s\n", FrameNumber, BoneName);
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
    for(uint32_t i = 0;i < motion_count; i++){
        vmd_motion[i].read(fp);
    }

}

