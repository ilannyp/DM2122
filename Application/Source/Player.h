#pragma once
class Player
{
private:
	int currency;
	bool alive;
	bool in_cab;
	bool first_scammed;
	bool coin1_obtained, coin2_obtained, coin3_obtained;
	bool coin1_enabled, coin2_enabled, coin3_enabled;
	bool currency_added_from_C1, currency_added_from_C2, currency_added_from_C3;
	bool win;
	int coins_collected;
	bool shop1, shop2, shop3, shop4;
	bool go_lv3;

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

	void set_go_lv3(bool x);
	bool get_go_lv3();

	void set_first_scammed(bool x);
	bool get_first_scammed();

	bool get_coin1_obtained();
	bool get_coin2_obtained();
	bool get_coin3_obtained();

	void set_coin1_obtained(bool x);
	void set_coin2_obtained(bool x);
	void set_coin3_obtained(bool x);

	bool get_coin1_enabled();
	bool get_coin2_enabled();
	bool get_coin3_enabled();

	void set_coin1_enabled(bool x);
	void set_coin2_enabled(bool x);
	void set_coin3_enabled(bool x);

	bool get_currency_added_from_C1();
	bool get_currency_added_from_C2();
	bool get_currency_added_from_C3();

	void set_currency_added_from_C1(bool x);
	void set_currency_added_from_C2(bool x);
	void set_currency_added_from_C3(bool x);

	void set_win(bool x);
	bool get_win();

	int get_coins_collected();
	void add_coins(int x);
	void deduct_coins(int x);
	void set_coins(int x);

	bool get_shop1();
	bool get_shop2();
	bool get_shop3();
	bool get_shop4();

	void set_shop1(bool x);
	void set_shop2(bool x);
	void set_shop3(bool x);
	void set_shop4(bool x);

	void total_reset();
};