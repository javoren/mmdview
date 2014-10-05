
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
    texture_id = 0;
}

Texture::~Texture(){
    if(pBmp)
        glDeleteTextures(1, &texture_id);
};

void Texture::load(const char* filename, GLuint tex_id){
    pBmp = NULL;
    FILE* fp = fopen(filename, "rb");
    if(fp != NULL){
        pBmp = BmpIO_Load(fp);
        if(pBmp != NULL){
            // OpenGL用にテクスチャつくる
            this->texture_id = tex_id;
            printf("load texture_id = %d\n", this->texture_id);
            glBindTexture(GL_TEXTURE_2D, this->texture_id);
            glTexImage2D(
                GL_TEXTURE_2D ,0 ,GL_RGB ,pBmp->width ,pBmp->height,
                0 ,GL_RGB ,GL_UNSIGNED_BYTE, pBmp->pColor );
            glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
            glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
//          glBindTexture(GL_TEXTURE_2D, 0 );   // いらないかも
        }else{
            printf(">>>>>>pBmp == NULL\n");
        }
        fclose(fp);
    }
}

unsigned int Texture::get_gl_texture_id(void){
    return this->texture_id;
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

void Material::read(FILE* fp, GLuint texture_id){
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
    texture.load( fullpath_texture.c_str(), texture_id );
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

    // テクスチャの上限数がここで確定するので、
    // この位置でテクスチャIDを保持する配列をつくる
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

void MaterialArray::draw(void){
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

//    glBindTexture(GL_TEXTURE_2D, texture.get_gl_texture_id() );
    glBindTexture(GL_TEXTURE_2D, 9 );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

//    glFrontFace(GL_CCW);        // GL_CW(時計回りが表), GL_CCW(反時計回りが表)
//    glEnable(GL_CULL_FACE);     //カリングON 


    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glRotatef(M_PI, 0.0f, 1.0f, 0.0f);

//  glBegin(GL_POLYGON);
    printf("texture_id = %d\n", texture.get_gl_texture_id() );
    for(int i = 0; i < face_count; i+=3 ){
        for(int j = 0; j < 3; j++){
            int index = face->face_index[start_face+i+j];
            MMD_vertex* vert = &mmdfile.m_vertics.pVertex[index];
            // TODO : opengl とdirectXで座標系が異なるので解決しないといけない
//            glTexCoord2f( vert->u, fabsf(vert->v - 1.0f) ); // u=x, v=y

            // バグの原因メモ
            // テクスチャ座標以前の話で、
            // すべてのマテリアルの描画で、同一のテクスチャハンドルを参照している様子
            // 具体的には、md_m_head.bmp ファイルの内容のみがすべてのマテリアルに反映されている。
            // (画像自身をすべて赤で塗りつぶすなどをして確認した。)


            // 正しいテクスチャ座標系の確認用のコード
            // 画像の左下隅のテクスチャのみ描画する。
            // (服で表現すると、正面の画像＋α)
            float v = fabsf(1.0f - vert->v);
//            float v = vert->v;
            float u = vert->u;
//            if(v > 0.1f && u > 0.1f){
                glTexCoord2f( u, v ); // u=x, v=y
//            }else{
//                glTexCoord2f( 0, 0 ); // u=x, v=y
//            }
            // テクスチャ座標系の調査メモ
            // 
            // OpenGLのテクスチャ座標はT,Sで表現される。
            // 画像左下を原点として
            // S ... 右方向が+の座標軸
            // T ... 上方向が+の座標軸
            //
            // なお、glTexCoord2Fは(S,T)で指定する
            //
            // DirectXのテクスチャ座標はu,vで表現される
            // 画像左上を原点として、
            // U ... 右方向が+の座標系
            // V ... 下方向が+の座標系
            // 

            glNormal3f(vert->nx, vert->ny, vert->nz);   // 法線
            glVertex3f(vert->x, vert->y, vert->z);      // 頂点
        }
    }
//    glEnd();
    glBindTexture(GL_TEXTURE_2D, 0 );   // いらないかも
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


