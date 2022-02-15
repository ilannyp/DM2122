#pragma once
class Player
{
private:
	int currency;
	bool alive;
	int gold;

public:
	Player();
	~Player();

	int get_currency();
	void set_currency(int x);

	void currency_deducted(int x);
	void currency_added(int x);

	void set_alive();
	void set_die();

	bool get_life_status();

	int get_gold();
	void set_gold(int x);

	void gold_deducted(int x);
	void gold_added(int x);
};

