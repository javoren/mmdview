
#include <GL/gl.h>
#include <GL/glut.h>

#include <math.h>
#include <string>
#include <stdio.h>
#include <memory.h>


// 外部のライブラリ
// http://www.syuhitu.org/other/bmp/BmpIoLib_h.html
#include "BmpIoLib.h"



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
    uint16_t    *face_index;

    void read(FILE* fp);
    MMD_face();
    virtual ~MMD_face();
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

class MMD_VertexArray{
public:
    uint32_t    count;

    MMD_vertex* pVertex;

    MMD_VertexArray();
    virtual ~MMD_VertexArray();
    void read(FILE* fp);
};


// テクスチャを管理するクラス
class Texture{
private:
    IBMP*   pBmp;
    unsigned int     texture_index;

public:
    Texture(void);
    virtual ~Texture(void);

    void load(const char* filename, unsigned int *tex_index);
    unsigned int get_gl_texture_index(void);
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

    void read(FILE* fp);

    void draw(uint32_t start_face);
};

// マテリアルの配列
class MaterialArray{
private:
    uint32_t    count;
    Material*   mat_array;
    std::string path;

public:
    MaterialArray();
    virtual ~MaterialArray();
    void setpath(std::string pathname);
    void read(FILE* fp);

    void draw(void);
};


class MMD_File{
public:
    MMD_Header          m_header;
    MMD_VertexArray     m_vertics;
    MMD_face            m_face;
    MaterialArray       m_materials;
    std::string         path;

    void setpath(const char* pathname);
    void load(const char* iFilename);

    void draw(void);
};


