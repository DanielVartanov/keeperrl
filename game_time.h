#pragma once

#include "stdafx.h"
#include "util.h"


class TimeInterval {
  public:
  TimeInterval();
  static TimeInterval fromVisible(int);
  static TimeInterval fromInternal(int);
  bool operator < (TimeInterval) const;
  bool operator > (TimeInterval) const;
  bool operator <= (TimeInterval) const;
  bool operator >= (TimeInterval) const;
  TimeInterval& tryToHalve();
  TimeInterval operator + (TimeInterval) const;
  TimeInterval operator - (TimeInterval) const;
  TimeInterval& operator *= (int);
  TimeInterval operator * (int) const;

  double getDouble() const;
  double getVisibleDouble() const;
  int getVisibleInt() const;
  int getInternal() const;

  template <class Archive>
  void serialize(Archive& ar, const unsigned int);

  int getHash() const;

  private:
  TimeInterval(int);
  template<typename Tag>
  friend class GameTime;
  int SERIAL(cnt) = 0;
};

TimeInterval operator "" _visible(unsigned long long value);

template<typename Tag>
class GameTime {
  public:
  GameTime();
  static GameTime fromVisible(int);
  static GameTime fromInteral(int);

  template <class Archive>
  void serialize(Archive& ar, const unsigned int);

  GameTime& operator += (TimeInterval);
  GameTime& operator -= (TimeInterval);
  bool operator < (GameTime) const;
  bool operator > (GameTime) const;
  bool operator <= (GameTime) const;
  bool operator >= (GameTime) const;
  bool operator == (GameTime) const;
  bool operator != (GameTime) const;
  TimeInterval operator - (GameTime) const;
  GameTime operator - (TimeInterval) const;
  GameTime operator + (TimeInterval) const;

  int getInternal() const;
  double getDouble() const;
  int getVisibleInt() const;

  int getHash() const;

  private:
  explicit GameTime(int);
  int SERIAL(cnt) = 0;
};

template<class T>
std::ostream& operator<<(std::ostream& d, GameTime<T> time);

std::ostream& operator<<(std::ostream& d, TimeInterval time);

using LocalTime = GameTime<struct LocalTimeTag>;
using GlobalTime = GameTime<struct GlobalTimeTag>;

