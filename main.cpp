#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <cmath>
#include <chrono>
#include <set>
#define DISTANCE 3000
#define DRONE_HIT_RANGE 200
#define UGLY_EAT_RANGE 304
#include <ctime>   // for time()
using namespace std;
#define PI 3.14159265

map<int, int> fshs, enemyFshs;
set<int> TargetFshsWithRadar;
set<int> Mnstrs;
int rndCnt = 0;
bool firstIsFirst = true;
typedef struct Fish {
	int id, x, y, vx, vy, type;
} Fish;
map<int, Fish> fishes;
struct Monster {
	int id, x, y, vx, vy;
	bool inRange(int x, int y, int range) {
		return (x - this->x) * (x - this->x) + (y - this->y) * (y - this->y) <= range * range;
	}
};

int calcDist(int x1, int y1, int x2, int y2) {
    return round(sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2)));
}
map<int, Monster> mnstr;
class Drone {
public:
	int id, x, y, emergency, battery, light, TargetUp = 0;
	int FirstTx, FirstTy;
	bool hitTarget;
	map<int, string> EnemyFshsRdr;
	map<int, string> rdrs;
	map<int, int> fishesScaned;
	bool ScanedAll = false;
	Drone():light(0), hitTarget(0){}

	void PrntErr() {
		cerr << "id: " << id << endl;
		cerr << "fishesScaned: " << endl;
		for (auto &i: fishesScaned) cerr << i.first << " " << i.second << endl;
		cerr << "---" << endl;
	}

	pair<int, int> TargetXandY(string Dir) {
		int PlusX, MinusX, PlusY, MinusY;
		PlusX = min(x + DISTANCE * cos(PI / 4), (double)9999), MinusX = max(x - DISTANCE * cos(PI / 4), (double)0);
		PlusY = min(y + DISTANCE * sin(PI / 4), (double)9999), MinusY = max(y - DISTANCE * sin(PI / 4), (double)0);
		int Tx, Ty;
		if (Dir == "TR")
			Tx = PlusX, Ty = MinusY;
		else if (Dir == "TL")
			Tx = MinusX, Ty = MinusY;
		else if (Dir == "BL")
			Tx = MinusX, Ty = PlusY;
		else if (Dir == "BR")
			Tx = PlusX, Ty = PlusY;
		else if (Dir == "UP")
			Tx = x, Ty = 0;
		else if (Dir == "DOWN")
			Tx = x, Ty = 9999;
		else if (Dir == "LEFT")
			Tx = 0, Ty = y;
		else if (Dir == "RIGHT")
			Tx = 9999, Ty = y;
		else
			Tx = 0, Ty = 0;
		return (calcNextLoc(Tx, Ty));
	}

	void MoveDrone(pair<int , int> t) {
		if ((light && y < 6550) || rndCnt < 5 || y < 3000) light = 0;
		else if (battery >= 5) light = 1;
		cout << "MOVE " << t.first << " " << t.second << " " << light << endl;
	}

	void targetWithRdr(map<int,int> &TrgetCreatures) {

		int mx = 1e9, id;
		for (auto &i: fishes) {
			if (TrgetCreatures.count(i.first) && calcDist(x, y, i.second.x, i.second.y) < mx) {
				mx = calcDist(x, y, i.second.x, i.second.y);
				id = i.first;
			}
		}

		if (TrgetCreatures.empty()) {
			emergencyMove(TargetXandY("UP"));
			return ;
		}
		emergencyMove({fishes[id].x, fishes[id].y});
	}

	void displayMove() {
		if (y == 500) TargetUp = 0;
		if (ScanedAll) {
			targetWithRdr(enemyFshs);
			return ;
		}
		if (fshs.empty() || fishesScaned.size() >= 5 || TargetUp) {
			hitTarget = 1;
			TargetUp = 1;
			return (emergencyMove({x, 500}));
		}

		if (hitTarget) {
			targetWithRdr(fshs);
			return ;
		}
		emergencyMove({FirstTx, 8000});
	}

	bool moveIsGood(int nextX, int nextY) {
		for (auto &i: mnstr) {
			if (this->getCollision(i.second, nextX, nextY)) return false;
		}
		// auto TmpMnst = mnstr;
		// for (auto &i: TmpMnst) {
		// 	if (i.second.vx + i.second.x < 0 || i.second.vx + i.second.x > 9999)
		// 		i.second.vx = -i.second.vx;
		// 	if (i.second.vy + i.second.y < 0 || i.second.vy + i.second.y > 9999 || i.second.y + i.second.vy < 2500)
		// 		i.second.vy = -i.second.vy;
		// 	i.second.x += i.second.vx;
		// 	i.second.y += i.second.vy;
		// }
		// int tmpX = x, tmpY = y;
		// x = nextX, y = nextY;
		// for (int i = 0; i < 200; i++) {
		// 	double theta = 2 * PI * i / 200;
		// 	int tx = round(x + 601 * cos(theta));
		// 	int ty = round(y + 601 * sin(theta));
		// 	auto tmp = calcNextLoc(tx, ty);
		// 	tx = tmp.first, ty = tmp.second;
		// 	for (auto &j: TmpMnst)
		// 		if (this->getCollision(j.second, tx, ty)) {
		// 			x = tmpX, y = tmpY;
		// 			return false;
		// 		}
		// }
		// x = tmpX, y = tmpY;
		return true;
	}

	void emergencyMove(pair<int, int> T) {
		auto T1 = calcNextLoc(T.first, T.second);
		map<int, pair<int, int>> mvs;
		int tx = T1.first, ty = T1.second;
		if (moveIsGood(tx, ty)) return MoveDrone({tx, ty});
		for (int i = 0; i < 10000; i++) {
			double theta = 2 * PI * i / 10000;
			tx = round(x + 601 * cos(theta));
			ty = round(y + 601 * sin(theta));
			auto tmp = calcNextLoc(tx, ty);
			tx = tmp.first, ty = tmp.second;
			if (moveIsGood(tx, ty)) mvs[calcDist(tx, ty, T.first, T.second)] = {tx, ty};
    	}
		if (mvs.empty()) {
			MoveDrone(TargetXandY("UP"));
			return ;
		}
		MoveDrone(mvs.begin()->second);
	}

	pair<int, int> calcNextLoc(int Tx, int Ty) {
		if (Tx > 9999) Tx = 9999;
		if (Tx < 0) Tx = 0;
		if (Ty > 9999) Ty = 9999;
		if (Ty < 0) Ty = 0;
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
set<int> enemySaved;

void updateMonster(set<int> &tmpMnstr) {
	for (auto &i: mnstr) {
		if (!tmpMnstr.count(i.first)) {
			int speed = calcDist(i.second.x, i.second.y, i.second.x + i.second.vx, i.second.y + i.second.vy);
			i.second.vx *=  270.0 / speed, i.second.vy *= 270.0 / speed;
			if (i.second.vx + i.second.x < 0 || i.second.vx + i.second.x > 9999)
				i.second.vx = -i.second.vx;
			if (i.second.vy + i.second.y < 0 || i.second.vy + i.second.y > 9999 || i.second.y + i.second.vy < 2500)
				i.second.vy = -i.second.vy;
			i.second.x += i.second.vx;
			i.second.y += i.second.vy;
		}
	}
}


void calcFishPos() {
	for (auto &i: fishes) {
		Drone Tmp1 = dr1, Tmp2 = dr2;
		if (Tmp1.x > Tmp2.x) swap(Tmp1, Tmp2);
		if (Tmp1.rdrs[i.first] == "BR" || Tmp1.rdrs[i.first] == "TR") {
			if (Tmp2.rdrs[i.first] == "BR" || Tmp2.rdrs[i.first] == "TR")
				i.second.x = ((9999 - Tmp2.x) / 2) + Tmp2.x;
			else
				i.second.x = Tmp2.x - ((Tmp2.x - Tmp1.x) / 2);
		}
		else
			i.second.x = (Tmp1.x / 2);
	}
	for (auto &i: fishes) {
		Drone Tmp1 = dr1, Tmp2 = dr2;
		if (Tmp1.y > Tmp2.y) swap(Tmp1, Tmp2);
		if (Tmp1.rdrs[i.first] == "BL" || Tmp1.rdrs[i.first] == "BR") {
			if (Tmp2.rdrs[i.first] == "BL" || Tmp2.rdrs[i.first] == "BR")
				i.second.y = ((9999 - Tmp2.y) / 2) + Tmp2.y;
			else
				i.second.y = Tmp2.y - ((Tmp2.y - Tmp1.y) / 2);
		}
		else
			i.second.y = (Tmp1.y / 2);
		if (i.second.type == 0) i.second.y = min(i.second.y, 4999), i.second.y = max(i.second.y, 2499);
		else if (i.second.type == 1) i.second.y = min(i.second.y, 7499), i.second.y = max(i.second.y, 4999);
		else if (i.second.type == 2) i.second.y = min(i.second.y, 9999), i.second.y = max(i.second.y, 7499);
	}
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
        if (type != -1) {
			int y = (type == 0 ? 3750 : type == 1 ? 6250 : 8750);
			fshs[creature_id] = type, fishes[creature_id] = {creature_id, 0, y, 0, 0, type};
		}
        else
            Mnstrs.insert(creature_id);
    }
	enemyFshs = fshs;
	dr1.FirstTy = 7400, dr2.FirstTy = 7400;
	dr1.FirstTx = 2500, dr2.FirstTx = 7500;
    while (1) {
		TargetFshsWithRadar.clear();
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
		if (fshs.empty()) {
			if (dr1.fishesScaned.empty() )
				dr1.ScanedAll = true;
			if (dr2.fishesScaned.empty() )
				dr2.ScanedAll = true;
		}
		else dr2.ScanedAll = dr1.ScanedAll = false;
		if (!dr1.fishesScaned.empty()) dr1.ScanedAll = false;
		if (!dr2.fishesScaned.empty()) dr2.ScanedAll = false;
        int foe_scan_count;
        cin >> foe_scan_count; cin.ignore();
        for (int i = 0; i < foe_scan_count; i++) {
            int creature_id;
            cin >> creature_id; cin.ignore();
			enemyFshs.erase(creature_id);
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
                    for (auto &h:dr2.fishesScaned) fshs[h.first] = h.second;
                    dr2.fishesScaned.clear();
                }
                dr2.id = drone_id, dr2.x = drone_x, dr2.y = drone_y, dr2.emergency = emergency, dr2.battery = battery;
            }
			else {
                if(emergency) {
                    for (auto &h:dr1.fishesScaned) fshs[h.first] = h.second;
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
                if (drone_id == dr1.id ) dr1.fishesScaned[creature_id]=fshs[creature_id];
                else dr2.fishesScaned[creature_id]=fshs[creature_id];
                fshs.erase(creature_id);
            }
        }
		set<int> tmpMnstr;
        int visible_creature_count;
        cin >> visible_creature_count; cin.ignore();
        for (int i = 0; i < visible_creature_count; i++) {
            int creature_id;
            int creature_x;
            int creature_y;
            int creature_vx;
            int creature_vy;
            cin >> creature_id >> creature_x >> creature_y >> creature_vx >> creature_vy; cin.ignore();
            if (Mnstrs.count(creature_id)) {
				Monster mn = {creature_id, creature_x, creature_y, creature_vx, creature_vy};
				tmpMnstr.insert(creature_id);
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
            if (!Mnstrs.count(creature_id)) {
                if (drone_id == dr1.id)
                    dr1.rdrs[creature_id] = radar;
                else
                    dr2.rdrs[creature_id] = radar;
				TargetFshsWithRadar.insert(creature_id);
            }
        }
		for (auto it = fshs.begin();it != fshs.end();) {
			if (!TargetFshsWithRadar.count(it->first))
				it = fshs.erase(it);
			else
				++it;
		}
		calcFishPos();
		if (dr1.y >= dr1.FirstTy) dr1.hitTarget = 1;
		if (dr2.y >= dr2.FirstTy) dr2.hitTarget = 1;
		updateMonster(tmpMnstr);
		if (firstIsFirst) 
			dr1.displayMove(), dr2.displayMove();
		else 
			dr2.displayMove(), dr1.displayMove();
		dr1.PrntErr(), dr2.PrntErr();
    }
}