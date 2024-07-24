#pragma once
#include "../../sdk/entity/Entity.h"



//�����б�����ҵĿ�������pawn ��ɱ ���� ������ı�
//������Ҳ��������������������Ժ���������ٵ���
struct CachedEntity{
	inline bool Valid() {
		return Controller && Pawn;
	}
	bool UpdatePawn();
	CBaseHandle handle;
	CCSPlayerController* Controller;
	C_CSPlayerPawn* Pawn;
};

class EntityCache
{
public:
	
	void OnAdd(CEntityInstance* inst, CBaseHandle handle);
	void OnRemove(CEntityInstance* inst, CBaseHandle handle);
	std::vector<CachedEntity> players;
	std::vector<CachedEntity> entitys;
private:





};

inline EntityCache* g_EntityCache = new EntityCache;

