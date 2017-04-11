/* Copyright (C) 2013-2014 Michal Brzozowski (rusolis@poczta.fm)

   This file is part of KeeperRL.

   KeeperRL is free software; you can redistribute it and/or modify it under the terms of the
   GNU General Public License as published by the Free Software Foundation; either version 2
   of the License, or (at your option) any later version.

   KeeperRL is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without
   even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License along with this program.
   If not, see http://www.gnu.org/licenses/ . */

#pragma once

#include "util.h"
#include "position.h"

class Creature;
class CreatureView;
class Attack;
class Tribe;
class ViewObject;
class EffectType;

class Trigger : public OwnedObject<Trigger> {
  public:
  virtual optional<ViewObject> getViewObject(WConstCreature viewer) const;
  virtual ~Trigger();

  virtual void onCreatureEnter(WCreature c);
  virtual bool interceptsFlyingItem(WItem it) const;
  virtual void onInterceptFlyingItem(vector<PItem> it, const Attack& a, int remainingDist, Vec2 dir, VisionId);

  virtual void tick();
  virtual void fireDamage(double size);

  static PTrigger getMeteorShower(WCreature, double duration);

  static const ViewObject& getTorchViewObject(Dir);

  SERIALIZATION_DECL(Trigger);

  protected:
  Trigger(Position);
  Trigger(const ViewObject& obj, Position);

  unique_ptr<ViewObject> SERIAL(viewObject);
  Position SERIAL(position);
};

