#include "Player.h"

Player::Player()
{
	currency = 10000;
	alive = true;
	in_cab = false;
	first_scammed = false;
	coin1_enabled, coin1_obtained, coin2_enabled, coin2_obtained, coin3_enabled, coin3_obtained = false;
	win = false;
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

void Player::set_first_scammed(bool x)
{
	if (x == true)
	{
		first_scammed = true;
	}
	else
	{
		first_scammed = false;
	}
}

bool Player::get_first_scammed()
{
	return first_scammed;
}

bool Player::get_coin1_obtained()
{
	return coin1_obtained;
}

bool Player::get_coin2_obtained()
{
	return coin2_obtained;
}

bool Player::get_coin3_obtained()
{
	return coin3_obtained;
}

void Player::set_coin1_obtained(bool x)
{
	coin1_obtained = x;
}

void Player::set_coin2_obtained(bool x)
{
	coin2_obtained = x;

}

void Player::set_coin3_obtained(bool x)
{
	coin3_obtained = x;

}

bool Player::get_coin1_enabled()
{
	return coin1_enabled;
}

bool Player::get_coin2_enabled()
{
	return coin2_enabled;
}

bool Player::get_coin3_enabled()
{
	return coin3_enabled;
}

void Player::set_coin1_enabled(bool x)
{

	coin1_enabled = x;

}

void Player::set_coin2_enabled(bool x)
{

	coin2_enabled = x;
}

void Player::set_coin3_enabled(bool x)
{

	coin3_enabled = x;
}

bool Player::get_currency_added_from_C1()
{
	return currency_added_from_C1;
}

bool Player::get_currency_added_from_C2()
{
	return currency_added_from_C2;
}

bool Player::get_currency_added_from_C3()
{
	return currency_added_from_C3;
}

void Player::set_currency_added_from_C1(bool x)
{

	currency_added_from_C1 = x;
}

void Player::set_currency_added_from_C2(bool x)
{
	currency_added_from_C2 = x;

}

void Player::set_currency_added_from_C3(bool x)
{

	currency_added_from_C3 = x;
}

void Player::set_win(bool x)
{
	win = x;
}

bool Player::get_win()
{
	return win;
}

void Player::total_reset()
{
	currency = 1;
	alive = true;
	in_cab = false;
	first_scammed = false;
	coin1_enabled, coin1_obtained, coin2_enabled, coin2_obtained, coin3_enabled, coin3_obtained = false;
}
