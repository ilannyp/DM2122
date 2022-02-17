#include "Player.h"

Player::Player()
{
	currency = 100;
	alive = true;
	in_cab = false;
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

void Player::set_in_cab(bool x)
{
	if (x == true)
	{
		in_cab = true;
	}
	else
	{
		in_cab = false;
	}
}

bool Player::get_in_cab()
{
	return in_cab;
}
