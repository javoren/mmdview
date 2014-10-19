
#include <GL/gl.h>
#include <GL/glut.h>
#include <GL/glx.h>
#define GL_GLEXT_PROTOTYPES     
#include <GL/glext.h>

#include <math.h>
#include <string>
#include <stdio.h>
#include <memory.h>


// 外部のライブラリ
// http://www.syuhitu.org/other/bmp/BmpIoLib_h.html
#include "BmpIoLib.h"

#include "mmd_model.h"


// TODO : 整理する。これは殴り書きしたときの残骸
// クラス構造が整理できたら消えるはず
extern MMD_File    mmdfile;
extern Texture     madoka_magic;


void MMD_Header::read(FILE* fp){
    // アライメントがそろっていないので、自前で読み込む
    fread(magic, 1, 3, fp);
    fread(&version, 1, 4, fp);
    fread(model_name, 1, 20, fp);
    fread(comment, 1, 256, fp);
}

MMD_Header::MMD_Header(){
    memset(magic, 3, 0);
    version = 0.0f;
    memset(model_name, 0, 20);
    memset(comment, 0, 256);
}
MMD_Header::~MMD_Header(){}


void MMD_face::read(FILE* fp){
    fread(&face_vert_count, 1, 4, fp);
    printf("face vert count = %d\n", face_vert_count);

    face_index = new GLuint[face_vert_count];
    for(int i = 0; i < face_vert_count; i++){
        fread(&face_index[i], 1, 2, fp);
    }
}

MMD_face::MMD_face(){
}

MMD_face::~MMD_face(){
}

void MMD_face::draw()
{
}

MMD_vertex::MMD_vertex(){
}

MMD_vertex::~MMD_vertex(){
}

void MMD_vertex::read(FILE* fp)
{
    fread(&x, 1, 4, fp);
    fread(&y, 1, 4, fp);
    fread(&z, 1, 4, fp);
    fread(&nx, 1, 4, fp);
    fread(&ny, 1, 4, fp);
    fread(&nz, 1, 4, fp);
    fread(&u, 1, 4, fp);
    fread(&v, 1, 4, fp);

    fread(bone_num, 2, 2, fp);
    fread(&bone_weight, 1, 1, fp);
    fread(&edge_flag, 1, 1, fp);
}


MMD_VertexArray::MMD_VertexArray()
{
    p3dVerted=NULL;
    p3dNormal=NULL;
    pTexuv=NULL;
}

MMD_VertexArray::~MMD_VertexArray()
{
    if(p3dVerted){
        delete []p3dVerted;
        p3dVerted = NULL;
    }
    if(p3dNormal){
        delete []p3dNormal;
        p3dNormal = NULL;
    }
    if(pTexuv){
        delete []pTexuv;
        pTexuv = NULL;
    }
};

void MMD_VertexArray::read(FILE* fp)
{
    fread(&count, 4, 1, fp);
    printf("vertex count == %d\n", count);       // デバッグ用

    pVertex = new MMD_vertex[count];
    for(int i = 0;i < count; i++){
        pVertex[i].read(fp);
    }
}

void MMD_VertexArray::draw()
{
    // 頂点, 法線, テクスチャ座標ごとの配列に置き換える
    p3dVerted = new GLfloat[count*3]; 
    p3dNormal = new GLfloat[count*3]; 
    pTexuv = new GLfloat[count*2];
    for(int i = 0; i < count; i++ ){
        p3dVerted[i*3 + 0] = pVertex[i].x;
        p3dVerted[i*3 + 1] = pVertex[i].y;
        p3dVerted[i*3 + 2] = pVertex[i].z;
        p3dNormal[i*3 + 0] = pVertex[i].nx;
        p3dNormal[i*3 + 1] = pVertex[i].ny;
        p3dNormal[i*3 + 2] = pVertex[i].nz;
        pTexuv[i*2+0] = pVertex[i].u;
        pTexuv[i*2+1] = fabsf(1.0f - pVertex[i].v);
    }
    glVertexPointer(3, GL_FLOAT, 0, p3dVerted);     // 頂点配列 (0はストライド)
    glNormalPointer(GL_FLOAT, 0, p3dNormal);        // 法線配列
    glTexCoordPointer(2, GL_FLOAT, 0, pTexuv);      // テクスチャ座標
    printf("%s : vert array draw \n", __FUNCTION__);
    printf("%s : vertex count = %d \n", __FUNCTION__, count);
}

Texture::Texture(void)
{
}

Texture::~Texture()
{
    if(pBmp)
        glDeleteTextures(1, &texture_id);
};

void Texture::load(const char* filename, GLuint tex_id)
{
    pBmp = NULL;
    FILE* fp = fopen(filename, "rb");
    if(fp != NULL){
        pBmp = BmpIO_Load(fp);
        if(pBmp != NULL){
            // OpenGL用にテクスチャつくる
            this->texture_id = tex_id;
            printf("load texture_id = %d\n", this->texture_id);
            printf("id: %d = %s\n", this->texture_id, filename);
            glBindTexture(GL_TEXTURE_2D, this->texture_id);
            glTexImage2D(
                GL_TEXTURE_2D ,0 ,GL_RGB ,pBmp->width ,pBmp->height,
                0 ,GL_RGB ,GL_UNSIGNED_BYTE, pBmp->pColor );
            glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
            glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
            glFlush();
            glBindTexture(GL_TEXTURE_2D, 0 );
//            glBindTexture( GL_TEXTURE_2D, 10 );   // いらないかも
        }else{
            printf(">>>>>>pBmp == NULL\n");
        }
        fclose(fp);
    }
}

unsigned int Texture::get_gl_texture_id(void)
{
    return this->texture_id;
}


// マテリアルデータを表すクラス
Material::Material(void)
{
    tex_index = 0;
}
Material::~Material(){}

uint32_t Material::get_face_vert_count(void)
{
    return face_vert_count;
}

void Material::setpath(std::string pathname)
{
    path = pathname;
}

void Material::read(FILE* fp, GLuint texture_id)
{
    memset(texture_name, 0, 21);
    fread(diffuse_color, 4, 3, fp);
    fread(&alpha, 4, 1, fp);
    fread(&specular, 4, 1, fp);
    fread(specular_color, 4, 3, fp);
    fread(mirror_color, 4, 3, fp);
    fread(&toon_index, 1, 1, fp);
    fread(&edge_flag, 1, 1, fp);
    fread(&face_vert_count, 4, 1, fp);
    fread(texture_name, 1, 20, fp);
    std::string fullpath_texture = path + texture_name;
    texture.load( fullpath_texture.c_str(), texture_id );
}

// マテリアルの配列
MaterialArray::MaterialArray(){};
MaterialArray::~MaterialArray(){};
void MaterialArray::setpath(std::string pathname)
{
    path = pathname;
}

void MaterialArray::read(FILE* fp)
{
    fread(&count, 1, 4, fp);
    mat_array = new Material[count];

    // テクスチャの上限数がここで確定するので、
    // この位置でテクスチャIDを保持する配列をつくる
    // TODO: テクスチャを使わないマテリアルの場合は無駄が出てしまうので、対応したい
    textureIds = new GLuint[count];
    glGenTextures(count, textureIds);
    printf("glGenTextures Array = %d\n", count);
    for(int i = 0;i < count; i++){
        printf("[index:%d == id=%d]\n", i, textureIds[i] );
    }

    // テクスチャを一枚ずつ読み込んでいく
    // TODO: 異なるマテリアルでも同じファイル名を指していることもあるので、
    // マテリアルとテクスチャを1:1の関係では効率悪そう。
    for(int i = 0;i < count; i++){
        mat_array[i].setpath(path);
        mat_array[i].read(fp, textureIds[i] );
    }
}


void MaterialArray::draw(void)
{
    uint32_t effect_draw_count = 0;
    for(int i = 0; i < count; i++){
        mat_array[i].draw(effect_draw_count);
        effect_draw_count += mat_array[i].get_face_vert_count();
    }
}


void Material::draw(uint32_t start_face)
{
    MMD_face*   face = &mmdfile.m_face;
    int face_count = this->face_vert_count;

    glBindTexture(GL_TEXTURE_2D, texture.get_gl_texture_id() );
    printf("texture_id = %d\n", texture.get_gl_texture_id() );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

    glFrontFace(GL_CCW);        // GL_CW(時計回りが表), GL_CCW(反時計回りが表)
    glEnable(GL_CULL_FACE);     //カリングOFF

    GLsizei primitive_count = face_count;
    printf("primitive_count=%d\n", primitive_count);
    printf("start_face=%d\n", start_face);

    glDrawElements(GL_TRIANGLES,                    // プリミティブの種類
                    primitive_count,                // レンダリング要素の数
                    GL_UNSIGNED_INT,                // インデックス配列の型
                    &face->face_index[start_face]); // インデックス配列をさすポインタ

    glFlush();
}


// テクスチャがこのpmdファイルの配下にあるので仕方なく。。
void MMD_File::setpath(const char* pathname)
{
    path = pathname;
}

// 引数で指定されたファイルを開いて、モデルデータを読み込む
void MMD_File::load(const char* iFilename)
{
    std::string fullpath = path + iFilename;
    FILE* fp = fopen(fullpath.c_str(), "rb");

    m_header.read(fp);
    m_vertics.read(fp);
    m_face.read(fp);
    m_materials.setpath(path);
    m_materials.read(fp);

    fclose(fp);
};

void MMD_File::draw(void){
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    m_vertics.draw();
    m_materials.draw();
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}


