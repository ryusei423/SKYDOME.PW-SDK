#pragma once
#include "../../sdk/entity/Entity.h"



//测试中本地玩家的控制器和pawn 自杀 换变 都不会改变
//其他玩家不清楚，暂且这样，如果以后出现问题再调查
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

