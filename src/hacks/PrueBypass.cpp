// Copyrigt Cow & Co 2023
// ** Made by OneTF2
#include <Bool.hpp>
#include "common.hpp"

namespace hacks::purebypass
{

hooks::VMTHook svpurehook{};
static void (*orig_RegisterFileWhilelist)(void *, void *, void *);

static void RegisterFileWhitelist(void *, void *, void *)
{
}

static InitRoutine init(
    []
    {
        svpurehook.Set(g_IFileSystem);
        svpurehook.HookMethod(RegisterFileWhitelist, offsets::RegisterFileWhitelist(), &orig_RegisterFileWhilelist);
        svpurehook.Apply();
    });

} // namespace hacks::purebypass
