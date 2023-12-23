#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <cmath>
#include <set>
#define DISTANCE 2500
#define DRONE_HIT_RANGE 200
#define UGLY_EAT_RANGE 400
#include <ctime>   // for time()
using namespace std;

map<int, int> fshs;
set<int> enemies;
int rndCnt = 0;
bool firstIsFirst = true;
struct Monster {
	int id, x, y, vx, vy;
	bool inRange(int x, int y, int range) {
		return (x - this->x) * (x - this->x) + (y - this->y) * (y - this->y) <= range * range;
	}
};

int calcDist(int x1, int y1, int x2, int y2) {
    return sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
}
map<int, Monster> mnstr;
class Drone {
public:
	int id, x, y, emergency, battery, light;
	int FirstTx, FirstTy;
	bool hitTarget;
	map<int, string> RaMnstr;
	vector<string> rdrs;
	set<int> fishesScaned;
	Drone():light(0), hitTarget(0){}


	pair<int, int> TargetXandY(string Dir) {
		int PlusX, MinusX, PlusY, MinusY;
		PlusX = min(x + DISTANCE, 9999), MinusX = max(x - DISTANCE, 0);
		PlusY = min(y + DISTANCE, 9999), MinusY = max(y - DISTANCE, 0);
		int Tx, Ty;
		if (Dir == "TR")
			Tx = PlusX, Ty = MinusY;
		else if (Dir == "TL")
			Tx = MinusX, Ty = MinusY;
		else if (Dir == "BL")
			Tx = MinusX, Ty = PlusY;
		else if (Dir == "BR")
			Tx = PlusX, Ty = PlusY;
		else
			Tx = x, Ty = 0;
		return (calcNextLoc(Tx, Ty));
		// return {Tx, Ty};
	}

	void MoveDrone(pair<int , int> t) {
		if (light || rndCnt < 5) light = 0;
		else if (battery >= 5) light = 1;
		cout << "MOVE " << t.first << " " << t.second << " " << this->light << endl;
	}
	void displayMove() {
		if (fshs.empty() || fishesScaned.size() >= 4) {
			emergencyMove({x, 499});
			return ;
		}
		// if (fishesScaned.size() >= 3 && calcDist(x, y, x, 499) < 1000) {
		// 	emergencyMove({x, 0});
		// 	return ;
		// }
		if (hitTarget && !fshs.empty()) {
			map<string, int> mp;
			for(auto &i: rdrs)
				mp[i]++;
			int mn = 1e9, maxElemnts = -1e9;
			string Direction = "";
			for (auto &i: mp) {
				if (i.second >= maxElemnts && i.first == "TR")  Direction = i.first, maxElemnts = i.second;
				if (i.second >= maxElemnts && i.first == "TL")  Direction = i.first, maxElemnts = i.second;
				if (i.second >= maxElemnts && i.first == "BL")  Direction = i.first, maxElemnts = i.second;
				if (i.second >= maxElemnts && i.first == "BR")  Direction = i.first, maxElemnts = i.second;
			}
			if (mnstr.empty()) {
				MoveDrone(TargetXandY(Direction));
				return ;
			}
			emergencyMove(TargetXandY(Direction));
			return ;
		}
		// map<string, int> mp;
		// for(auto &i: rdrs)
		// 	mp[i]++;
		// int mn = 1e9, maxElemnts = -1e9;
		// string Direction = "";
		// for (auto &i: mp) {
		// 	if (i.second >= maxElemnts && i.first == "TR")  Direction = i.first, maxElemnts = i.second;
		// 	if (i.second >= maxElemnts && i.first == "TL")  Direction = i.first, maxElemnts = i.second;
		// 	if (i.second >= maxElemnts && i.first == "BL")  Direction = i.first, maxElemnts = i.second;
		// 	if (i.second >= maxElemnts && i.first == "BR")  Direction = i.first, maxElemnts = i.second;
		// }
		// for (auto &i : mnstr) {
		// 	//cerr << "MNSTR " << i.second.id << " " << i.second.x << " " << i.second.y << " " << i.second.vx << " " << i.second.vy << endl;
		// }
		// //cerr << "DIRECTION " << Direction << " Elements " << maxElemnts << endl;
		// if (this->mnstr.empty()) {
		// 	MoveDrone(TargetXandY(Direction));
		// 	return ;
		// }
		// emergencyMove(TargetXandY(Direction));
		if (mnstr.empty()) {
			MoveDrone({FirstTx, FirstTy});
			return ;
		}
		emergencyMove({FirstTx, FirstTy});
	}

	bool moveIsGood(int nextX, int nextY) {
		for (auto &i: mnstr) {
			if (this->getCollision(i.second, nextX, nextY)) return false;
		}
		return true;
	}

	void emergencyMove(pair<int, int> T) {
		T = calcNextLoc(T.first, T.second);
		map<int, pair<int, int>> mvs;
		int cnt = 0;
		int tx = T.first, ty = T.second;
		if (moveIsGood(tx, ty)) return MoveDrone({tx, ty});
		for (;cnt < 20000; cnt++) {
			if (moveIsGood(tx, ty)) mvs[calcDist(tx, ty, T.first, T.second)] = {tx, ty};
			tx = rand() % 10000, ty = rand() % 10000;
			auto tmp = calcNextLoc(tx, ty);
			tx = tmp.first, ty = tmp.second;
		}
		if (mvs.empty()) {
			MoveDrone(TargetXandY("UP"));
			return ;
		}
		MoveDrone(mvs.begin()->second);
	}

	pair<int, int> calcNextLoc(int Tx, int Ty) {
		int dx = Tx - x, dy = Ty - y;
		double dist = sqrt(dx * dx + dy * dy);
		if (dist > 600) {
			dx = round(dx * 600.0 / dist);
			dy = round(dy * 600.0 / dist);
		}
		return {x + dx, y + dy};
	}


	bool getCollision(Monster &mn, int nextX, int nextY) {

		pair<int, int> nextLoc = this->calcNextLoc(nextX, nextY);
		double DroneSpeedX = nextLoc.first - this->x, DroneSpeedY = nextLoc.second - this->y;

		if (mn.inRange(nextLoc.first, nextLoc.second, DRONE_HIT_RANGE + UGLY_EAT_RANGE)) return true;
		if (!mn.vx && !mn.vy && !DroneSpeedX && !DroneSpeedY) return false;

		double x = mn.x, y = mn.y, ux = this->x, uy = this->y;
		double x2 = x - ux, y2 = y - uy;
		double r2 = UGLY_EAT_RANGE + DRONE_HIT_RANGE;

		double vx2 = mn.vx - DroneSpeedX, vy2 = mn.vy - DroneSpeedY;
		double a = vx2 * vx2 + vy2 * vy2;
		if ( a <= 0.0) return false;
		double b = 2.0 * (x2 * vx2 + y2 * vy2);
		double c = x2 * x2 + y2 * y2 - r2 * r2;
		double delta = b * b - 4.0 * a * c;
		if (delta < 0.0) return false;
		double t = (-b - sqrt(delta)) / (2.0 * a);
		if (t <= 0.0) return false; 
		if (t > 1.0) return false;
		return true;
	}
};


Drone dr1, dr2;


bool find2Elem(vector<string> &v, string s1, string s2) {
    return (find(v.begin(), v.end(), s1) != v.end() && find(v.begin(), v.end(), s2) != v.end());
}


int main()
{
	srand(time(0));
    int creature_count;
    cin >> creature_count; cin.ignore();
    for (int i = 0; i < creature_count; i++) {
        int creature_id;
        int color;
        int type;
        cin >> creature_id >> color >> type; cin.ignore();
        if (type != -1)
           fshs[creature_id] = rndCnt;
        else
            enemies.insert(creature_id);
    }
	dr1.FirstTx = 1800, dr2.FirstTx = 8200;
	dr1.FirstTy = 8200, dr2.FirstTy = 8200;
    // game loop
    int l = 0;
    while (1) {
		map<int, Monster> tmpMnstr;
		dr1.RaMnstr.clear(), dr2.RaMnstr.clear();
		dr1.rdrs.clear(), dr2.rdrs.clear();
        rndCnt++;
        int  emergency1=0,emergency2=0;
        int my_score;
        cin >> my_score; cin.ignore();
        int foe_score;
        cin >> foe_score; cin.ignore();
        int my_scan_count;
        cin >> my_scan_count; cin.ignore();
        for (int i = 0; i < my_scan_count; i++) {
            int creature_id;
            cin >> creature_id; cin.ignore();
			if (dr1.fishesScaned.count(creature_id)) {
				dr1.fishesScaned.erase(creature_id);
				fshs.erase(creature_id);
			}
			if (dr2.fishesScaned.count(creature_id)) {
				dr2.fishesScaned.erase(creature_id);
				fshs.erase(creature_id);
			}
        }
        int foe_scan_count;
        cin >> foe_scan_count; cin.ignore();
        for (int i = 0; i < foe_scan_count; i++) {
            int creature_id;
            cin >> creature_id; cin.ignore();
        }
        int my_drone_count;
        cin >> my_drone_count; cin.ignore();
        for (int i = 0; i < my_drone_count; i++) {
            int drone_id;
            int drone_x;
            int drone_y;
            int emergency;
            int battery;
            cin >> drone_id >> drone_x >> drone_y >> emergency >> battery; cin.ignore();
			if (drone_id == 1 || drone_id == 2) {
				if (!i) firstIsFirst = false;
                if(emergency) {
                    for (auto &h:dr2.fishesScaned) fshs[h] = rndCnt;
                    dr2.fishesScaned.clear();
                }
                dr2.id = drone_id, dr2.x = drone_x, dr2.y = drone_y, dr2.emergency = emergency, dr2.battery = battery;
            }
			else {
                if(emergency) {
                    for (auto &h:dr1.fishesScaned) fshs[h] = rndCnt;
                    dr1.fishesScaned.clear();
                }
                dr1.id = drone_id, dr1.x = drone_x, dr1.y = drone_y, dr1.emergency = emergency, dr1.battery = battery;
            }
        }
        int foe_drone_count;
        cin >> foe_drone_count; cin.ignore();
        for (int i = 0; i < foe_drone_count; i++) {
            int drone_id;
            int drone_x;
            int drone_y;
            int emergency;
            int battery;
            cin >> drone_id >> drone_x >> drone_y >> emergency >> battery; cin.ignore();
        }
        int drone_scan_count;
        cin >> drone_scan_count; cin.ignore();
        for (int i = 0; i < drone_scan_count; i++) {
            int drone_id;
            int creature_id;
            cin >> drone_id >> creature_id; cin.ignore();
            if ((drone_id == dr1.id || drone_id == dr2.id) && fshs.count(creature_id)) {
                if (drone_id == dr1.id) dr1.fishesScaned.insert(creature_id);
                else dr2.fishesScaned.insert(creature_id);
                fshs.erase(creature_id);
            }
        }
        int visible_creature_count;
        cin >> visible_creature_count; cin.ignore();
        for (int i = 0; i < visible_creature_count; i++) {
            int creature_id;
            int creature_x;
            int creature_y;
            int creature_vx;
            int creature_vy;
            cin >> creature_id >> creature_x >> creature_y >> creature_vx >> creature_vy; cin.ignore();
            if (enemies.count(creature_id)) {
				Monster mn = {creature_id, creature_x, creature_y, creature_vx, creature_vy};
				tmpMnstr[creature_id] = mn;
				mnstr[creature_id] = mn;
            }
        }
        int radar_blip_count;
        cin >> radar_blip_count; cin.ignore();
        for (int i = 0; i < radar_blip_count; i++) {
            int drone_id;
            int creature_id;
            string radar;
            cin >> drone_id >> creature_id >> radar; cin.ignore();
            if (fshs.count(creature_id)) {
                if (drone_id == dr1.id)
                    dr1.rdrs.push_back(radar);
                else
                    dr2.rdrs.push_back(radar);
				fshs[creature_id] = rndCnt;
            }
            // if (enemies.count(creature_id)) {
            //     if (drone_id == dr1.id)
            //         dr1.RaMnstr[creature_id] = radar;
            //     else dr2.RaMnstr[creature_id] = radar;
            // }
        }
        for (auto it=fshs.begin(); it != fshs.end();) {
            if (it->second != rndCnt)
                fshs.erase(it), it = fshs.begin();
            else
                ++it;
        }
		if (dr1.x == dr1.FirstTx && dr1.y == dr1.FirstTy) dr1.hitTarget = 1;
		if (dr2.x == dr2.FirstTx && dr2.y == dr2.FirstTy) dr2.hitTarget = 1;
		for (auto &i: mnstr) {
			if (tmpMnstr.count(i.first))
				continue;
			if (i.second.vx + i.second.x < 0)
				i.second.vx = -i.second.vx;
			if (i.second.vy + i.second.y < 0)
				i.second.vy = -i.second.vy;
			i.second.x += i.second.vx;
			i.second.y += i.second.vy;
		}
		if (firstIsFirst) {
			dr1.displayMove();
			dr2.displayMove();
		}
		else {
			dr2.displayMove();
			dr1.displayMove();
		}
    }
}