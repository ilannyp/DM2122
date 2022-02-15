#include "Player.h"

Player::Player()
{
	currency = 100;
	alive = true;
	gold = 0;
}

Player::~Player()
{
}

int Player::get_currency()
{
	return currency;
}

void Player::set_currency(int x)
{
	currency = x;
}

void Player::currency_deducted(int x)
{
	currency = currency - x;
}

void Player::currency_added(int x)
{
	currency = currency + x;
}

void Player::set_alive()
{
	alive = true;
}

void Player::set_die()
{
	alive = false;
}

bool Player::get_life_status()
{
	return alive;
}

int Player::get_gold()
{
	return gold;
}

void Player::set_gold(int x)
{
	gold = x;
}

void Player::gold_deducted(int x)
{
	gold = gold - x;
}

void Player::gold_added(int x)
{
	gold = gold + x;
}
