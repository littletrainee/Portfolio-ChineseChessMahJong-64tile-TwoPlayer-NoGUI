#include <algorithm>
#include <ctime>
#include <iostream>
#include <iterator>
#include <map>
#include <set>
#include <string>
#include <vector>
#ifdef WIN32 
#include <Windows.h>
#else
#include <unistd.h>
#endif
using namespace std;

void Delay(int t){
#ifdef WIN32
	Sleep(t);
#else
	usleep(1000*t);
#endif
}

// player struct
struct Player{
	string name = "";
	int code;
	bool tenpai = false;
	bool concealed = true;
	bool bookmaker = false;
	vector<string> hand;
	vector<vector<string>> meldhand;
};

// append to wallhand from tile 4time
void AppendToWallHand(map<string, string> tiles, vector<string> &wallhand) {
	for (map<string, string>::iterator tile = tiles.begin(); tile != tiles.end();
			tile++)
		switch(tile->first[0]){
			case '1':
				for (int i = 0; i < 2; i++)
					wallhand.push_back(tile->first);
				break;
			case '7':
				for (int i = 0;i < 10; i++)
					wallhand.push_back(tile->first);
				break;
			default:
				for(int i = 0; i < 4; i++)
					wallhand.push_back(tile->first);
				break;
		}
	//for (int i = 0; i < 4; ++i)
	//wallhand.push_back(tile->first);
};

// draw card from wallhand to playerhand
void DrawCard(vector<string> &wallhand, vector<string> &playerhand) {
	playerhand.push_back(wallhand.front());
	wallhand.erase(wallhand.begin());
}

// sort playerhand by sortby
void Sort(vector<string> sortby, vector<string> &playerhand) {
	vector<string> temphand;
	for (vector<string>::iterator s = sortby.begin(); s != sortby.end(); s++)
		if (find(playerhand.begin(), playerhand.end(), *s) != playerhand.end())
			for (int j = 0; j < count(playerhand.begin(), playerhand.end(), *s); j++)
				temphand.push_back(*s);
	playerhand = temphand;
}

// set bookmaker and drawcard sort
void SetUp(Player &player1, Player &player2, map<string,string> tile, 
		vector<string> &wallhand, vector<string> sortby){
	// declare vector<vector<string>
	vector<vector<string>> temphand = {{},{}};
	// append tile to wallhand and shuffle
	AppendToWallHand(tile, wallhand);
	random_shuffle(wallhand.begin(), wallhand.end());
	// each player drawcard 2 tile 3 time (6tile)
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 2; j++)
			DrawCard(wallhand, temphand[0]);
		for (int j = 0; j < 2; j++)
			DrawCard(wallhand, temphand[1]);
	}
	// each player draw the 7th tile
	DrawCard(wallhand, temphand[0]);
	DrawCard(wallhand, temphand[1]);
	// frist player draw the 8th card
	DrawCard(wallhand, temphand[0]);
	// sort all playerhand
	Sort(sortby, temphand[0]);
	Sort(sortby, temphand[1]);
	// player1 be bookmaker
	if (player1.bookmaker){
		player1.hand = temphand[0];
		player2.hand = temphand[1];
		// palyer2 be bookmaker
	}else if (player2.bookmaker){
		player2.hand = temphand[0];
		player1.hand = temphand[1];
	}
}

// PrintAll class
class PrintAll {
	public:
		// set name and map function
		void PrintAll_SetNameAndMap(string player1, string player2, string wall, 
				string river, map<string, string> tile) {
			this->player1 = player1;
			this->player2 = player2;
			this->wall = wall;
			this->river = river;
			this->tile = tile;
		};
		// print hand function
		void PrintHand(int whitchplayer, vector<string> hand, bool shownumber) {
			// -1 = river, 0 = wall, 1 = player1, 2 = player2
			switch (whitchplayer) { 
				case -1:
					cout << this->river;
					break;
				case 0:
					cout << this->wall;
					break;
				case 1:
					cout << this->player1;
					break;
				case 2:
					cout << this->player2;
					break;
			}
			if (whitchplayer >  0)
				cout << "手牌";
			cout << ":";
			if (shownumber)
				for (int i = 0; i < hand.size(); i++)
					cout << i + 1 << "." << this->tile[hand[i]] << "  ";
			else {
				for (vector<string>::iterator target = hand.begin(); target != hand.end();
						target++){
					if ((target - hand.begin()) % 12 == 0)
						cout << endl;
					cout << this->tile[*target] << ' ';
				}
			}
			cout << endl << endl;
		};
		// print all hand function
		void PrintAll_Print(Player player1, Player player2,	
				vector<string> wallhand, vector<string> riverhand) {
			// print wall
			cout << "============================================================" << endl;
			PrintHand(0, wallhand, false);
			// print player1 and player1meld
			cout << "============================================================" << endl;
			PrintHand(1, player1.hand, false);
			cout << "------------------------------------------------------------" << endl;
			PrintMeldHand(1, player1.meldhand);
			// print river
			cout << "============================================================" << endl;
			PrintHand(-1, riverhand, false);
			// print player2 and player2meld
			cout << "============================================================" << endl;
			PrintMeldHand(2, player2.meldhand);
			cout << "------------------------------------------------------------" << endl;
			PrintHand(2, player2.hand, false);
		};
	private:
		// declare type
		string              player1;
		string              player2;
		string              wall;
		string              river;
		map<string, string> tile;
		// print meldhand function
		void PrintMeldHand(int whitchplayermeldhand, 
				vector<vector<string>> meldhand) {
			vector<string> tempmeld;
			switch (whitchplayermeldhand) {
				case 1:
					cout << this->player1;
					break;
				case 2:
					cout << this->player2;
					break;
			}
			cout << "組合手牌:";
			for (vector<vector<string>>::iterator meld = meldhand.begin();
					meld != meldhand.end(); meld++) {
				cout << "{ ";
				tempmeld = *meld;
				for (vector<string>::iterator m = tempmeld.begin(); m != tempmeld.end();
						m++)
					cout << this->tile[*m] << ' ';
				cout << '}';
			}
			cout << endl << endl;
		};
};

// CheckWin  class
class CheckWin {
	public:
		// declare type
		string eye;
		// set map tile function
		void CheckWin_SetTile(vector<string> tile) { 
			this->tile = tile;
		};
		// check function
		bool CheckWin_Check(Player player, vector<string> riverhand) {
			// declare type
			bool           isgamingloop = true;
			vector<string> temphand;
			vector<string> eyes;
			switch(player.meldhand.size()){
				case 0:
					bool meld1;
					bool meld2;
					// check playerhand is 8 and sort
					if (player.hand.size() != 8)
						player.hand.push_back(riverhand.back());
					Sort(this->tile, player.hand);
					// clone playerhand to temphand
					this->hand = player.hand;
					// check there is a pair in playerhand
					eyes = ProbablyEyes();
					// 4-pair
					if (eyes.size() == 4) {
						isgamingloop = false;
						this->eye = "4pair";
						// not 4-pair
					}else{
						// eyes isn't empty
						if (!eyes.empty()) { 
							for (vector<string>::iterator it = eyes.begin(); it != eyes.end();
									it++) {
								// check playerhand need reset
								if (player.hand != this->hand)
									player.hand = this->hand;
								// erase eye 2 time
								for (int i = 0; i < 2; i++)
									player.hand.erase(find(player.hand.begin(), player.hand.end(), *it));
								// [0][1][2] = meld1, [3][4][5] = meld2
								meld1 = CheckMeld(player.hand[0], player.hand[1], 
										player.hand[2]);
								meld2 = CheckMeld(player.hand[3], player.hand[4], 
										player.hand[5]);
								// meld1 is a meld and meld2 is a meld then set isgamingloop to false
								if (meld1 && meld2) {
									isgamingloop = false;
									this->eye = *it;
									break;
									// [0][2][4] = meld1, [1][3][5] = meld2
								}else{
									meld1 = CheckMeld(player.hand[0], player.hand[2], 
											player.hand[4]);
									meld2 = CheckMeld(player.hand[1], player.hand[3],
											player.hand[5]);	
									if (meld1 && meld2){
										isgamingloop = false;
										this->eye = *it;
										break;
									}
								}
							}
						}
					}
					break;
				case 1:
					// check playerhand is 5 and sort
					if (player.hand.size() != 5)
						player.hand.push_back(riverhand.back());
					Sort(this->tile, player.hand);
					// clone playerhand to temphand
					this->hand = player.hand;
					// check there is a pair in playerhand
					eyes = ProbablyEyes();
					//eye isn't empty
					if (!eyes.empty()){
						for(vector<string>::iterator eye = eyes.begin(); eye != eyes.end();
								eye++){
							// check playerhand need reset
							if (player.hand != this->hand)
								player.hand = this->hand;
							// erase eye 2 time
							for (int i = 0; i < 2; i++)
								player.hand.erase(find(player.hand.begin(), player.hand.end(), *eye));
							// hand is meld then set isgamingloop to false
							if (CheckMeld(player.hand[0], player.hand[1], player.hand[2])){
								isgamingloop = false;
								this->eye = *eye;
								break;
							}
						}
					}
					break;
				case 2:
					if (player.hand[0] == player.hand[1]){
						isgamingloop = false;
						this->eye = player.hand[0];
					}else if (player.hand[0] == riverhand.back()){
						isgamingloop = false;
						this->eye = player.hand[0];
					}
					break;
			}
			return isgamingloop;
		};
		// get eye function
		void GetEye(string &eye) {
			eye = this->eye;
		};
	private:
		// declare type
		vector<string> tile;
		vector<string> hand;
		// probabley eyes function
		vector<string> ProbablyEyes() {
			// declare type
			vector<string> temphand;
			for (vector<string>::iterator t = this->tile.begin(); t != this->tile.end();
					t++)
				if (find(this->hand.begin(), this->hand.end(), *t) != this->hand.end() &&
						count(this->hand.begin(), this->hand.end(), *t) >= 2)
					temphand.push_back(*t);
			return temphand;
		};
		// check meld function
		bool CheckMeld(string a, string b, string c) {
			// declare type
			bool returnbool = false;
			// check kind
			if (a[1] == b[1] && b[1] == c[1]) 
				// check sequence or triple
				// sequence
				if ((int)a[0] - 47 == (int)b[0] - 48 &&	(int)b[0] - 47 == (int)c[0] - 48 
						// triple
						|| a == b && b == c)                 
					returnbool = !returnbool;
			return returnbool;
		};
};

// discard function
void DisCard(Player &player, vector<string> &riverhand) {
	// declare type
	char                     keyin;
	int                      target;
	vector<string>::iterator it;
	cout << "輪到" << player.name << endl;
	cout << "請選擇您要打出牌的編號(1-" << player.hand.size() << "):";
	cin >> keyin;
	while (isdigit(keyin) == false && (int)keyin != 1 && (int)keyin != 2 &&
			(int)keyin != 3 && (int)keyin != 4 && (int)keyin != 5) {
		cout << "請重新輸入:";
		cin >> keyin;
	}
	target = (int)keyin - 48; // set select number
	it = player.hand.begin() + target - 1;
	riverhand.push_back(player.hand[target - 1]);
	player.hand.erase(it);
}

// Kong class
class Kong {
	public:
		// big meld kong function
		void Big(Player &player, vector<string> &river, vector<string> &wall, 
				bool &top) {
			// remove the same as river last element from hand
			for(int i = 0; i < 3; i++)
				player.hand.erase(find(player.hand.begin(), 
							player.hand.end(), river.back()));
			// append quadruple river last element to meldhand
			player.meldhand.push_back(
					{river.back(), river.back(), river.back(), river.back()});
			// remove river last element
			river.pop_back();
			DrawFromWallTail(player.hand, wall, top);
		};
		// Concealed and Small kong function
		void CheckConcealedAndSmall(Player &player, vector<string> &wall, 
				bool &top){
			char key;
			// Concealed
			if (count(player.hand.begin(), player.hand.end(), 
						player.hand.back()) == 4){
				cout << "要槓嗎(y/n)?";
				cin >> key;
				if (key == 'y') {
					player.meldhand.push_back({player.hand.back(), player.hand.back(), 
							player.hand.back(), player.hand.back()});
					for (int i = 0; i < 4; i++)
						player.hand.erase(find(player.hand.begin(), player.hand.end(), 
									player.hand.back()));
					DrawFromWallTail(player.hand, wall, top);
				}
				// Small
			}else if (!player.meldhand.empty() && count(player.meldhand[0].begin(), 
						player.meldhand[0].end(), player.hand.back()) == 3 ){
				cout << "要槓嗎(y/n)?";
				cin >> key;
				if (key == 'y') {
					player.meldhand[0].push_back(player.hand.back());
					player.hand.pop_back();
					DrawFromWallTail(player.hand, wall, top);
				}
			};
		};
	private:
		// Draw From Wall Tail function
		void DrawFromWallTail(vector<string> &hand, vector<string> &wall, bool &top){
			// check need to draw last or second last
			if(top){ // second last = top tile on pier
				hand.push_back(*(wall.rbegin() + 1));
				wall.erase(wall.end() - 2);
			}else{ // last = bottom on pier
				hand.push_back(wall.back());
				wall.erase(wall.end());
			}
			// switch between last and second last
			top = !top;
		};
};

// check meld class inheritance from Kong class
class CheckMeld : public Kong {
	public:
		// declare type
		char key;
		// Set Tile function
		void SetTile(map<string, string> tile) { 
			this->tile = tile;
		};
		// Check function
		bool Check(vector<string> riverhand, Player player, bool isnextplayer) {
			// set this->target = riverhand.back and this->hand = hand
			this->target = riverhand.back();
			this->hand = player.hand;
			this->name = player.name;
			// call CheckTripleAndQuadruple function
			CheckTripleAndQuadruple();
			if (isnextplayer){
				// call CheckSequence function
				CheckSequence();
				if (!this->probablysequence.empty() || this->triple || this->quadruple)
					return true;
				else
					return false;
			}else{
				if (this->triple || this->quadruple)
					return true;
				else
					return false;
			}
		};
		// player meld select function
		void MeldAskAndSelect() {
			// declare
			string maxrange = "";
MeldChoice:
			// print question for meld yes or not
			// this->probablysequence isn't empty
			if (!this->probablysequence.empty()) {
				// and this->triple is true
				if (this->triple) {
					// and this->quadruple is true
					if (this->quadruple) {
						cout << "輪到" << this->name << endl; 
						cout << "要吃(c)碰(p)槓(k)或跳過(s)嗎? ";
						maxrange = "cpks";
						// this->quadruple isn't true
					} else {
						cout << "輪到" << this->name << endl; 
						cout << "要吃(c)碰(p)或跳過(s)嗎? ";
						maxrange = "cps";
					}
					// this->triple isn''t ture
				} else {
					cout << "輪到" << this->name << endl; 
					cout << "要吃(c)或跳過(s)嗎? ";
					maxrange = "cs";
				}
				// player choice
				cin >> this->key;
				// check the enter is correct
				if (maxrange.empty()) {
					switch (maxrange.size()) {
						case 2:
							while (this->key != maxrange[0] && this->key != maxrange[1]) {
								cout << "請重新輸入選擇:";
								cin >> this->key;
								cout << endl;
							}
							break;
						case 3:
							while (this->key != maxrange[0] && this->key != maxrange[1] &&
									this->key != maxrange[2]) {
								cout << "請重新輸入選擇:";
								cin >> this->key;
								cout << endl;
							}
							break;
						case 4:
							while (this->key != maxrange[0] && this->key != maxrange[1] &&
									this->key != maxrange[2] && this->key != maxrange[3]) {
								cout << "請重新輸入選擇:";
								cin >> this->key;
								cout << endl;
							}
							break;
					}
				}
				// this-> probabllysequence is empty this->triple is true
			} else if (this->triple) {
				// and this->quadruple is true
				if (this->quadruple) {
					cout << "輪到" << this->name << endl; 
					cout << "要碰(p)槓(k)或跳過(s)嗎? ";
					maxrange = "pks";
					// this->quadruple isn't ture
				} else {
					cout << "輪到" << this->name << endl; 
					cout << "要碰(p)或跳過(s)嗎? ";
					maxrange = "ps";
				}
				// player choice
				cin >> this->key;
				// check the enter is correct
				if (maxrange.empty()) {
					switch (maxrange.size()) {
						case 2:
							while (this->key != maxrange[0] && this->key != maxrange[1]) {
								cout << "請重新輸入選擇:";
								cin >> this->key;
								cout << endl;
							}
							break;
						case 3:
							while (this->key != maxrange[0] && this->key != maxrange[1] &&
									this->key != maxrange[2]) {
								cout << "請重新輸入選擇:";
								cin >> this->key;
								cout << endl;
							}
							break;
					}
				}
			} else {
				this->key = 's';
			}
		};
		// Been Meld function
		void BeenMeld(Player &player, vector<string> &river, vector<string> &wall, bool &top) {
			switch (this->key) {
				case 'c':
					Chi(player, river);
					break;
				case 'p':
					Pong(player, river);
					break;
				case 'k':
					Big(player, river, wall, top);
					break;
				case 's':
					DrawCard(wall, player.hand);
					break;
			}
		};
		// Chi function
		void Chi(Player &player, vector<string> &river) {
			// declare
			vector<string> temphand;
			// check this->probablysequence is more than 1
			if (this->probablysequence.size() != 1) {
				// print this->probablysequence
				for (vector<vector<string>>::iterator probablyseq =
						this->probablysequence.begin();
						probablyseq != this->probablysequence.end();
						probablyseq++) {
					// print index
					cout << probablyseq - this->probablysequence.begin() + 1 << '.';
					// print probablysequence
					for (vector<string>::iterator ps = probablyseq->begin();
							ps != probablyseq->end(); ps++)
						cout << this->tile[*ps] << ' ';
					cout << ' ';
				}
				// show the warnning message and let player choice
ReturnSelect:
				cout << endl
					<< "有超過" << this->probablysequence.size() << "個選擇，"
					<< "請選擇(1-" << this->probablysequence.size() << ")或是返回(r): ";
				// player enter choice
				cin >> this->key;
				// check the enter is correct
				switch (this->probablysequence.size()) {
					case 2:
						while (this->key != '1' && this->key != '2' && this->key != 'r') {
							cout << "請重新輸入選擇:";
							cin >> this->key;
						}
						break;
					case 3:
						while (this->key != '1' && this->key != '2' && this->key != '3' &&
								this->key != 'r') {
							cout << "請重新輸入選擇:";
							cin >> this->key;
						}
						break;
				}
				// set temphand = player choice
				switch (this->key) {
					case '1':
						temphand = this->probablysequence[0];
						break;
					case '2':
						temphand = this->probablysequence[1];
						break;
					case '3':
						temphand = this->probablysequence[2];
						break;
					case 'r':
						goto ReturnSelect;
				}
			} else { // this->probablysequence is equal to 1
				temphand = this->probablysequence[0];
			}
			// make meld to meldhand from hand and riverhand
			player.hand.erase(find(player.hand.begin(), player.hand.end(), temphand[0]));
			player.hand.erase(find(player.hand.begin(), player.hand.end(), temphand[1]));
			// insert river last to temphand
			temphand.insert(temphand.begin() + 1, river.back());
			// remove river last element
			river.pop_back();
			// append temphand to meldhand
			player.meldhand.push_back(temphand);
		};
		// Pong function
		void Pong(Player &player, vector<string> &river) {
			// remove the same as river last element from hand
			for (int i = 0; i < 2; i++)
				player.hand.erase(find(player.hand.begin(), 
							player.hand.end(), river.back()));
			// append triple river last element to meldhand
			player.meldhand.push_back({river.back(), river.back(), river.back()});
			// remove river last element
			river.pop_back();
		};
		// reset check meld function
		void Reset() {
			this->target = "";
			this->name = "";
			this->hand.clear();
			this->meldselect.clear();
			this->probablysequence.clear();
			this->triple = false;
			this->quadruple = false;
			this->key = ' ';
		};
	private:
		// declare type
		bool                   triple = false;
		bool                   quadruple = false;
		map<string, string>    tile;
		string                 target;
		string 								 name;
		vector<string>         hand;
		vector<string>         meldselect;
		vector<vector<string>> probablysequence;
		// check sequence function
		void CheckSequence() {
			string probably1 = to_string((int)this->target[0] - 50) + target[1];
			string probably2 = to_string((int)this->target[0] - 49) + target[1];
			string probably3 = to_string((int)this->target[0] - 47) + target[1];
			string probably4 = to_string((int)this->target[0] - 46) + target[1];
			vector<string>::iterator p1 =
				find(this->hand.begin(), this->hand.end(), probably1);
			vector<string>::iterator p2 =
				find(this->hand.begin(), this->hand.end(), probably2);
			vector<string>::iterator p3 =
				find(this->hand.begin(), this->hand.end(), probably3);
			vector<string>::iterator p4 =
				find(this->hand.begin(), this->hand.end(), probably4);
			if (p1 != this->hand.end() && p2 != this->hand.end())
				this->probablysequence.push_back({probably1, probably2});
			if (p2 != this->hand.end() && p3 != this->hand.end())
				this->probablysequence.push_back({probably2, probably3});
			if (p3 != this->hand.end() && p4 != this->hand.end())
				this->probablysequence.push_back({probably3, probably4});
		};
		// check triple and quadruple function
		void CheckTripleAndQuadruple() {
			if (count(this->hand.begin(), this->hand.end(), this->target) >= 2)
				this->triple = true;
			if (count(this->hand.begin(), this->hand.end(), this->target) == 3)
				this->quadruple = true;
		};
};

// ten pai check function
void TenPaiCheck(Player &player, vector<string> tiles, vector<string> riverhand) {
	// declare type
	char           key;
	vector<string> probablywintile;
	vector<string> checkhand;
	vector<string> poponetilehand;
	Player         temp = player;
	CheckWin       cw;
	cw.CheckWin_SetTile(tiles);
	// pop one by one from checkhand and append one by one from tile to check win
	for (vector<string>::iterator compareby = player.hand.begin();
			compareby != player.hand.end(); compareby++) {
		// reset checkhand from playerhand 
		checkhand = player.hand;
		// try to pop one of checkhand for check win
		checkhand.erase(find(checkhand.begin(), checkhand.end(), *compareby));
		// clone checkhand to poponetilehand(hand minus one)
		poponetilehand = checkhand;
		// try to append tile to hand for check win one by one  
		for (vector<string>::iterator tile = tiles.begin(); tile != tiles.end();
				tile++) {
			// reset checkhand from poponetilehand
			checkhand = poponetilehand;
			// append tile to checkhand equal to hand amont
			checkhand.push_back(*tile);
			// sort checkhand for check win
			Sort(tiles, checkhand);
			// copy to clonecheckhand from checkhand
			temp.hand = checkhand;
			if (!cw.CheckWin_Check(temp, riverhand)) {
				// check the tile isn't in probablywintile
				if (find(probablywintile.begin(), probablywintile.end(), *tile) ==
						probablywintile.end()) {
					// then add to probablywintile
					probablywintile.push_back(*tile);
				}
			}
		}
	}
	// probablywintile isn't empty
	if (!probablywintile.empty()) {
		cout << "要宣告聽牌嗎(y/n)? ";
		cin >> key;
		cout << endl;
		// check key is equal to y or n
		while (key != 'y' && key != 'n'){
			cout << "請重新輸入選擇: ";
			cin >> key;
			cout << endl;
		}
		// set tenpai
		player.tenpai = key == 'y' ? true : false;
	}
}

// AutoDiscard function
void AutoDisCard(vector<string> &playerhand, vector<string> &riverhand) {
	riverhand.push_back(playerhand.back());
	playerhand.pop_back();
}

// Score Class
class Score {
	public:
		// set tile function
		void SetTile(map<string,string> tile){
			this->tile = tile;
		};
		// set score calcualte according to information from player
		void SetAccordingTo(vector<string> sortby, Player player, 
				vector<string> riverhand, string eye, int gameround,
				int gameturn, bool tsumo, bool lastwalltile, 
				int continuetobookmaker) {
			this->sortby = sortby;
			this->playerhand = player.hand;
			this->playermeldhand = player.meldhand;
			this->concealed = player.concealed;
			this->riverhandlastelement = riverhand.back();
			this->gameround = gameround;
			this->gameturn = gameturn;
			this->eye = eye;
			this->tsumo = tsumo;
			this->lastwalltile = lastwalltile;
			this->bookmaker = player.bookmaker;
			this->continuetobookmaker = continuetobookmaker;
			this->tenpai = player.tenpai;
		};
		// print score
		void PrintScore() {
			// declare type
			int ct = 0;
			switch(this->gameturn){
				case 0:
					cout << "獲勝者是玩家1" << endl << endl; 
					break;
				case 1:
					cout << "獲勝者是玩家2" << endl << endl; 
					break;
			}
			Delay(750);
			// print player hand
			ct += this->playerhand.size();
			if (!this->tsumo){
				for (vector<string>::iterator target = this->playerhand.begin();
						target != this->playerhand.end(); target ++)
					cout << this->tile[*target] << ' ';
			}else{
				for (vector<string>::iterator target = this->playerhand.begin();
						target != this->playerhand.end(); target ++){
					if (target == this->playerhand.end()-1)
						break;
					else
						cout << this->tile[*target] << ' ';
				}
			}
			cout << "    ";
			// print player meld hand
			if(!this->playermeldhand.empty())
				for(vector<vector<string>>::iterator target = this->playermeldhand.begin();
						target != this->playermeldhand.end(); target++){
					ct += target->size();
					for(vector<string>::iterator t = target->begin(); t != target->end(); t++)
						cout << this->tile[*t] << ' ';
				}
			// print ron(tsumo) by
			cout << "    ";
			if(ct != 8){
				cout << this->tile[this->riverhandlastelement] ;
			}else{
				cout << this->tile[this->playerhand.back()];
			}
			cout << endl << endl;
			Ten_DiOrNinHou();
			TenPai();
			BookMaker();
			ContinueToBookmaker();
			OnlyOrNoGeneralAndSorider();
			TsumoOrNot();
			WinOnLastTile();
			FourPair();
			SameKind();
			TwoKong();
			EightGenerals();//
			if(this->noeightgenerals){
				AllPaired();//
				FiveSoldier();//
			}
			TwoDragonHug();
			OneDragon();
			Delay(750);
			cout << "===============" << endl;
			Delay(750);
			cout << "共：        " << this->totaltai << "台" << endl;
		};
	private:
		// delcare type
		bool 									 noeightgenerals = false;
		bool                   tsumo = false;
		bool                   lastwalltile = false;
		bool                   winonthewalltail = false;
		bool                   bookmaker = false;
		bool                   tenpai = false;
		bool 									 concealed = true;
		int                    totaltai = 0;
		int                    gameround;
		int                    gameturn;
		int 									 continuetobookmaker = 0;
		map<string,string>     tile;
		string                 riverhandlastelement;
		string                 eye;
		vector<string>         sortby;
		vector<string>         playerhand;
		vector<vector<string>> playermeldhand;
		// 10 or 5 tai
		// ten, di or nin hou function
		void Ten_DiOrNinHou() {
			if (this->gameround == 1 && this->bookmaker && this->tsumo) {
				this->tsumo;
				this->totaltai += 10;
				cout << "天胡       10台" << endl;
				Delay(750);
			}else if (this->gameround == 1 && !this->bookmaker && this->tsumo){
				this->tsumo;
				this->totaltai += 10;
				cout << "地胡       10台" << endl;
				Delay(750);
			}else if (this->gameround == 1 && !this->tsumo){
				this->tsumo;
				this->totaltai += 10;
				cout << "人胡        5台" << endl;
				Delay(750);
			}
		};
		// 8 tai
		// eight generals function
		void EightGenerals(){
			vector<string> temphand = this->playerhand;
			if (!this->playermeldhand.empty())
				for (vector<vector<string>>::iterator meld = this->playermeldhand.begin();
						meld != this->playermeldhand.end(); meld++){
					if (meld->size() == 4)
						temphand.insert(temphand.end(), 3, meld->back());
					else if ((*meld)[0] != (*meld)[1] && (*meld)[1] != (*meld)[2])
						temphand.insert(temphand.end(), meld->begin(), meld->end());
				}
			if (temphand.size() != 8)
				temphand.push_back(this->riverhandlastelement);
			for(vector<string>::iterator hand = this->playerhand.begin();
					hand != this->playerhand.end(); hand++){
				if(*hand != "7b" || *hand != "7r"){
					this->noeightgenerals = true;
					break;
				}
				else if (hand == this->playerhand.end() - 1){
					this->totaltai += 8;
					cout << "八家將      4台" << endl;
					Delay(750);
				}	
			}
		};
		// 4 tai
		// one dragon
		void OneDragon() {
			vector<string> temphand = this->playerhand;
			vector<string> compariso_b{"1b", "2b", "3b", "4b", "5b", "6b", "7b", "7b"};
			vector<string> compariso_r{"1r", "2r", "3r", "4r", "5r", "6r", "7r", "7r"};
			if (!this->playermeldhand.empty()) {
				for (vector<vector<string>>::iterator meld = this->playermeldhand.begin();
						meld != this->playermeldhand.end(); meld++)
					if ((*meld)[0] != (*meld)[1] && (*meld)[1] != (*meld)[2])
						temphand.insert(temphand.end(), meld->begin(), meld->end());
			}
			if (temphand.size() != 8)
				temphand.push_back(this->riverhandlastelement);
			Sort(this->sortby, temphand);
			if (temphand == compariso_b || temphand == compariso_r) {
				this->totaltai += 4;
				cout << "一條龍      4台" << endl;
				Delay(750);
			}
		};
		// two dragon hug
		void TwoDragonHug() {
			vector<string> temphand;
			vector<string> compare1;
			vector<string> compare2;
			bool           turn = true;
			if (this->concealed) {
				temphand = this->playerhand;
				if (temphand.size() != 8)
					temphand.push_back(this->riverhandlastelement);
				Sort(this->sortby, temphand);
				for (int i = 0; i < 2; i++)
					temphand.erase(find(temphand.begin(), temphand.end(), this->eye));
				for (vector<string>::iterator t = temphand.begin(); t != temphand.end();
						t++) {
					if (turn) {
						compare1.push_back(*t);
						turn = !turn;
					} else {
						compare2.push_back(*t);
						turn = !turn;
					}
				}
				if(compare1[0] == to_string((int)compare1[1][0] - 49) + compare1[1][1] 
					 && compare1[1] == to_string((int)compare1[2][0] - 49) + compare1[2][1]
					 && compare2[0] == to_string((int)compare2[1][0] - 49) + compare2[1][1]
					 && compare2[1] == to_string((int)compare2[2][0] - 49) + compare2[2][1]
					 && compare1 == compare2) {
					this->totaltai += 4;
					cout << "雙龍抱      4台" << endl;
					Delay(750);
				}
			}
		};
		// 2 tai
		// FiveSoldier
		void FiveSoldier(){
			vector<string> temphand = this->playerhand;
			if (!this->playermeldhand.empty()) {
				for (vector<vector<string>>::iterator meld = this->playermeldhand.begin();
						meld != this->playermeldhand.end(); meld++)
					if ((*meld)[0] != (*meld)[1] && (*meld)[1] != (*meld)[2])
						temphand.insert(temphand.end(), meld->begin(), meld->end());
			}
			if (temphand.size() != 8)
				temphand.push_back(this->riverhandlastelement);
			if (count(temphand.begin(), temphand.end(), "b7") == 5){
				this -> totaltai += 2;
				cout << "五卒連橫    2台"  <<  endl;
			}else if (count(temphand.begin(), temphand.end(), "r7") == 5){
				this -> totaltai  += 2;	
				cout << "五兵合縱    2台" <<  endl;
			}
		};
		// all paired
		void AllPaired() {
			vector<string> temphand;
			// copy playerhand to temphand
			temphand = this->playerhand;
			// if palyermeldhand isn't empty then add element to temphand
			if (!this->playermeldhand.empty()) {
				for (vector<vector<string>>::iterator meld = this->playermeldhand.begin();
						meld != this->playermeldhand.end(); meld++)
					if ((*meld)[0] == (*meld)[1] && (*meld)[1] == (*meld)[2])
						for (int i = 0; i < 3; i++)
							temphand.push_back((*meld)[0]);
			}
			// check the tempahnd isn't equal to 8
			if (temphand.size() != 8)
				temphand.push_back(this->riverhandlastelement);
			// sort temphand
			Sort(this->sortby, temphand);
			// pop out the eye from temphand
			for (int i = 0; i < 2; i++)
				temphand.erase(find(temphand.begin(), temphand.end(), this->eye));
			if (temphand[0] == temphand[1] && temphand[1] == temphand[2] &&
					temphand[3] == temphand[4] && temphand[4] == temphand[5]) {
				this->totaltai += 2;
				cout << "碰碰胡      2台" << endl;
				Delay(750);
			}
		};
		// two kong
		void TwoKong() {
			if (this->playermeldhand.size() ==2)
				if (this->playermeldhand[0].size() == 4 &&
						this->playermeldhand[1].size() == 4) {
					this->totaltai += 2;
					cout << "二槓子      2台" << endl;
					Delay(750);
				}
		};
		// 2 or 1 tai
		// tsumo or not
		void TsumoOrNot() {
			if (this->tsumo) {
				this->totaltai += 2;
				cout << "自摸        2台" << endl;
				Delay(750);
			} else if (!this->concealed && !this->tsumo) {
				this->totaltai += 1;
				cout << "胡牌        1台" << endl;
				Delay(750);
			}
		};
		// only or no general and sorider(chinlauto or danyau)
		void OnlyOrNoGeneralAndSorider() {
			vector<string> temphand;
			set<int>       compare;
			// copy playerhand to temphand
			temphand = this->playerhand;
			// if palyermeldhand isn't empty then add element to temphand
			if (!this->playermeldhand.empty()) {
				for (vector<vector<string>>::iterator meld = this->playermeldhand.begin();
						meld != this->playermeldhand.end(); meld++)
					temphand.insert(temphand.end(), meld->begin(), meld->end());
			}
			// check the tempahnd isn't equal to 8
			if (temphand.size() != 8)
				temphand.push_back(this->riverhandlastelement);
			// sort temphand
			Sort(this->sortby, temphand);
			// check only general and sorider
			for (vector<string>::iterator temp = temphand.begin();
					temp != temphand.end(); temp++) {
				// if there is 1 or 7 in temphand
				(*temp)[0] == '1' || (*temp)[0] == '7'
					?
					// set false to true mean no generalandsorider
					compare.insert(1)
					: compare.insert(0);
			}
			// doesn't contain '1' and '7' but contain '2' ~ '6'
			if (find(compare.begin(), compare.end(), 1) == compare.end() &&
					find(compare.begin(), compare.end(), 0) != compare.end()) {
				this->totaltai += 1;
				cout << "段頭尾      1台" << endl;
				Delay(750);
			} // contain '1' and '7' but not contain '2' ~ '6'
			else if (find(compare.begin(), compare.end(), 1) != compare.end() &&
					find(compare.begin(), compare.end(), 0) == compare.end()) {
				this->totaltai += 2;
				cout << "將帥領兵    2台" << endl;
				Delay(750);
			}
		};
		// double tai
		// continu to bookmaker 
		void ContinueToBookmaker() {
			if (this->bookmaker) {
				if (this->continuetobookmaker > 0){
					this->totaltai += this->continuetobookmaker*2;
					cout << "連" << this->continuetobookmaker  << "拉" 
						<< this->continuetobookmaker  << "      " 
						<< this->continuetobookmaker * 2 << "台" << endl;  
					Delay(750);
				}
			}
		};
		// 1 tai
		// win on last draw tile or last discard tile
		void WinOnLastTile() {
			if (this->lastwalltile && this->tsumo) {
				this->totaltai += 1;
				cout << "海底撈月    1台" << endl;
				Delay(750);
			} else if (this->lastwalltile && !this->tsumo) {
				this->totaltai += 1;
				cout << "河底撈魚    1台" << endl;
				Delay(750);
			}
		};
		// same kind
		void SameKind() {
			bool           samekind = true;
			char           kind = ' ';
			vector<string> temphand;
			// copy playerhand to temphand
			temphand = this->playerhand;
			// if palyermeldhand isn't empty then add element to temphand
			if (!this->playermeldhand.empty()) {
				for (vector<vector<string>>::iterator meld = this->playermeldhand.begin();
						meld != this->playermeldhand.end(); meld++)
					if ((*meld)[0] != (*meld)[1] && (*meld)[1] != (*meld)[2])
						temphand.insert(temphand.end(), meld->begin(), meld->end());
			}
			// check the tempahnd isn't equal to 8
			if (temphand.size() != 8)
				temphand.push_back(this->riverhandlastelement);
			// sort temphand
			Sort(this->sortby, temphand);
			kind = temphand[0][1];
			for (vector<string>::iterator temp = temphand.begin();
					temp != temphand.end(); temp++) {
				if ((*temp)[1] != kind) {
					samekind = !samekind;
					break;
				}
			}
			if (samekind) {
				this->totaltai += 1;
				cout << "清一色      1台" << endl;
				Delay(750);
			}
		};
		// four pair
		void FourPair() {
			vector<string> temphand;
			// copy playerhand to temphand
			temphand = this->playerhand;
			// if palyermeldhand isn't empty then add element to temphand
			if (this->concealed){
				if (!this->playermeldhand.empty()) {
					for (vector<vector<string>>::iterator meld = this->playermeldhand.begin();
							meld != this->playermeldhand.end(); meld++)
						if ((*meld)[0] != (*meld)[1] && (*meld)[1] != (*meld)[2])
							temphand.insert(temphand.end(), meld->begin(), meld->end());
				}
				// check the tempahnd isn't equal to 8
				if (temphand.size() != 8)
					temphand.push_back(this->riverhandlastelement);
				// sort temphand
				Sort(this->sortby, temphand);
				if (temphand[0] == temphand[1] && temphand[2] == temphand[3] &&
						temphand[4] == temphand[5] && temphand[6] == temphand[7]) {
					this->totaltai += 1;
					cout << "四對子      1台" << endl;
					Delay(750);
				}
			}
		};
		// bookmaker
		void BookMaker() {
			if (this->bookmaker) {
				this->totaltai += 1;
				cout << "莊家        1台" << endl;
				Delay(750);
			}
		};
		// tenpai function
		void TenPai() {
			if (this->tenpai) {
				this->totaltai += 1;
				cout << "聽牌        1台" << endl;
				Delay(750);
			}
		};
};

// continue to bookmaker or change to next player function
void ContinueToBookmakerOrChange(int &continuetobookmaker, int winner,
		bool &player1bookmaker, bool &player2bookmaker,
		int tempbookmaker, int &gameturn){
	// if winner equal to bookmaker
	if (winner == tempbookmaker){
		// continuetobookmaker + 1
		continuetobookmaker += 1;
		// set bookmaker to original player
		switch (tempbookmaker){
			case 1:
				player1bookmaker = !player1bookmaker;
				gameturn = 0;
				break;
			case 2:
				player2bookmaker = !player2bookmaker;
				gameturn = 1;
				break;
		}
		// winner isn't bookmaker 
	}else{
		// reset continuetobookmaker
		continuetobookmaker = 0;
		// change bookmaker to next player
		switch(tempbookmaker){
			case 1:
				player2bookmaker = !player2bookmaker;
				gameturn = 1;
				break;
			case 2:
				player1bookmaker = !player1bookmaker;
				gameturn = 0;
				break;
		}
	}
}

// check kong two time function
void CheckKongTwoTime(vector<string> sortby, map<string,string> tile, 
		Player &player,vector<string> &wallhand, vector<string> &riverhand,
		bool &gameloop, bool &top, bool &tsumo, string &eye){
	// declare type
	CheckWin cw;
	CheckMeld cm;
	cw.CheckWin_SetTile(sortby);
	cm.SetTile(tile);
	// try to check concealed or small kong
	for(int i = 0; i < 2;i++){
		cm.CheckConcealedAndSmall(player, wallhand, top);
		// check player is winning by wall tail
		gameloop = cw.CheckWin_Check(player, riverhand);
		// if player is winning
		if (!gameloop){
			// set eye 
			cw.GetEye(eye);
			// set tsumo to true
			tsumo = true;
			// palyer isn't winning
		}
	}
}

// discard and print all hand function
void DiscardAndPrintHand(vector<string> sortby, map<string,string> tile, 
		int &gameround, vector<string> &riverhand, 
		vector<string> wallhand, Player &player1,
		Player player2){
	// declare class	
	bool ismeld = false;
	PrintAll pa;
	// pa class set name and map
	pa.PrintAll_SetNameAndMap(player1.name, player2.name, "牌山", "牌河", tile);
	// increase round and print
	if (player1.bookmaker){
		// print round
		cout << "第 " << gameround << " 巡" << endl;
		gameround += 1;
	}
	// player1 is tenpai
	if (player1.tenpai){
		AutoDisCard(player1.hand, riverhand);
		// player1 isn't tenpai
	}else{
		// print player1hand with number index
		pa.PrintHand(1, player1.hand, true);
		// check player1 is tenpai and declare
		TenPaiCheck(player1, sortby, riverhand);
		// discard from player1hand to riverhand
		DisCard(player1, riverhand);
	}
	// sort player1hand
	Sort(sortby, player1.hand);
	// print all hand
	pa.PrintAll_Print(player1, player2, wallhand, riverhand);
}

// check win or meld function
void WinAndMeldCheck(vector<string> sortby, map<string,string> tile,
		vector<string> &riverhand, vector<string> &wallhand, Player &player, 
		int &gameturn, bool &gameloop, bool nextplayer, bool &top,	bool &tsumo, 
		string &eye, bool &gameflow){
	// declare class
	bool ismeld = false;
	CheckWin cw;
	CheckMeld cm;
	// cw class set tile
	cw.CheckWin_SetTile(sortby);
	// cm class set tile
	cm.SetTile(tile);
	// check player is ron
	gameloop = cw.CheckWin_Check(player, riverhand);
	// player is ron
	if (!gameloop){
		// set eye
		cw.GetEye(eye);
		// set gameturn to player code
		gameturn = player.code - 1;
		// player isn't ron then check meld
	}else{
		// check player has meld with riverhand last
		ismeld = cm.Check(riverhand, player, nextplayer);
		// player can make meld and player isn't tenpai 
		if (ismeld && !player.tenpai){
			// ask player want meld or not
			cm.MeldAskAndSelect();
			// make meld (pong or kong) from playerhand 
			// and riverhand last one to playermeldhand
			cm.BeenMeld(player, riverhand, wallhand, top);
			// playerconcealed is concealed and playermeldhand isn't empty
			if (player.concealed && !player.meldhand.empty())
				// set playerconcealed to false
				player.concealed = false;
			// player has make meld
			if (cm.key == 'p' || cm.key =='k' || cm.key == 'c'){
				// set gameturn to player.code
				gameturn = player.code - 1;
				if(cm.key == 'k'){
					// set winonthewalltail = true mean is konged
					ismeld = true;
					//winonthewalltail = !winonthewalltail;
					// check win after kong
					gameloop = cw.CheckWin_Check(player, riverhand);
					// player is winning
					if (!gameloop){
						// set eye
						cw.GetEye(eye);
						// set tsumo to true
						tsumo = true;
						// player isn't winning
					}else{
						// try to chek the kong again 
						cm.CheckConcealedAndSmall(player, wallhand, top);
						// check win after kong again
						gameloop = cw.CheckWin_Check(player, riverhand);
						// player is winning after kong again
						if (!gameloop){
							// set eye
							cw.GetEye(eye);
							// set tsumo to true
							tsumo = true;
							// player isn't winning after kong again
						}
					}
				}			
			}
			// player can't make meld or player is tenpai
		}else if(nextplayer){
			// draw card
			DrawCard(wallhand, player.hand);
			// check wall is empty mean flow
			if (wallhand.empty()) {
				// forced end
				gameloop = false;
				// make flow
				gameflow = true;
				// wall isn't empty mean no flow
			}else{
				// try to check concealed or small kong
				CheckKongTwoTime(sortby, tile, player, wallhand, riverhand, 
						gameloop, top, tsumo, eye);
				// wall is empty mean flow
				if (wallhand.empty()) {
					gameloop = false;
					gameflow = true;
					// wall isn't empty mean no flow
				} else {
					// check player is tsumo
					gameloop = cw.CheckWin_Check(player, riverhand);
					// player isn't winning(tsumo)
					if (!gameloop) {
						// set eye
						cw.GetEye(eye);
						// set the tsumo bool to false or true
						tsumo = true;
					}
				}
			}
		}
	}
}

int main() {
	// declare type
	// bool
	bool gameloop = false;
	bool tsumo = false;
	bool gameflow = false;
	bool lastwalltile = false;
	bool top = true;
	// int
	int gameturn = 0; // #0 = player1, #1 = player2
	int gameround = 1;
	int winnerortempgameturn = 0;
	int continuetobookmaker = 0;
	int tempbookmaker = 0;
	// map
	map<string, string> tile = {{"1b", "黑將"}, {"2b", "黑士"}, {"3b", "黑象"},
		{"4b", "黑車"}, {"5b", "黑馬"}, {"6b", "黑包"},
		{"7b", "黑卒"}, {"1r", "紅帥"}, {"2r", "紅仕"},
		{"3r", "紅相"}, {"4r", "紅俥"}, {"5r", "紅傌"},
		{"6r", "紅炮"}, {"7r", "紅兵"}};
	// string
	string eye;
	// struct
	Player player1;
	Player player2;
	// vector<string>
	vector<string> sortby{"1b", "2b", "3b", "4b", "5b", "6b", "7b",
		"1r", "2r", "3r", "4r", "5r", "6r", "7r"};
	vector<string> wallhand;
	vector<string> riverhand;
	// class
	PrintAll  pa;
	CheckWin  cw;
	CheckMeld cm;
	Score     s;
	// set type
	// random seed
	srand(unsigned(time(0)));
	// set player name
	player1.name = "玩家1";
	player2.name = "玩家2";
	// set player code
	player1.code = 1;
	player2.code = 2;
	// bookmaker
	player1.bookmaker = true;
	// pa class set name and map
	pa.PrintAll_SetNameAndMap(player1.name, player2.name, "牌山", "牌河", tile);
	// cw class set tile
	cw.CheckWin_SetTile(sortby);
	// cm class set tile
	cm.SetTile(tile);
	// s class set tile
	s.SetTile(tile);
ReStart:
	SetUp(player1, player2, tile, wallhand, sortby);
	// print all hand
	pa.PrintAll_Print(player1, player2, wallhand, riverhand);
	// check whitch player is tsumo(tenho)
	switch(gameturn){
		// player1
		case 0:
			gameloop = cw.CheckWin_Check(player1, riverhand);
			break;
			// player2
		case 1:
			gameloop = cw.CheckWin_Check(player2, riverhand);
			break;
	}
	// if first player is winning 
	if (!gameloop)
		// set eye
		cw.GetEye(eye);
	// first player isn't winning
	else{
		// check first player has concealkong
		switch(gameturn){
			case 0:
				CheckKongTwoTime(sortby, tile, player1, wallhand,
						riverhand, gameloop, top, tsumo, eye);
				break;
			case 1:
				CheckKongTwoTime(sortby, tile, player2, wallhand,
						riverhand, gameloop, top, tsumo, eye);
				break;
		}
	}
	while (gameloop) {
		// reset check meld class
		cm.Reset();
		// change to next player
		gameturn != 1 ? gameturn += 1 : gameturn -= 1;
		// player2 discard and other player check tsumo, ron or meld
		if (gameturn == 0) {
			// player2 discard and printhand
			DiscardAndPrintHand(sortby, tile, gameround, riverhand, wallhand, 
					player2, player1);
			winnerortempgameturn = gameturn;	
			// player1 win and meld check
			WinAndMeldCheck(sortby, tile, riverhand, wallhand, player1, gameturn, 
					gameloop,	true, top, tsumo, eye, gameflow);
			// print all hand
			pa.PrintAll_Print(player1, player2, wallhand, riverhand);
			// player1 discard and other palyer check tsumo, ron or meld
		} else if (gameturn == 1) {
			// player1 discard and printhand
			DiscardAndPrintHand(sortby, tile, gameround, riverhand, wallhand, 
					player1, player2);
			winnerortempgameturn = gameturn;	
			// player2 win and meld check
			WinAndMeldCheck(sortby, tile, riverhand, wallhand, player2, gameturn,
					gameloop,	true, top, tsumo, eye, gameflow);
			// print all hand
			pa.PrintAll_Print(player1, player2, wallhand, riverhand);
		}
	}
	switch (gameturn) {
		case 0:
			s.SetAccordingTo(sortby, player1, riverhand, eye, gameround, gameturn,
					tsumo, lastwalltile, continuetobookmaker);
			winnerortempgameturn = 1;
			break;
		case 1:
			s.SetAccordingTo(sortby, player2, riverhand, eye, gameround, gameturn, 
					tsumo, lastwalltile, continuetobookmaker);
			winnerortempgameturn = 2;
			break;
	}
	s.PrintScore();
	if (player2.bookmaker && winnerortempgameturn != 2) {
		return  0;
	}else{
		for(int i = 6; i > 0; i--){
			cout << i << "秒後開始下一輪" << endl; 
			Delay(1000);
		}
		// set the original bookmaker
		if (player1.bookmaker)
			tempbookmaker = 1;
		else if(player2.bookmaker)
			tempbookmaker = 2;
		// reset value
		// bool
		gameloop = false;
		tsumo = false;
		lastwalltile = false;
		top = true;
		// int
		gameround = 1;
		// string
		eye = "";
		// struct
		player1 = Player();
		player2 = Player();
		// vector<string>
		wallhand.clear();
		riverhand.clear();
		// change bookmaker or continue
		ContinueToBookmakerOrChange(continuetobookmaker, winnerortempgameturn, 
				player1.bookmaker, player2.bookmaker, tempbookmaker, gameturn);
		tempbookmaker = 0;
		winnerortempgameturn = 0;
		// class 
		pa = PrintAll();
		cw = CheckWin();
		cm = CheckMeld();
		s = Score();
		// set struct player name
		player1.name = "玩家1";
		player2.name = "玩家2";
		// set player code
		player1.code = 1;
		player2.code = 2;
		// class setname and tile
		pa.PrintAll_SetNameAndMap(player1.name, player2.name, "牌山", "牌河", tile);
		// cw class set tile
		cw.CheckWin_SetTile(sortby);
		// cm class set tile
		cm.SetTile(tile);
		// s class set tile
		s.SetTile(tile);
		goto ReStart;
	}
}
