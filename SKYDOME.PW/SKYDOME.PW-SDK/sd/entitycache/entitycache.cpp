#include "entitycache.h"

void EntityCache::OnAdd(CEntityInstance* inst, CBaseHandle handle){
    CCSPlayerController* pEntity = (CCSPlayerController*)inst;
    if (!pEntity) return;

    // 只储存联网实体 不要缓存本地玩家
    // https://developer.valvesoftware.com/wiki/Entity_limit#Source_2_limits
    if (handle.GetEntryIndex() >= 16384 || handle.GetEntryIndex() == g_interfaces->EngineClient->GetLocalPlayer()) return;

    if (pEntity->IsBasePlayerController()) {
        LOG(INFO) << "ADD: " << handle.GetEntryIndex();
        players.emplace_back(handle, pEntity,g_interfaces->GameResourceService->pGameEntitySystem->Get<C_CSPlayerPawn>(pEntity->GetPawnHandle()));
    }

    
}

void EntityCache::OnRemove(CEntityInstance* inst, CBaseHandle handle){
    CCSPlayerController* pEntity = (CCSPlayerController*)inst;
    if (!pEntity) return;

    if (handle.GetEntryIndex() >= 16384) return;


    if (pEntity->IsBasePlayerController()) {
        for (int i = 0; i < players.size(); i++) {
            if (players[i].handle == handle){
                LOG(INFO) << "REMOVE: " << handle.GetEntryIndex();
                players.erase(players.begin() + i);
            }

        }
    
    }
    

}

bool CachedEntity::UpdatePawn(){
    this->Pawn = g_interfaces->GameResourceService->pGameEntitySystem->Get<C_CSPlayerPawn>(this->Controller->GetPawnHandle());
    return this->Pawn;
}
