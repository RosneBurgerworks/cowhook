/*
 *  Created by Alex
 *  Fixed by OneTF2
 */
/* License: GPLv3 */

#include <common.hpp>
#include <MiscTemporary.hpp>
#include <e8call.hpp>
#include <set>
#include <map>
#include <boost/algorithm/string.hpp>
#include <utility>

static settings::Boolean enable{ "dc.enable", "false" };
static settings::String regions{ "dc.regions", "" };
static settings::Boolean restrict { "dc.restrict", "true" };
static settings::Boolean enable_eu{ "dc.toggle-europe", "false" };
static settings::Boolean enable_north_america{ "dc.toggle-north-america", "false" };
static settings::Boolean enable_south_america{ "dc.toggle-south-america", "false" };
static settings::Boolean enable_asia{ "dc.toggle-asia", "false" };
static settings::Boolean enable_oceania{ "dc.toggle-oceania", "false" };


typedef std::array<char, 5> CidStr_t;

struct SteamNetworkingPOPID_decl
{
    unsigned v;
    void ToString(char *out) const
    {
        out[0] = char(v >> 16);
        out[1] = char(v >> 8);
        out[2] = char(v);
        out[3] = char(v >> 24);
        out[4] = 0;
    }
};

static std::set<CidStr_t> regionsSet;

static void *g_ISteamNetworkingUtils;

static std::unordered_map<std::string, std::string> dc_name_map{
    {"ams", "Amsterdam"},
    {"atl", "Atlanta"},
    {"bom", "Mumbai"},
    {"dxb", "Dubai"},
    {"eat", "Moses Lake/Washington"},
    {"mwh", "Moses Lake/Washington"},
    {"fra", "Frankfurt"},
    {"gnrt", "Tokyo (gnrt)"},
    {"hkg", "Honk Kong"},
    {"iad", "Virginia"},
    {"lax", "Los Angelos"},
    {"lhr", "London"},
    {"lim", "Lima"},
    {"maa", "Chennai"},
    {"mad", "Madrid"},
    {"man", "Manilla"},
    {"par", "Paris"},
    {"scl", "Santiago"},
    {"sea", "Seattle"},
    {"sgp", "Singapore"},
    {"sto", "Stockholm (Kista)"},
    {"sto2", "Stockholm (Bromma)"},
    {"syd", "Sydney"},
    {"tyo", "Tokyo (North)"},
    {"tyo2", "Tokyo (North)"},
    {"tyo1", "Tokyo (South)"},
    {"waw", "Warsaw"}};

static std::vector<std::string> eu_datacenters            = { { "ams" }, { "fra" }, { "lhr" }, { "mad" }, { "par" }, { "sto" }, { "sto2" }, { "waw" }, { "lux" }, { "lux1" }, { "lux2" } };
static std::vector<std::string> north_america_datacenters = { { "atl" }, { "eat" }, { "mwh" }, { "iad" }, { "lax" }, { "sea" } };
static std::vector<std::string> south_america_datacenters = { { "gru" }, { "scl" } };
static std::vector<std::string> asia_datacenters          = { { "bom" }, { "dxb" }, { "gnrt" }, { "hkg" }, { "maa" }, { "man" }, { "sgp" }, { "tyo" }, { "tyo2" }, { "tyo1" } };
static std::vector<std::string> oceana_datacenters        = { { "syd" } };


static void Refresh()
{
    auto gc = (bool *) re::CTFGCClientSystem::GTFGCClientSystem();
    if (!gc)
        return;
    gc[0x374] = true;
}

static CatCommand force_refersh("refresh", "Force refresh of ping data", Refresh);

static void OnRegionsUpdate(std::string regions)
{
    CidStr_t region;
    regionsSet.clear();

    std::vector<std::string> regions_vec;
    boost::split(regions_vec, regions, boost::is_any_of(","));
    for (auto &region_str : regions_vec)
    {
        if (region_str.empty())
            continue;
        if (region_str.length() > 4)
        {
            logging::Info("Ignoring invalid region %s", region_str.c_str());
            continue;
        }
        region.fill(0);
        std::strcpy(region.data(), region_str.c_str());
        regionsSet.emplace(region);
        g_ICvar->ConsoleColorPrintf(MENU_COLOR, "[cowhook]: Froce refresh ping\n");

    }
    if (*enable)
        Refresh();
}

static int (*o_GetDirectPingToPOP)(void *self, SteamNetworkingPOPID_decl cid);
static int h_GetDirectPingToPOP(void *self, SteamNetworkingPOPID_decl cid)
{
    CidStr_t cidStr;

    if (regionsSet.empty())
        return UniformRandomInt(5, 30);

    cid.ToString(cidStr.data());
    auto it = regionsSet.find(cidStr);
    if (it != regionsSet.end())
        return UniformRandomInt(5, 30);

    return *restrict ? UniformRandomInt(500, 800) : o_GetDirectPingToPOP(self, cid);
}

static void Hook(bool on)
{
    static hooks::VMTHook v;

    if (on)
    {
        v.Set(g_ISteamNetworkingUtils);
        v.HookMethod(h_GetDirectPingToPOP, 9, &o_GetDirectPingToPOP);
        v.Apply();
    }
    else
        v.Release();

    Refresh();
}

void manageRegions(const std::vector<std::string> &regions_vec, bool add)
{
    std::set<std::string> regions_split;
    if ((*regions).length())
        boost::split(regions_split, *regions, boost::is_any_of(","));

    std::set<std::string> new_regions = regions_split;

    if (add)
    {
        for (auto &region : regions_vec)
        {
            auto found = regions_split.find(region) != regions_split.end();
            if (!found)
                new_regions.emplace(region);
        }
    }
    else
    {
        for (auto &region : regions_vec)
        {
            auto position = new_regions.find(region);
            if (position != new_regions.end())
                new_regions.erase(position);
        }
    }
    regions = boost::join(new_regions, ",");
}

static void Init()
{
    enable.installChangeCallback(
        [](settings::VariableBase<bool> &, bool on)
        {
            if (!g_ISteamNetworkingUtils)
            {
                g_ISteamNetworkingUtils = ((void *(*) ())(dlsym(sharedobj::libsteamnetworkingsockets().lmap, "SteamNetworkingUtils_LibV4")))();
                if (!g_ISteamNetworkingUtils)
                {
                    logging::Info("DataCenters: Failed to create ISteamNetworkingUtils!");
                    return;
                }
            }
            Hook(on);
        });
    regions.installChangeCallback([](settings::VariableBase<std::string> &, std::string regions) { OnRegionsUpdate(std::move(regions)); });
    restrict.installChangeCallback([](settings::VariableBase<bool> &, bool) { Refresh(); });
    enable_eu.installChangeCallback([](settings::VariableBase<bool> &, bool after) { manageRegions(eu_datacenters, after); });
    enable_north_america.installChangeCallback([](settings::VariableBase<bool> &, bool after) { manageRegions(north_america_datacenters, after); });
    enable_south_america.installChangeCallback([](settings::VariableBase<bool> &, bool after) { manageRegions(south_america_datacenters, after); });
    enable_asia.installChangeCallback([](settings::VariableBase<bool> &, bool after) { manageRegions(asia_datacenters, after); });
    enable_oceania.installChangeCallback([](settings::VariableBase<bool> &, bool after) { manageRegions(oceana_datacenters, after); });
}

static InitRoutine init(Init);