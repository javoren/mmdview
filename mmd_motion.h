
#pragma once


#include <GL/gl.h>
#include <GL/glut.h>

#include <math.h>
#include <stdio.h>
#include <memory.h>
#include <map>
#include <string>

#include "mmd_model.h"

// MMDファイルのヘッダを表現するクラス
class VMD_Header{
public: // とりあえず
    char        VmdHeader[30];      // "Vocaloid Motion Data 0002"
    char        VmdModelName[20];   // 名前が入っている
    uint32_t    motion_count;

public:
    VMD_Header();
    virtual ~VMD_Header();
    void read(FILE* fp);

    // 必要最低限のアクセサを記述していく
    uint32_t    get_motion_count(){return motion_count;}
};

class VMD_Motion{
public:
    char        BoneName[15];           // ボーン名
    uint32_t    FrameNumber;            // フレーム番号
    float       px,py,pz;               // 位置
    Quaternion  rq;                     // 回転角(クォータニオン)
    uint8_t     Interpolation[4][4][4]; // 補完
public:
    void read(FILE* fp);
    VMD_Motion();
    ~VMD_Motion();
};

class VMD_File{
public:
    VMD_Header  vmd_header;             // ヘッダ
    VMD_Motion  *vmd_motion;            // モーションデータ
    int         motion_index;

public:
    VMD_File();
    ~VMD_File();

    void setMmdMotion(MMD_File* pmd, int frame_number);
    void read(const char* filename);
};

