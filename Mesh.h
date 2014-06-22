#pragma once

#include <vector>

// 3Dベクトルの定義
typedef struct MMD3DVECTOR {
    float x;
    float y;
    float z;
} MMD3DVECTOR;

// 2Dベクトルの定義
typedef struct MMD2DXVECTOR {
    float x;
    float y;
} MMD2DXVECTOR;

/// メッシュの頂点データ
struct Vertex {
	MMD3DVECTOR position;	// 頂点位置
	MMD3DVECTOR normal;		// 法線ベクトル
	MMD2DXVECTOR texture;	// テクスチャ座標
};

/// メッシュのポリゴンデータ
struct Face {
	unsigned short indices[3];		// 3頂点のインデックス
	unsigned long material_number;	// 材料番号
};

// メッシュデータ一時保存用構造体。独自形式データは一度この構造体に格納し、Mesh::SetMesh()でMesh::pMeshにセットする。
struct MeshData {
	std::vector<Vertex> vertices;
	std::vector<Face> faces;
//	std::vector<D3DMATERIAL9> material;
//	std::vector<string> texture_filename;
};

/// メッシュのベース
class Mesh {
protected:
//	LPDIRECT3DDEVICE9 pDevice;			// Direct3Dデバイスオブジェクト
//	LPD3DXMESH pMesh;					// メッシュ
//	D3DMATERIAL9* pMeshMaterials;		// マテリアル配列
//	LPDIRECT3DTEXTURE9*	pMeshTextures;	// テクスチャ配列
//	DWORD dwNumMaterials;				// マテリアル・テクスチャ配列の大きさ
	void AddNormalVector(MeshData& meshData);// MeshDataに法線ベクトルを追加
	void SetMesh(MeshData meshData);	// MeshDataをpMeshにセット
public:
//	Mesh(LPDIRECT3DDEVICE9 pDevice);
//	virtual void Draw(MMD3DVECTOR position, D3DXMATRIX rotation);
//	virtual LPD3DXMESH GetMesh();
//	virtual int GetNumMaterial();

	Mesh(void);
	virtual ~Mesh();
//	virtual void Draw(MMD3DVECTOR position, D3DXMATRIX rotation);
//	virtual LPD3DXMESH GetMesh();
//	virtual int GetNumMaterial();
};

#if 0
/// メッシュのビュー変換、射影変換を行なうカメラ
class MeshCamera {
private:
	LPDIRECT3DDEVICE9 pDevice;
public:
	MeshCamera(LPDIRECT3DDEVICE9 pDev);
	void Look(MMD3DVECTOR eyePoint, MMD3DVECTOR lookAtPoint);
};
#endif

#if 0
/// メッシュ用ライト
class MeshLight sealed {
private:
	LPDIRECT3DDEVICE9 pDevice;
	D3DLIGHT9 light;
public:
	MeshLight(LPDIRECT3DDEVICE9 pDev);
	void Illume(MMD3DVECTOR direction);
};
#endif


/// PMDファイルから読込んだメッシュ
class PmdMesh : public Mesh {
/// PMD構造体定義
#pragma pack(push,1)	//アラインメント制御をオフ
	struct PmdHeader {
		unsigned char magic[3];
		float version;
		unsigned char model_name[20];
		unsigned char comment[256];
	} pmdHeader;
	struct PmdVertex{
		float pos[3];
		float normal_vec[3];
		float uv[2];
		unsigned short bone_num[2];
		unsigned char bone_weight;
		unsigned char edge_flag;
	};
	struct PmdMaterial{
		float diffuse_color[3];
		float alpha;
		float specularity;
		float specular_color[3];
		float mirror_color[3];
		unsigned char toon_index;
		unsigned char edge_flag;
		unsigned long face_vert_count;	// この材料の面頂点数 → 材料番号iのポリゴン番号： pmdMaterial[i - 1].face_vert_count/3 〜 pmdMaterial[i].face_vert_count/3 - 1
		char texture_file_name[20];
	};
#pragma pack(pop)
//	void CopyMaterial(D3DMATERIAL9& material, PmdMaterial& pmdMaterial);	// PmdMaterialからD3DMATERIAL9にデータをコピー
public:
//	PmdMesh(LPCTSTR filename, LPDIRECT3DDEVICE9 pDevice);
	PmdMesh(LPCTSTR filename);
};
