#include"rbhelper.h"

#include "../../sdk/entity/Weapon.h"

#include"../movement/movement.h"

float saturate(float x) {
    return std::clamp(x, 0.0f, 1.0f);
}

float RemapValClamped(float val, float A, float B, float C, float D)
{
    float cVal = (val - A) / (B - A);
    cVal = saturate(cVal);

    return C + (D - C) * cVal;
}

//https://github.com/perilouswithadollarsign/cstrike15_src/blob/f82112a2388b841d72cb62ca48ab1846dfcc11c8/game/shared/cstrike15/weapon_csbase.cpp#L3754
//https://github.com/perilouswithadollarsign/cstrike15_src/blob/f82112a2388b841d72cb62ca48ab1846dfcc11c8/game/shared/cstrike15/weapon_csbase.cpp#L3685
//https://github.com/perilouswithadollarsign/cstrike15_src/blob/f82112a2388b841d72cb62ca48ab1846dfcc11c8/game/shared/cstrike15/weapon_csbase.cpp#L1208

void RageBotHelper::AutomaticStop(C_CSPlayerPawn* pLocal, C_CSWeaponBase* weapon, CUserCmd* cmd){

    if (!pLocal || !weapon)
        return;

    /*if (!pLocal->IsValidMoveType() || !cheat->onground)
        return;

    if (rage_data.rage_hc <= 0.f)
        return;*/

    /*auto movesys = pLocal->m_pMovementServices();
    if (!movesys) {
        return;
    }*/
    auto weapon_data = weapon->datawep();

    if (!weapon_data)
        return;

    if ((weapon->m_zoomLevel() == 0 && weapon_data->m_WeaponType() == WEAPONTYPE_SNIPER_RIFLE))
        return;

    /*if (!m_stop)
        return;*/

    /*if (!rage_data.auto_stop)
        return;*/

    float m_max_speed = weapon->get_max_speed();
    Vector velocity = pLocal->m_vecVelocity();



    velocity.z = 0.f;

    float speed = velocity.Length2D();

    QAngle angle;
    MATH::VectorAngles(velocity, &angle);
    angle.yaw =  g_MovementManager->save_angle.yaw - angle.yaw;

    Vector direction;
    MATH::AngleVectors(angle, &direction);

    Vector stop = direction * speed;

    stop *= -0.015625;

    if (speed > 23.f) {
        cmd->csgoUserCmd.pBaseCmd->flForwardMove = stop.x;
        cmd->csgoUserCmd.pBaseCmd->flSideMove = -stop.y;
    }
    else {
        cmd->csgoUserCmd.pBaseCmd->flForwardMove = 0;
        cmd->csgoUserCmd.pBaseCmd->flSideMove = 0;
    }





}




#include "../esp/esp.h"
bool RageBotHelper::Hitchance(C_CSPlayerPawn* pLocal, C_CSPlayerPawn* ent, C_CSWeaponBase* weapon, QAngle vAimpoint, float hc){

    if (!pLocal)
        return false;

    
    if (!weapon)
        return false;

    auto data = weapon->datawep();
    if (!data)
        return false;

    float hc_MAX = 100.f;
    constexpr int   SEED_MAX = 255;

    Vector     start{ pLocal->GetEyePosition() }, end, fwd, right, up, dir, wep_spread;
    float      inaccuracy, spread;
    
    //are u fucking kiding me
    //Vector point = Vector(vAimpoint.pitch, vAimpoint.yaw, vAimpoint.roll);

    if (hc <= 0) {
        return true;
    }

    //// we cant reach it anymore too far 
    //if (pLocal->GetEyePosition().DistTo(point) > data->m_flRange()) {
    //    return false;
    //}

    size_t     total_hits{ }, needed_hits{ (size_t)std::ceil((hc * SEED_MAX) / hc_MAX) };

    // get needed directional vectors.
    MATH::AngleVectors(vAimpoint, &fwd, &right, &up);

    // store off inaccuracy / spread ( these functions are quite intensive and we only need them once ).
    static float save_inaccuracy = weapon->get_inaccuracy();
    inaccuracy = weapon->get_inaccuracy();

    //对于不准确率引擎预测的贫民窟解决方案
    float delta = std::clamp(inaccuracy - save_inaccuracy,-0.009f, 0.009f);
    inaccuracy = save_inaccuracy + delta;
    save_inaccuracy = inaccuracy;
    spread = weapon->get_spread();

    /*std::cout << "m_flInaccuracyCrouch -> " << *data->m_flInaccuracyCrouch().flValue << "\n";
    std::cout << "m_flInaccuracyStand -> " << *data->m_flInaccuracyStand().flValue << "\n";
    std::cout << "m_flInaccuracyJump -> " << *data->m_flInaccuracyJump().flValue << "\n";
    std::cout << "m_flInaccuracyMove -> " << *data->m_flInaccuracyMove().flValue << "\n";
    std::cout << "m_flCycleTime -> " << *data->m_flCycleTime().flValue << "\n";
    std::cout << inaccuracy << std::endl;*/

    


    // iterate all possible seeds.
    for (int i{ }; i <= SEED_MAX; ++i) {
        
        MATH::fnRandomSeed((i & 255) + 1);
        const auto b = MATH::fnRandomFloat(0.f, 2.0f * 3.14159265358979323846);
        const auto c = MATH::fnRandomFloat(0.0f, 1.0f);
        const auto d = MATH::fnRandomFloat(0.f, 2.0f * 3.14159265358979323846);

        float Inaccuracy = c * inaccuracy;
        float Spread = c * spread;

        const Vector v_spread((cos(b) * Spread) + (cos(d) * Inaccuracy), (sin(b) * Spread) + (sin(d) * Inaccuracy), 0);



        // get spread.
        wep_spread = v_spread * 1.3/*CalculateSpread(weapon, i, inaccuracy, spread)*/;

        // get spread direction.
        dir = fwd + (right * wep_spread.x) + (up * wep_spread.y);

        // get end of trace.
        end = start - (dir * -data->m_flRange());

        trace_filter_t filter = {};
        g_interfaces->Trace->Init(filter, pLocal, MASK_SHOT, 3, 7);

        game_trace_t trace = {};
        ray_t ray = {};

        g_interfaces->Trace->TraceShape(ray, &start, &end, filter, trace);
        g_interfaces->Trace->ClipTraceToPlayers(start, end, &filter, &trace, 0.F, 60.F, (1.F / (start - end).Length()) * (trace.m_end_pos - start).Length());

        if (trace.HitEntity && trace.HitEntity->GetRefEHandle().GetEntryIndex() == ent->GetRefEHandle().GetEntryIndex())
            ++total_hits;

        // we made it.
        if (total_hits >= needed_hits) {
            return true;

        }
        // we cant make it anymore.
        if ((SEED_MAX - i + total_hits) < needed_hits) {
            return false;

        }
    }

    return false;

}
