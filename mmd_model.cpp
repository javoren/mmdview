
#include <GL/gl.h>
#include <GL/glut.h>

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

    face_index = new uint16_t[face_vert_count];
    for(int i = 0; i < face_vert_count; i++){
        fread(&face_index[i], 1, 2, fp);
    }
}

MMD_face::MMD_face(){
}

MMD_face::~MMD_face(){
}


MMD_vertex::MMD_vertex(){
}

MMD_vertex::~MMD_vertex(){
}

void MMD_vertex::read(FILE* fp){
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


MMD_VertexArray::MMD_VertexArray(){
}

MMD_VertexArray::~MMD_VertexArray(){
};

void MMD_VertexArray::read(FILE* fp){
    fread(&count, 4, 1, fp);
    printf("vertex count == %d\n", count);       // デバッグ用

    pVertex = new MMD_vertex[count];
    for(int i = 0;i < count; i++){
        pVertex[i].read(fp);
    }
}


Texture::Texture(void){
    texture_index = 0;
}

Texture::~Texture(){
    if(pBmp)
        glDeleteTextures(1, &texture_index);
};

void Texture::load(const char* filename, unsigned int *tex_index){
    pBmp = NULL;
    FILE* fp = fopen(filename, "rb");
    if(fp != NULL){
        pBmp = BmpIO_Load(fp);
        if(pBmp != NULL){
            // OpenGL用にテクスチャつくる
            glGenTextures(1, tex_index);
            texture_index = *tex_index;
            printf("texture_index = %d\n", texture_index);
            glBindTexture(GL_TEXTURE_2D, texture_index);
            glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
            glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
            glTexImage2D(
                GL_TEXTURE_2D ,0 ,GL_RGB ,pBmp->width ,pBmp->height,
                0 ,GL_RGB ,GL_UNSIGNED_BYTE, pBmp->pColor );
        }else{
            printf(">>>>>>pBmp == NULL\n");
        }
    }
}

unsigned int Texture::get_gl_texture_index(void){
    return this->texture_index;
}


// マテリアルデータを表すクラス
Material::Material(void){
    tex_index = 0;
}
Material::~Material(){}

uint32_t Material::get_face_vert_count(void){
    return face_vert_count;
}

void Material::setpath(std::string pathname){
    path = pathname;
}

void Material::read(FILE* fp){
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
    printf("fullpath_texture = %s\n", fullpath_texture.c_str() );
    texture.load( fullpath_texture.c_str(), &tex_index );
}

// マテリアルの配列
MaterialArray::MaterialArray(){};
MaterialArray::~MaterialArray(){};
void MaterialArray::setpath(std::string pathname){
    path = pathname;
}

void MaterialArray::read(FILE* fp){
    fread(&count, 1, 4, fp);
    mat_array = new Material[count];

    for(int i = 0;i < count; i++){
        mat_array[i].setpath(path);
        mat_array[i].read(fp);
    }
}

void MaterialArray::draw(void){
    uint32_t effect_draw_count = 0;
    for(int i = 0; i < count; i++){
        printf("%d: (%d to %d)\n", i, effect_draw_count, mat_array[i].get_face_vert_count() );
        mat_array[i].draw(effect_draw_count);
        effect_draw_count += mat_array[i].get_face_vert_count();
    }
}


// テクスチャがこのpmdファイルの配下にあるので仕方なく。。
void MMD_File::setpath(const char* pathname){
    path = pathname;
}

// 引数で指定されたファイルを開いて、モデルデータを読み込む
void MMD_File::load(const char* iFilename){
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
    m_materials.draw();
}


void Material::draw(uint32_t start_face)
{
    MMD_face*   face = &mmdfile.m_face;
    int face_count = this->face_vert_count;

    glBindTexture(GL_TEXTURE_2D, texture.get_gl_texture_index() );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

    glFrontFace(GL_CCW);        // GL_CW(時計回りが表), GL_CCW(反時計回りが表)
    glEnable(GL_CULL_FACE);     //カリングON 


    printf("texture_index = %d\n", texture.get_gl_texture_index() );
    for(int i = 0; i < face_count; i+=3 ){
        for(int j = 0; j < 3; j++){
            int index = face->face_index[start_face+i+j];
            MMD_vertex* vert = &mmdfile.m_vertics.pVertex[index];
            // TODO : opengl とdirectXで座標系が異なるので解決しないといけない
            glTexCoord2f( vert->u, fabsf(vert->v - 1.0f) ); // u=x, v=y
            glNormal3f(vert->nx, vert->ny, vert->nz);   // 法線
            glVertex3f(vert->x, vert->y, vert->z);      // 頂点
        }
    }
}

