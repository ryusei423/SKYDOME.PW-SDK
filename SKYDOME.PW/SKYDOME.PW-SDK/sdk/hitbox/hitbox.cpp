#include "hitbox.h"
#include "../../OffsetManager.h"

uint32_t CModel::GetHitboxFlags(uint32_t index){
    using fnHitboxFlags = uint32_t(__fastcall*)(void*, uint32_t);
    static auto HitboxFlags = reinterpret_cast<fnHitboxFlags>(g_OffsetManager->offsets[g_OffsetManager->OFFSET_GET_HITBOX_FLAGS]);
    SD_ASSERT(HitboxFlags != nullptr);

    return HitboxFlags(this, index);
}

const char* CModel::GetHitboxName(uint32_t index)
{
    using fnHitboxName = const char* (__fastcall*)(void*, uint32_t);
    static auto HitboxName = reinterpret_cast<fnHitboxName>(g_OffsetManager->offsets[g_OffsetManager->OFFSET_GET_HITBOX_NAME]);
    SD_ASSERT(HitboxName != nullptr);

    return HitboxName(this, index);
}

uint32_t CModel::GetHitboxParent(uint32_t index){
    using fnHitboxParent = uint32_t(__fastcall*)(void*, uint32_t);
    static auto HitboxParent = reinterpret_cast<fnHitboxParent>(g_OffsetManager->offsets[g_OffsetManager->OFFSET_GET_HITBOX_PARENT]);
    SD_ASSERT(HitboxParent != nullptr);
    return HitboxParent(this, index);
}

uint32_t CModel::GetHitboxesNum()
{
    using fnHitboxNum = uint32_t(__fastcall*)(void*);
    static auto HitboxNum = reinterpret_cast<fnHitboxNum>(g_OffsetManager->offsets[g_OffsetManager->OFFSET_GET_HITBOXS_NUM]);
    SD_ASSERT(HitboxNum != nullptr);

    return HitboxNum(this);
}
