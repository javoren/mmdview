
#pragma once

#include <GL/gl.h>
#include <GL/glut.h>

#include <math.h>
#include <string>
#include <stdio.h>
#include <memory.h>
#include <map>
#include <string>
#include <vector>


// 外部のライブラリ
// http://www.syuhitu.org/other/bmp/BmpIoLib_h.html
#include "BmpIoLib.h"


//クォータニオン構造体
struct Quaternion{
    float w;
    float x;
    float y;
    float z;
};


// MMDファイルのヘッダを表現するクラス
class MMD_Header{
private:
    char magic[3];
    float version;
    char model_name[20];
    char comment[256];

public:
    void read(FILE* fp);
    MMD_Header();
    virtual ~MMD_Header();
};


class MMD_face{
public:
    uint32_t    face_vert_count;
    GLuint      *face_index;

    void read(FILE* fp);
    MMD_face();
    virtual ~MMD_face();
    void draw();
};


class MMD_vertex{
public:
    float       x,y,z;
    float       nx,ny,nz;
    float       u,v;

    // モデルの表示だけならここのデータは使わないので、
    // とりあえず放置(readだけはしとくけども。)
    uint16_t    bone_num[2];
    uint8_t     bone_weight;
    uint8_t     edge_flag;

    MMD_vertex();
    virtual ~MMD_vertex();
    void read(FILE* fp);
};

// プリミティブに関する情報をすべて保持する
// (頂点, 法線, テクスチャ)
class MMD_VertexArray{
public:
    uint32_t    count;

    // ファイルから読み込んだデータ列を保持する
    MMD_vertex*     pVertex;

    //----------------------------------
    // OpenGLでレンダリングするためのバッファ領域を管理する
    //----------------------------------
    // vertex_bufferとindex_buffer
    GLfloat         *p3dVerted;
    GLfloat         *p3dNormal;
    GLfloat         *pTexuv;
    unsigned int    *pIndexBuffer;

    MMD_VertexArray();
    virtual ~MMD_VertexArray();
    void read(FILE* fp);
    void draw();
};


// テクスチャを管理するクラス
class Texture{
private:
    IBMP*   pBmp;
    GLuint  texture_id;

public:
    Texture(void);
    virtual ~Texture(void);

    void load(const char* filename, GLuint tex_id);
    unsigned int get_gl_texture_id(void);
};


// マテリアルデータを表すクラス
class Material{
private:
    float       diffuse_color[3];       // dr, dg, db
    float       alpha;
    float       specular;
    float       specular_color[3];
    float       mirror_color[3];
    uint8_t     toon_index;
    uint8_t     edge_flag;
    uint32_t    face_vert_count;
    char        texture_name[20+1];     // +1はNULL文字用
    Texture     texture;
    std::string path;
    unsigned int    tex_index;

public:
    Material(void);
    virtual ~Material();
    uint32_t get_face_vert_count(void);
    void setpath(std::string pathname);

    void read(FILE* fp, GLuint tex_id);

    void draw(uint32_t start_face);
};

// マテリアルの配列
class MaterialArray{
private:
    uint32_t    count;
    Material*   mat_array;
    std::string path;
    GLuint      *textureIds;

public:
    MaterialArray();
    virtual ~MaterialArray();
    void setpath(std::string pathname);
    void read(FILE* fp);

    void draw(void);
};

// ボーン情報
class MMD_Bone{
public: // 暫定
    char        bone_name[20];          // ボーン名
    uint16_t    parent_bone_index;      // 親ボーンインデックス
    uint16_t    tail_pos_bone_index;    // tail位置のボーンインデックス
    uint8_t     bone_type;              // ボーンの種類
    uint16_t    ik_parent_bone_index;   // IKボーン番号(ない場合は0)
    float       bone_head_pos[3];       // ボーンのヘッドの位置

    // 描画用の情報
    // ファイルフォーマットには含まれていない。
    // TODO : ファイルフォーマットとは分離したい
    float       tx,ty,tz;
    Quaternion  quot;
    float       rot_mat[4*4];
public:
    MMD_Bone();
    ~MMD_Bone();
    void read(FILE* fp);
    void draw();
    void set_translate(float tx, float ty, float tz);
    void set_rotation(float qx, float qy, float qz, float qw);
};

// 親子関係をもとにボーンを計算するためのクラス
class MMD_BoneNode{
public:     // 暫定
    MMD_BoneNode(){bone_index = 0;} // root-nodeが使うよ
    ~MMD_BoneNode(){}

    uint16_t   bone_index;      // このノードが示すボーンのインデックス
    std::vector<MMD_BoneNode>   children;
};
// 子供を追加するための関数
void add_children(uint16_t parent, uint16_t child);


// ボーン情報の配列
class MMD_BoneArray{
public: // 暫定
    uint16_t    bone_count;             // ボーン数
    MMD_Bone    *bone_array;            // ボーンリスト(並び順に意味があるので注意)

    std::map<std::string, int>  bone_name_dict;     // ボーン名をキーにして、インデックスを引くための辞書

public:
    MMD_BoneArray();
    ~MMD_BoneArray();

    void read(FILE* fp);

    // デバッグ用機能
    // ボーンのレンダリング
    void draw();
};


// 設計メモ
//      モーションデータはキーフレーム単位, ボーン単位に指定される。
//      モーションデータから、ボーンに対して座標と回転角度が与えらえる
//      ボーンから、頂点に対して、影響度などの情報が与えられる  
//      頂点はただの配列として保持されている。
//

class MMD_File{
public:
    MMD_Header          m_header;
    MMD_VertexArray     m_vertics;
    MMD_face            m_face;
    MaterialArray       m_materials;
    MMD_BoneArray       m_bones;
    std::string         path;

    void setpath(const char* pathname);
    void load(const char* iFilename);

    void draw(void);
};
