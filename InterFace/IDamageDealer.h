#pragma once

// ダメージを返すオブジェクトのインターフェース
class IDamageDealer
{
public:
    virtual ~IDamageDealer() = default;
    virtual int GetDamage() const = 0;
};