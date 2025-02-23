#include "stdafx.h"
#include "item_prefix.h"
#include "item_attributes.h"
#include "lasting_effect.h"
#include "effect.h"
#include "creature_factory.h"
#include "content_factory.h"
#include "creature.h"
#include "creature_attributes.h"
#include "spell_map.h"
#include "game.h"
#include "content_factory.h"
#include "body.h"
#include "item.h"
#include "effect_type.h"

void applyPrefix(const ContentFactory* factory, const ItemPrefix& prefix, ItemAttributes& attr) {
  if (attr.upgradeInfo && attr.upgradeInfo->prefix->contains<ItemPrefixes::AssembledCreatureEffect>()) {
    auto& effect = *attr.upgradeInfo->prefix->getReferenceMaybe<ItemPrefixes::AssembledCreatureEffect>();
    attr.upgradeInfo->prefix = Effect(EffectType::Chain{{effect, Effect(prefix)}});
    attr.prefixes.push_back(getItemName(factory, prefix));
  } else {
    attr.prefixes.push_back(getItemName(factory, prefix));
    prefix.visit<void>(
        [&](LastingOrBuff effect) {
          attr.equipedEffect.push_back(effect);
        },
        [&](const ItemPrefixes::AttackerEffect& e) {
          attr.weaponInfo.attackerEffect.push_back(e.effect);
        },
        [&](const ItemPrefixes::VictimEffect& e) {
          attr.weaponInfo.victimEffect.push_back(e);
        },
        [&](ItemPrefixes::ItemAttrBonus bonus) {
          attr.modifiers[bonus.attr] += bonus.value;
        },
        [&](const ItemPrefixes::JoinPrefixes& join) {
          for (auto& elem : join.prefixes)
            applyPrefix(factory, elem, attr);
        },
        [&](const SpellId& spell) {
          attr.equipedAbility.push_back(spell);
        },
        [&](const SpecialAttr& a) {
          attr.specialAttr[a.attr] = make_pair(a.value, a.predicate);
        },
        [&](const ItemPrefixes::AssembledCreatureEffect& a) {
          if (auto effect = attr.effect->effect->getValueMaybe<Effects::AssembledMinion>()) {
            effect->effects.push_back(a);
            attr.effect = Effect(*effect);
          }
        }
    );
  }
}

void applyPrefixToCreature(const ItemPrefix& prefix, Creature* c) {
  auto applyToIntrinsicAttack = [&] {
    auto& attacks = c->getBody().getIntrinsicAttacks();
    for (auto part : ENUM_ALL(BodyPart))
      if (!attacks[part].empty()) {
        attacks[part].back().item->applyPrefix(prefix, c->getGame()->getContentFactory());
        break;
      }
  };
  prefix.visit<void>(
      [&](LastingOrBuff effect) {
        addPermanentEffect(effect, c);
      },
      [&](const ItemPrefixes::AttackerEffect& e) {
        applyToIntrinsicAttack();
      },
      [&](const ItemPrefixes::VictimEffect& e) {
        applyToIntrinsicAttack();
      },
      [&](ItemPrefixes::ItemAttrBonus e) {
        c->getAttributes().increaseBaseAttr(e.attr, e.value);
      },
      [&](const ItemPrefixes::JoinPrefixes& join) {
        for (auto& elem : join.prefixes)
          applyPrefixToCreature(elem, c);
      },
      [&](const SpellId& spell) {
        c->getSpellMap().add(*c->getGame()->getContentFactory()->getCreatures().getSpell(spell),
            ExperienceType::MELEE, 0);
      },
      [&](const SpecialAttr& a) {
        c->getAttributes().specialAttr[a.attr].push_back(make_pair(a.value, a.predicate));
      },
      [](auto&) {}
  );
}

vector<string> getEffectDescription(const ContentFactory* factory, const ItemPrefix& prefix) {
  return prefix.visit<vector<string>>(
      [&](LastingOrBuff effect) -> vector<string> {
        return {"grants " + getName(effect, factory)};
      },
      [&](const ItemPrefixes::AttackerEffect& e) -> vector<string> {
        return {"attacker affected by: " + e.effect.getName(factory)};
      },
      [&](const ItemPrefixes::VictimEffect& e) -> vector<string> {
        return {"victim affected by: " + e.effect.getName(factory) + " (" + toPercentage(e.chance) + " chance)"};
      },
      [&](ItemPrefixes::ItemAttrBonus bonus) -> vector<string> {
        return {"+"_s + toString(bonus.value) + " " + factory->attrInfo.at(bonus.attr).name};
      },
      [&](const ItemPrefixes::JoinPrefixes& join) -> vector<string> {
        vector<string> ret;
        for (auto& e : join.prefixes)
          ret.append(getEffectDescription(factory, e));
        return ret;
      },
      [&](const SpellId& id) -> vector<string> {
        return {"grants "_s + id.data() + " ability"};
      },
      [&](const SpecialAttr& a) -> vector<string> {
        return {toStringWithSign(a.value) + " " + factory->attrInfo.at(a.attr).name + " " +
            a.predicate.getName(factory)};
      },
      [&](const ItemPrefixes::AssembledCreatureEffect& effect) -> vector<string> {
        return {effect.getDescription(factory)};
      }
  );
}

string getItemName(const ContentFactory* factory, const ItemPrefix& prefix) {
  return prefix.visit<string>(
      [&](LastingOrBuff effect) {
        return "of " + getName(effect, factory);
      },
      [&](const ItemPrefixes::AttackerEffect& e) {
        return "of " + e.effect.getName(factory);
      },
      [&](const ItemPrefixes::VictimEffect& e) {
        return "of " + e.effect.getName(factory);
      },
      [&](ItemPrefixes::ItemAttrBonus bonus) {
        return "of " + factory->attrInfo.at(bonus.attr).name;
      },
      [&](const ItemPrefixes::JoinPrefixes& join) {
        return getItemName(factory, join.prefixes.back());
      },
      [&](const SpellId& id) -> string {
        return "of "_s + factory->getCreatures().getSpell(id)->getName(factory);
      },
      [&](const SpecialAttr& a) -> string {
        return a.predicate.getName(factory);
      },
      [&](const ItemPrefixes::AssembledCreatureEffect& effect) -> string {
        return "with " + effect.getName(factory);
      }
  );
}

string getGlyphName(const ContentFactory* factory, const ItemPrefix& prefix) {
  return prefix.visit<string>(
      [&](const auto&) {
        return ::getItemName(factory, prefix);
      },
      [&](ItemPrefixes::ItemAttrBonus bonus) {
        return "of +"_s + toString(bonus.value) + " " + factory->attrInfo.at(bonus.attr).name;
      },
      [&](const ItemPrefixes::JoinPrefixes& join) {
        return getGlyphName(factory, join.prefixes.back());
      }
  );
}
