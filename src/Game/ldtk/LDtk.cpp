#include "pch.h"
#include "ldtk/LDtk.h"
#include "level/Level.h"
#include "graphics/texture/Tileset.h"
#include "graphics/texture/TilesetManager.h"


NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(IntGridValueDef, value, identifier);


// Static members

json LDtk::ourData;
std::unordered_map<unsigned, Tileset*> LDtk::ourTilesets;
std::unordered_map<std::string, EntityDef> LDtk::ourEntityDefs;
std::unordered_map<std::string, float> LDtk::ourLayerDepths;
std::unordered_map<size_t, LevelDef> LDtk::ourLevelDefs;
std::unordered_map<std::string, IntGridLayerDef> LDtk::ourIntGridLayerDefs;


// Public static methods

bool LDtk::LoadProjectFile(const fs::path& aProjectFile, TilesetManager& aTilesetMgr)
{
    assert(aProjectFile.extension() == ".ldtk");

    std::ifstream i(aProjectFile);
    if (!i.is_open())
        return false;
    i >> ourData;

    assert(ourData["simplifiedExport"].get<bool>());

    // Import tilesets
    for (const auto& tileset : ourData["defs"]["tilesets"])
    {
        if (tileset["relPath"].is_null())
            continue;

        fs::path path = aProjectFile.parent_path() / tileset["relPath"].get<std::string>();

        path = path.lexically_normal();

        ourTilesets[tileset["uid"].get<unsigned>()] = aTilesetMgr.GetTileset(path);
    }

    // Import entity defs
    for (const auto& entity : ourData["defs"]["entities"])
    {
        const std::string id = entity["identifier"].get<std::string>();

        EntityDef& entityDef = ourEntityDefs[id];
        entityDef.id = id;

        for (const std::string& tag : entity["tags"])
            entityDef.tags.emplace_back(tag);

        entity["width"].get_to(entityDef.size.x);
        entity["height"].get_to(entityDef.size.y);

        entity["pivotX"].get_to(entityDef.pivot.x);
        entity["pivotY"].get_to(entityDef.pivot.y);

        if (!entity["tilesetId"].is_null())
            entityDef.tileset = ourTilesets[entity["tilesetId"].get<unsigned>()];

        if (entityDef.tileset)
        {
            const json& rect = entity["tileRect"];

            entityDef.tile.min = { rect["x"].get<float>(), rect["y"].get<float>() };
            entityDef.tile.max = { rect["w"].get<float>(), rect["h"].get<float>() };
            entityDef.tile.max += entityDef.tile.min;
        }

        for (const auto& fieldDef : entity["fieldDefs"])
        {
            if (fieldDef["defaultOverride"].is_null())
                continue;

            const std::string& id = fieldDef["identifier"];
            const std::string& type = fieldDef["__type"];
            const json& defaultValue = fieldDef["defaultOverride"]["params"][0];

            if (type == "Int")
                entityDef.defaultFieldDefs.emplace(id, defaultValue.get<int>());
            else if (type == "Float")
                entityDef.defaultFieldDefs.emplace(id, defaultValue.get<float>());
            else if (type == "Bool")
                entityDef.defaultFieldDefs.emplace(id, defaultValue.get<bool>());
            else if (type == "String" || type == "Multilines")
                entityDef.defaultFieldDefs.emplace(id, defaultValue.get<std::string>());
        }
    }

    // Compute layer depths
    {
        float depth = 0.f;
        for (const auto& layer : ourData["defs"]["layers"])
        {
            ourLayerDepths.emplace(layer["identifier"].get<std::string>(), depth);
            depth += 1.f;
        }
    }

    // Import IntGrid layer defs
    for (const auto& layer : ourData["defs"]["layers"])
    {
        if (layer["type"].get<std::string>() != "IntGrid")
            continue;

        IntGridLayerDef& def = ourIntGridLayerDefs[layer["identifier"].get<std::string>()];

        def.tileset = ourTilesets[layer["tilesetDefUid"].get<unsigned>()];
        layer["intGridValues"].get_to(def.values);
    }

    // Import level defs
    for (const auto& level : ourData["levels"])
    {
        LevelDef def = {};

        for (const auto& neighbour : level["__neighbours"])
        {
            size_t iid = std::hash<std::string>{}(neighbour["levelIid"]);
            Direction dir;

            switch (neighbour["dir"].get<std::string>().front())
            {
            case 'n':
                dir = Direction::Up;
                break;
            case 's':
                dir = Direction::Down;
                break;
            case 'e':
                dir = Direction::Right;
                break;
            case 'w':
                dir = Direction::Left;
                break;
            default:
                assert(false);
            }

            def.neighbours.emplace_back(iid, dir);
        }

        ourLevelDefs[std::hash<std::string>{}(level["iid"])] = def;
    }

    return true;
}


// Getters

float LDtk::GetLayerDepth(const std::string& anIdentifier)
{
    return ourLayerDepths.at(anIdentifier);
}

const EntityDef& LDtk::GetEntityDef(const std::string& anIdentifier)
{
    return ourEntityDefs.at(anIdentifier);
}

const LevelDef& LDtk::GetLevelDef(size_t iid)
{
    return ourLevelDefs.at(iid);
}

const IntGridLayerDef LDtk::GetIntGridLayerDef(const std::string& anIdentifier)
{
    return ourIntGridLayerDefs.at(anIdentifier);
}
