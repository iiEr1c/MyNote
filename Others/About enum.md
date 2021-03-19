# About enum

先看一个例子. 假如有一个游戏场景, 有玩家uid和卡牌uid, 我们提供根据uid查找玩家以及根据玩家uid和卡片uid查找某个卡片. 代码如下:
```cpp
#include <iostream>
#include <memory>

class Player
{
public:
    uint64_t uid() const;
};

class Card
{
public:
    uint64_t unique_id() const;
    uint32_t template_id() const;
};

std::shared_ptr<Player> get_player_by_uid(uint64_t uid);            // [1]
std::shared_ptr<Card> get_card(uint64_t uid, uint64_t unique_id);   // [2]

int main()
{
    return 0;
}
```

在```[1]```和```[2]```处可能会出现卡片uid和玩家uid混用而出现问题的情况, 为了做到编译时就检查出类型安全问题, 我们可以做到以下改进:

```cpp
#include <iostream>
#include <memory>

struct UID
{
    uint64_t value;
    constexpr explicit UID(uint64_t id = 0) noexcept
        : value(id) {}
    constexpr explicit operator bool () const noexcept
    { return this->value != 0; }
};

inline std::ostream& operator<<(std::ostream& os, UID uid)      // [1]
{ return os << uid.value; }

class Player
{
public:
    uint64_t uid() const;
};

class Card
{
public:
    uint64_t unique_id() const;
    uint32_t template_id() const;
};

std::shared_ptr<Player> get_player_by_uid(UID uid)
{
    if(!uid)
        return nullptr;
    // ...
    // 这里要重载<<运算符, 没有与这些操作数匹配的 "<<" 运算符 -- 操作数类型为
    // std::basic_ostream<char, std::char_traits<char>>
    std::cerr<<"get_player_by_uid:"<<uid<<std::endl;
}
std::shared_ptr<Card> get_card(UID uid, uint64_t unique_id);

int main()
{
    return 0;
}
```

首先需要定义一个UID表示玩家uid, 类似地, 根据业务需求还可能要定义CardUID等. 倘若需要打印日志等操作, 还需添加操作符```<<```的重载(如这里的```[1]```), 这样基本解决了类型安全问题, 但是倘若要添加的uid过多, 则需要写大量重复的代码(如这里的```struct UID```和```nline std::ostream& operator<<(...)```), 那么是否还有方案改进呢?

我们可将类型声明为枚举```enum```类型, 做到类型安全, 此外这里的```uid```是```uint64_t```类型, 输出到```std::cerr```中不需要额外的重载.

```cpp
#include <iostream>
#include <memory>

enum UID : uint64_t;
enum CardId : uint64_t;

class Player
{
public:
    uint64_t uid() const;
};

class Card
{
public:
    uint64_t unique_id() const;
    uint32_t template_id() const;
};

std::shared_ptr<Player> get_player_by_uid(UID uid)
{
    if(!uid)
        return nullptr;
    // ...
    // 这里要重载<<运算符, 没有与这些操作数匹配的 "<<" 运算符 -- 操作数类型为
    // std::basic_ostream<char, std::char_traits<char>>
    std::cerr<<"get_player_by_uid:"<<uid<<std::endl;
    return std::make_shared<Player>();
}
std::shared_ptr<Card> get_card(UID uid, uint64_t unique_id);

int main()
{

    auto player = get_player_by_uid(UID(1000));
    return 0;
}
```