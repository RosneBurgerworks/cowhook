/* VERY Simple taunt ""fix"" */
#include "common.hpp"

namespace hacks::antitaunts
{

#if ENABLE_TEXTMODE
static settings::Boolean remove_taunts{ "remove.taunts", "true" };
#else
static settings::Boolean remove_taunts{ "remove.taunts", "false" };
#endif

void CreateMove()
{
    for (int i = 1; i < g_GlobalVars->maxClients; ++i)
    {
        auto ent = ENTITY(i);
        if (CE_BAD(ent) || ent == LOCAL_E || ent->m_Type() != ENTITY_PLAYER)
            continue;
        RemoveCondition<TFCond_Taunting>(ent);
    }
}

static InitRoutine EC(
    []()
    {
        EC::Register(EC::CreateMove, CreateMove, "CM_AntiTaunt");
        EC::Register(EC::CreateMoveWarp, CreateMove, "CMW_AntiTaunt");
    });
    
} // namespace hacks::antitaunts