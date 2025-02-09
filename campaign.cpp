#include "stdafx.h"
#include "campaign.h"
#include "view_id.h"
#include "model.h"
#include "progress_meter.h"
#include "campaign_type.h"
#include "player_role.h"
#include "villain_type.h"
#include "pretty_archive.h"

SERIALIZATION_CONSTRUCTOR_IMPL(Campaign);

SERIALIZE_DEF(Campaign, sites, playerPos, worldName, defeated, influencePos, influenceSize, playerRole, type, mapZoom)

void VillainViewId::serialize(PrettyInputArchive& ar1, unsigned int) {
  if (ar1.peek() == "{" && ar1.peek(2) == "{")
    ar1(ids);
  else {
    ViewId id;
    ar1(id);
    ids.push_back(id);
  }
}

const Table<Campaign::SiteInfo>& Campaign::getSites() const {
  return sites;
}

bool Campaign::canEmbark(Vec2 pos) const {
  switch (playerRole) {
    case PlayerRole::ADVENTURER: return !!sites[pos].dweller;
    case PlayerRole::KEEPER: return !sites[pos].dweller && !sites[pos].blocked;
  }
}

CampaignType Campaign::getType() const {
  return type;
}

bool Campaign::canTravelTo(Vec2 pos) const {
  return (isInInfluence(pos) || playerPos == pos) && !sites[pos].isEmpty();
}

optional<Vec2> Campaign::getPlayerPos() const {
  return playerPos;
}

Campaign::Campaign(Table<SiteInfo> s, CampaignType t, PlayerRole r, const string& w)
    : sites(s), worldName(w), defeated(sites.getBounds(), false), playerRole(r), type(t) {
}

const string& Campaign::getWorldName() const {
  return worldName;
}

void Campaign::clearSite(Vec2 v) {
  sites[v] = SiteInfo{};
  sites[v].viewId = {ViewId("grass")};
}

bool Campaign::isDefeated(Vec2 pos) const {
  return defeated[pos];
}

void Campaign::setDefeated(Vec2 pos) {
  defeated[pos] = true;
  refreshInfluencePos();
}

bool Campaign::VillainInfo::isEnemy() const {
  return type != VillainType::ALLY;
}

string Campaign::VillainInfo::getDescription() const {
  switch (type) {
    case VillainType::ALLY: return "ally";
    case VillainType::MAIN: return "main villain";
    case VillainType::LESSER: return "lesser villain";
    case VillainType::PLAYER: return "player";
    case VillainType::NONE:
      FATAL << "Tried to present villain of type NONE in campaign";
      return "player";
  }
}

optional<Campaign::VillainInfo> Campaign::SiteInfo::getVillain() const {
  if (dweller)
    return dweller->getValueMaybe<VillainInfo>();
  return none;
}

optional<Campaign::KeeperInfo> Campaign::SiteInfo::getKeeper() const {
  if (dweller)
    return dweller->getValueMaybe<KeeperInfo>();
  return none;
}

optional<Campaign::RetiredInfo> Campaign::SiteInfo::getRetired() const {
  if (dweller)
    return dweller->getValueMaybe<RetiredInfo>();
  return none;
}
 
bool Campaign::SiteInfo::isEmpty() const {
  return !dweller;
}

optional<string> Campaign::SiteInfo::getDwellerDescription() const {
  if (dweller)
    return dweller->match(
        [](const VillainInfo& info) { return info.name + " (" + info.getDescription() + ")"; },
        [](const RetiredInfo& info) { return info.gameInfo.name + " (main villain)" ;},
        [](const KeeperInfo&)->string { return "This is your home site"; });
  else
    return none;
}

optional<VillainType> Campaign::SiteInfo::getVillainType() const {
  if (dweller)
    return dweller->match(
        [](const VillainInfo& info) { return info.type; },
        [](const RetiredInfo&) { return VillainType::MAIN; },
        [](const KeeperInfo&) { return VillainType::PLAYER; });
  else
    return none;
}

optional<ViewIdList> Campaign::SiteInfo::getDwellerViewId() const {
  if (dweller)
    return dweller->match(
        [](const VillainInfo& info) { return ViewIdList{{info.viewId.ids}}; },
        [](const RetiredInfo& info) { return info.gameInfo.getViewId(); },
        [](const KeeperInfo& info) { return info.viewId; });
  else
    return none;
}

bool Campaign::SiteInfo::isEnemy() const {
  return getRetired() || (getVillain() && getVillain()->isEnemy());
}

void Campaign::SiteInfo::setBlocked() {
  blocked = true;
  viewId.push_back(Random.choose(ViewId("map_mountain1"), ViewId("map_mountain2"), ViewId("map_mountain3"),
        ViewId("canif_tree"), ViewId("decid_tree")));
}

bool Campaign::isInInfluence(Vec2 pos) const {
  return influencePos.count(pos);
}

void Campaign::refreshInfluencePos() {
  influencePos.clear();
  if (!playerPos)
    return;
  for (double r = 1; r <= sites.getWidth() + sites.getHeight(); r += 0.1) {
    for (Vec2 v : sites.getBounds())
      if ((sites[v].getVillain() || sites[v].getRetired()) && v.distD(*playerPos) <= r)
        influencePos.insert(v);
    int numEnemies = 0;
    for (Vec2 v : influencePos)
      if (sites[v].isEnemy() && !defeated[v])
        ++numEnemies;
    if (numEnemies >= influenceSize)
      break;
  }
}

int Campaign::getNumNonEmpty() const {
  int ret = 0;
  for (Vec2 v : sites.getBounds())
    if (!sites[v].isEmpty())
      ++ret;
  return ret;
}

int Campaign::getMapZoom() const {
  return mapZoom;
}

map<string, string> Campaign::getParameters() const {
  int numMain = 0;
  int numLesser = 0;
  int numAlly = 0;
  int numRetired = 0;
  for (Vec2 v : sites.getBounds())
    if (sites[v].getRetired())
      ++numRetired;
    else if (auto villain = sites[v].getVillain())
      switch (villain->type) {
        case VillainType::ALLY: ++numAlly; break;
        case VillainType::MAIN: ++numMain; break;
        case VillainType::LESSER: ++numLesser; break;
        default: break;
      }
  auto role = EnumInfo<PlayerRole>::getString(playerRole);
  auto gameType = EnumInfo<CampaignType>::getString(type);
  if (playerRole == PlayerRole::ADVENTURER && type == CampaignType::SINGLE_KEEPER) {
    role = "WARLORD";
    gameType = "WARLORD";
  }
  return {
    {"main", toString(numMain)},
    {"lesser", toString(numLesser)},
    {"allies", toString(numAlly)},
    {"retired", toString(numRetired)},
    {"player_role", role},
    {"game_type", gameType},
  };
}

PlayerRole Campaign::getPlayerRole() const {
  return playerRole;
}
