#pragma once

#include "util.h"
#include "saved_game_info.h"
#include "save_file_info.h"
#include "enemy_id.h"
#include "tribe.h"

class View;
class ProgressMeter;
class Options;
class RetiredGames;

struct CampaignSetup;

struct VillainViewId {
  ViewIdList SERIAL(ids);
  SERIALIZE_ALL(ids)
  void serialize(PrettyInputArchive& ar, unsigned int);
};

class Campaign {
  public:
  struct VillainInfo {
    VillainViewId SERIAL(viewId);
    EnemyId SERIAL(enemyId);
    string SERIAL(name);
    bool SERIAL(alwaysPresent) = false;
    string getDescription() const;
    bool isEnemy() const;
    VillainType SERIAL(type);
    SERIALIZE_ALL(NAMED(viewId), NAMED(enemyId), NAMED(name), NAMED(type), OPTION(alwaysPresent))
  };
  struct KeeperInfo {
    ViewIdList SERIAL(viewId);
    TribeId SERIAL(tribe);
    SERIALIZE_ALL(viewId, tribe)
  };
  struct RetiredInfo {
    SavedGameInfo SERIAL(gameInfo);
    SaveFileInfo SERIAL(fileInfo);
    SERIALIZE_ALL(gameInfo, fileInfo)
  };
  struct SiteInfo {
    vector<ViewId> SERIAL(viewId);
    typedef variant<VillainInfo, RetiredInfo, KeeperInfo> Dweller;
    optional<Dweller> SERIAL(dweller);
    optional<VillainInfo> getVillain() const;
    optional<RetiredInfo> getRetired() const;
    optional<KeeperInfo> getKeeper() const;
    bool isEnemy() const;
    bool isEmpty() const;
    bool SERIAL(blocked) = false;
    void setBlocked();
    optional<ViewIdList> getDwellerViewId() const;
    optional<string> getDwellerDescription() const;
    optional<VillainType> getVillainType() const;
    SERIALIZE_ALL(viewId, dweller, blocked)
  };

  const Table<SiteInfo>& getSites() const;
  void clearSite(Vec2);
  optional<Vec2> getPlayerPos() const;
  const string& getWorldName() const;
  bool isDefeated(Vec2) const;
  void setDefeated(Vec2);
  bool canTravelTo(Vec2) const;
  bool isInInfluence(Vec2) const;
  int getNumNonEmpty() const;
  int getMapZoom() const;
  bool canEmbark(Vec2) const;
  CampaignType getType() const;
  PlayerRole getPlayerRole() const;

  map<string, string> getParameters() const;

  SERIALIZATION_DECL(Campaign)

  private:
  friend class CampaignBuilder;
  void refreshInfluencePos();
  Campaign(Table<SiteInfo>, CampaignType, PlayerRole, const string& worldName);
  Table<SiteInfo> SERIAL(sites);
  optional<Vec2> SERIAL(playerPos);
  string SERIAL(worldName);
  Table<bool> SERIAL(defeated);
  set<Vec2> SERIAL(influencePos);
  int SERIAL(influenceSize);
  PlayerRole SERIAL(playerRole);
  CampaignType SERIAL(type);
  int SERIAL(mapZoom);
};
