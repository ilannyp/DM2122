#pragma once
class Player
{
private:
	int currency;
	bool alive;
	bool in_cab;

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

	void set_in_cab(bool x);
	bool get_in_cab();

};

