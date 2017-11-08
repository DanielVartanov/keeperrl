#include "game_time.h"

template<typename Tag>
GameTime<Tag>::GameTime() {
}

template<typename Tag>
GameTime<Tag> GameTime<Tag>::fromInteral(int i) {
  return GameTime<Tag>(i);
}

template<typename Tag>
GameTime<Tag> GameTime<Tag>::fromVisible(int i) {
  return GameTime<Tag>(i * 2);
}

template<typename Tag>
GameTime<Tag>::GameTime(int i) : cnt(i) {
}

template<typename Tag>
GameTime<Tag>& GameTime<Tag>::operator +=(TimeInterval i) {
  cnt += i.cnt;
  return *this;
}

template<typename Tag>
GameTime<Tag>& GameTime<Tag>::operator -= (TimeInterval i) {
  cnt -= i.cnt;
  return *this;
}

template<typename Tag>
bool GameTime<Tag>::operator < (GameTime<Tag> o) const {
  return cnt < o.cnt;
}

template<typename Tag>
bool GameTime<Tag>::operator > (GameTime<Tag> o) const {
  return cnt > o.cnt;
}

template<typename Tag>
bool GameTime<Tag>::operator <= (GameTime<Tag> o) const {
  return cnt <= o.cnt;
}

template<typename Tag>
bool GameTime<Tag>::operator >= (GameTime<Tag> o) const {
  return cnt >= o.cnt;
}

template<typename Tag>
bool GameTime<Tag>::operator == (GameTime<Tag> o) const {
  return cnt == o.cnt;
}

template<typename Tag>
bool GameTime<Tag>::operator != (GameTime<Tag> o) const {
  return cnt != o.cnt;
}

template<typename Tag>
TimeInterval GameTime<Tag>::operator - (GameTime<Tag> o) const {
  return TimeInterval(cnt - o.cnt);
}

template<typename Tag>
GameTime<Tag> GameTime<Tag>::operator - (TimeInterval i) const {
  return GameTime<Tag>(cnt - i.cnt);
}

template<typename Tag>
GameTime<Tag> GameTime<Tag>::operator + (TimeInterval i) const {
  return GameTime<Tag>(cnt + i.cnt);
}

template<typename Tag>
int GameTime<Tag>::getInternal() const {
  return cnt;
}

template<typename Tag>
double GameTime<Tag>::getDouble() const {
  return (double) cnt;
}

template<typename Tag>
int GameTime<Tag>::getVisibleInt() const {
  return cnt / 2;
}

template<typename Tag>
int GameTime<Tag>::getHash() const {
  return cnt;
}

template <typename Tag>
template <class Archive>
void GameTime<Tag>::serialize(Archive& ar, const unsigned int version) {
  ar(cnt);
}

TimeInterval::TimeInterval() {}

TimeInterval TimeInterval::fromVisible(int i) {
  return TimeInterval(i * 2);
}

TimeInterval TimeInterval::fromInternal(int i) {
  return TimeInterval(i);
}

bool TimeInterval::operator < (TimeInterval i) const {
  return cnt < i.cnt;
}

bool TimeInterval::operator > (TimeInterval i) const {
  return cnt > i.cnt;
}

bool TimeInterval::operator <=(TimeInterval i) const {
  return cnt <= i.cnt;
}

bool TimeInterval::operator >=(TimeInterval i) const {
  return cnt >= i.cnt;
}

TimeInterval& TimeInterval::tryToHalve() {
  cnt /= 2;
  if (cnt == 0)
    cnt = 1;
  return *this;
}

TimeInterval TimeInterval::operator -(TimeInterval i) const {
  return TimeInterval(cnt - i.cnt);
}

TimeInterval TimeInterval::operator +(TimeInterval i) const {
  return TimeInterval(cnt + i.cnt);
}

double TimeInterval::getDouble() const {
  return cnt;
}

double TimeInterval::getVisibleDouble() const {
  return double(cnt) / 2;
}

int TimeInterval::getVisibleInt() const {
  return cnt / 2;
}

int TimeInterval::getInternal() const {
  return cnt;
}

TimeInterval& TimeInterval::operator *=(int i) {
  cnt *= i;
  return *this;
}

TimeInterval TimeInterval::operator*(int i) const {
  return TimeInterval(cnt * i);
}

SERIALIZE_DEF(TimeInterval, cnt)

int TimeInterval::getHash() const {
  return cnt;
}

TimeInterval::TimeInterval(int i) : cnt(i) {}

template<class T>
std::ostream& operator<<(std::ostream& d, GameTime<T> time) {
  d << time.getInternal() / 2;
  if (time.getInternal() % 2 == 1)
    d << ".5";
  return d;
}

template
std::ostream& operator<<(std::ostream& d, GameTime<LocalTimeTag> time);

template
std::ostream& operator<<(std::ostream& d, GameTime<GlobalTimeTag> time);

std::ostream& operator<<(std::ostream& d, TimeInterval i) {
  d << i.getInternal() / 2;
  if (i.getInternal() % 2 == 1)
    d << ".5";
  return d;
}

SERIALIZABLE_TMPL(GameTime, LocalTimeTag);
SERIALIZABLE_TMPL(GameTime, GlobalTimeTag);


TimeInterval operator "" _visible(unsigned long long value) {
  return TimeInterval::fromVisible((int) value);
}
