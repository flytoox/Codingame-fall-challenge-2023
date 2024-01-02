#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <cmath>
#include <chrono>
#include <set>
#define DRONE_HIT_RANGE 200
#define UGLY_EAT_RANGE 305
using namespace std;
#define PI 3.14159265358979323846

map<int, int> fshs, enemyFshs;
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
	int ClosestFsh = -1, ClosestEnemyFsh = -1;
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

	void MoveDrone(pair<int , int> t) {
		if ((light && y < 5800) || rndCnt < 5 || y < 3000) light = 0;
		else if (battery >= 5) light = 1;
		cout << "MOVE " << t.first << " " << t.second << " " << light << endl;
	}

	void targetWithRdr(map<int,int> &TrgetCreatures, bool Enemy) {

		// int mx = 1e9, id = -1;
		// for (auto &i: fishes) {
		// 	if (TrgetCreatures.count(i.first) && calcDist(x, y, i.second.x, i.second.y) < mx) {
		// 		mx = calcDist(x, y, i.second.x, i.second.y);
		// 		id = i.first;
		// 	}
		// }
		// ClosestFsh = id;
		if (TrgetCreatures.empty() || ClosestFsh == -1) {
			ScanedAll = true;
			emergencyMove({x, 500});
			return ;
		}
		int tx = fishes[ClosestFsh].x, ty = fishes[ClosestFsh].y;
		if (Enemy) {
			if (tx < 5000) tx += 500;
			else tx -= 500;
		}
		emergencyMove({tx, ty});
	}

	void displayMove() {
		if (emergency) {
			cout << "MOVE 0 0 0 GAY DRONE\n";
			return ;
		}
		if (y == 500) TargetUp = 0;
		if (ScanedAll) {
			targetWithRdr(enemyFshs, 1);
			return ;
		}
		if (fshs.empty() || fishesScaned.size() >= 5 || TargetUp) {
			hitTarget = 1;
			TargetUp = 1;
			return (emergencyMove({x, 500}));
		}

		if (hitTarget) {
			targetWithRdr(fshs, 0);
			return ;
		}
		emergencyMove({FirstTx, 8000});
	}

	bool moveIsGood(int nextX, int nextY) {
		if (mnstr.empty()) return true;
		for (auto &i: mnstr) {
			if (this->getCollision(i.second, nextX, nextY)) return false;
		}
		int tmpX = x, tmpY = y;
		x = nextX, y = nextY;
		auto TmpMnst = mnstr;
		for (auto &i: TmpMnst) {
			if (i.second.vx + i.second.x < 0 || i.second.vx + i.second.x > 9999)
				i.second.vx = -i.second.vx;
			if (i.second.vy + i.second.y < 0 || i.second.vy + i.second.y > 9999 || i.second.y + i.second.vy < 2500)
				i.second.vy = -i.second.vy;
			i.second.x += i.second.vx;
			i.second.y += i.second.vy;
			double Xx = x - i.second.x, Yy = y - i.second.y;
			double length = sqrt(Xx * Xx + Yy * Yy);
			if (length > 540)
				Xx = Xx / length, Yy = Yy / length, i.second.vx = round(Xx * 540), i.second.vy = round(Yy * 540);
			else
				i.second.vx = round(Xx), i.second.vy = round(Yy);
		}
		for (int i = 0; i < 300; i++) {
			bool flag = true;
			double theta = 2 * PI * i / 300;
			int tx = round(x + 601 * cos(theta));
			int ty = round(y + 601 * sin(theta));
			auto tmp = calcNextLoc(tx, ty);
			tx = tmp.first, ty = tmp.second;
			for (auto &j: TmpMnst) {
				if (this->getCollision(j.second, tx, ty)) {
					flag = false;
					break;
				}
			}
			if (flag) return (x = tmpX, y = tmpY, true);
		}
		x = tmpX, y = tmpY;	
		return false;
	}

	void emergencyMove(pair<int, int> T) {
		auto T1 = calcNextLoc(T.first, T.second);
		map<int, pair<int, int>> mvs;
		int tx = T1.first, ty = T1.second;
		if (moveIsGood(tx, ty)) return MoveDrone({tx, ty});
		for (int i = 0; i < 300; i++) {
			double theta = 2 * PI * i / 300;
			tx = round(x + 601 * cos(theta));
			ty = round(y + 601 * sin(theta));
			auto tmp = calcNextLoc(tx, ty);
			tx = tmp.first, ty = tmp.second;
			if (tx < 20 || !ty || tx >= 9980 || ty >= 9980) continue;
			if (moveIsGood(tx, ty)) mvs[calcDist(tx, ty, T.first, T.second)] = {tx, ty};
    	}
		if (mvs.empty()) {
			// cerr << "NO WAY OUT\n";
			MoveDrone({x, 500});
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
		double dist = sqrt((dx * dx) + (dy * dy));
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

Drone dr1, dr2, EnemyDr1, EnemyDr2;

void updateMonster() {
	for (auto &i: mnstr) {
		int speed = calcDist(i.second.x, i.second.y, i.second.x + i.second.vx, i.second.y + i.second.vy);
		if (i.second.vx + i.second.x < 0 || i.second.vx + i.second.x > 9999)
			i.second.vx = -i.second.vx;
		if (i.second.vy + i.second.y < 0 || i.second.vy + i.second.y > 9999 || i.second.y + i.second.vy < 2500)
			i.second.vy = -i.second.vy;
		i.second.x += i.second.vx;
		i.second.y += i.second.vy;
		if (speed)
			i.second.vx *=  round(270.0 / speed), i.second.vy *= round(270.0 / speed);
	}
}

void calcFishPos(set<int> &DontUpdateThisFishes) {
	Drone Tmp1 = dr1, Tmp2 = dr2;
	if (Tmp1.x > Tmp2.x) swap(Tmp1, Tmp2);
	for (auto &i: fishes) {
		if (DontUpdateThisFishes.count(i.first)) continue;
		if (Tmp1.rdrs[i.first] == "BR" || Tmp1.rdrs[i.first] == "TR") {
			if (Tmp2.rdrs[i.first] == "BR" || Tmp2.rdrs[i.first] == "TR")
				i.second.x = ((9999 - Tmp2.x) / 2) + Tmp2.x;
			else
				i.second.x = Tmp2.x - ((Tmp2.x - Tmp1.x) / 2);
		}
		else
			i.second.x = (Tmp1.x / 2);
	}
	if (Tmp1.y > Tmp2.y) swap(Tmp1, Tmp2);
	for (auto &i: fishes) {
		if (DontUpdateThisFishes.count(i.first)) continue;
		int mxY = 0;
		if (i.second.type == 0) mxY = 5000;
		else if (i.second.type == 1) mxY = 7500;
		else if (i.second.type == 2) mxY = 9999;
		if (Tmp1.rdrs[i.first] == "BL" || Tmp1.rdrs[i.first] == "BR") {
			if (Tmp2.rdrs[i.first] == "BL" || Tmp2.rdrs[i.first] == "BR")
				i.second.y = ((mxY - Tmp2.y) / 2) + Tmp2.y;
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

int getClosestCreature(int except, Drone &dr, map<int, int> &Creatures) {
	int mn = 1e9, id = -1;
	for (auto &i: Creatures) {
		if (i.first == except) continue;
		int dst = calcDist(dr.x, dr.y, fishes[i.first].x, fishes[i.first].y);
		if (dst < mn) mn = dst, id = i.first;
	}
	return id;

}

void updateClosestIDS() {
	// for (auto &i: fishes) cerr << i.first << " " << i.second.x << " " << i.second.y << endl;
	if (dr1.y == 500) dr1.TargetUp = 0;
	if (dr2.y == 500) dr2.TargetUp = 0;
	int closest1 = getClosestCreature(-1, dr1, fshs);
	int closest2 = getClosestCreature(-1, dr2, fshs);
	if (dr1.TargetUp) closest1 = -1;
	if (dr2.TargetUp) closest2 = -1;
	if (closest1 != -1 && closest1 == closest2) {
		if (calcDist(dr1.x, dr1.y, fishes[closest1].x, fishes[closest1].y) < calcDist(dr2.x, dr2.y, fishes[closest2].x, fishes[closest2].y))
			closest2 = getClosestCreature(closest1, dr2, fshs);
		else
			closest1 = getClosestCreature(closest2, dr1, fshs);
	}
	if (closest1 == -1) getClosestCreature(-1, dr1, enemyFshs);
	if (closest2 == -1) getClosestCreature(-1, dr2, enemyFshs);
	if (dr1.TargetUp) closest1 = -1;
	if (dr2.TargetUp) closest2 = -1;
	dr1.ClosestFsh = closest1, dr2.ClosestFsh = closest2;
}

void cinScore() {
	int my_score;
	cin >> my_score; cin.ignore();
	int foe_score;
	cin >> foe_score; cin.ignore();

}
void my_scan_count() {
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
}

void foe_scan_count() {
	int foe_scan_count;
	cin >> foe_scan_count; cin.ignore();
	for (int i = 0; i < foe_scan_count; i++) {
		int creature_id;
		cin >> creature_id; cin.ignore();
		if (EnemyDr1.fishesScaned.count(creature_id)) {
			EnemyDr1.fishesScaned.erase(creature_id);
			enemyFshs.erase(creature_id);
		}
		if (EnemyDr2.fishesScaned.count(creature_id)) {
			EnemyDr2.fishesScaned.erase(creature_id);
			enemyFshs.erase(creature_id);
		}
	}
}

void my_drone_count() {
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
		} else {
			if(emergency) {
				for (auto &h:dr1.fishesScaned) fshs[h.first] = h.second;
				dr1.fishesScaned.clear();
			}
			dr1.id = drone_id, dr1.x = drone_x, dr1.y = drone_y, dr1.emergency = emergency, dr1.battery = battery;
		}
	}
}

void foe_drone_count() {
	int foe_drone_count;
	cin >> foe_drone_count; cin.ignore();
	for (int i = 0; i < foe_drone_count; i++) {
		int drone_id;
		int drone_x;
		int drone_y;
		int emergency;
		int battery;
		cin >> drone_id >> drone_x >> drone_y >> emergency >> battery; cin.ignore();
		if (!i) {
			if(emergency) {
				for (auto &h:EnemyDr1.fishesScaned) enemyFshs[h.first] = h.second;
				EnemyDr1.fishesScaned.clear();
			}
			EnemyDr1.id = drone_id, EnemyDr1.x = drone_x, EnemyDr1.y = drone_y, EnemyDr1.emergency = emergency, EnemyDr1.battery = battery;
		} else {
			if(emergency) {
				for (auto &h:EnemyDr2.fishesScaned) enemyFshs[h.first] = h.second;
				EnemyDr2.fishesScaned.clear();
			}
			EnemyDr2.id = drone_id, EnemyDr2.x = drone_x, EnemyDr2.y = drone_y, EnemyDr2.emergency = emergency, EnemyDr2.battery = battery;
		}
	}
}

void drone_scan_count() {
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
		else if ((drone_id == EnemyDr1.id || drone_id == EnemyDr2.id) && enemyFshs.count(creature_id)) {
			if (drone_id == EnemyDr1.id ) EnemyDr1.fishesScaned[creature_id]=enemyFshs[creature_id];
			else EnemyDr2.fishesScaned[creature_id]=enemyFshs[creature_id];
			enemyFshs.erase(creature_id);
		}
	}
}

int main()
{
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
	dr1.FirstTy = 6000, dr2.FirstTy = 6000;
	dr1.FirstTx = 2000, dr2.FirstTx = 8000;
	set<int> creatureInsideMap;
    while (1) {
		updateMonster();
		creatureInsideMap.clear();
		dr1.rdrs.clear(), dr2.rdrs.clear();
        rndCnt++;
        cinScore();
        my_scan_count();
		if (fshs.empty()) {
			if (dr1.fishesScaned.empty() )
				dr1.ScanedAll = true;
			if (dr2.fishesScaned.empty() )
				dr2.ScanedAll = true;
		}
		if (!dr1.fishesScaned.empty()) dr1.ScanedAll = false;
		if (!dr2.fishesScaned.empty()) dr2.ScanedAll = false;
		foe_scan_count();
        my_drone_count();
        foe_drone_count();
        drone_scan_count();
		set<int> DontUpdateThisFishes;
        int visible_creature_count;
        cin >> visible_creature_count; cin.ignore();
        for (int i = 0; i < visible_creature_count; i++) {
            int creature_id;
            int creature_x;
            int creature_y;
            int creature_vx;
            int creature_vy;
            cin >> creature_id >> creature_x >> creature_y >> creature_vx >> creature_vy; cin.ignore();
            if (Mnstrs.count(creature_id))
				mnstr[creature_id] = {creature_id, creature_x, creature_y, creature_vx, creature_vy};
			else {
				DontUpdateThisFishes.insert(creature_id);
				fishes[creature_id].x = creature_x;
				fishes[creature_id].y = creature_y;
				fishes[creature_id].vx = creature_vx;
				fishes[creature_id].vy = creature_vy;
			}
        }
        int rdrCount;
        cin >> rdrCount; cin.ignore();
        for (int i = 0; i < rdrCount; i++) {
            int drone_id;
            int creature_id;
            string radar;
            cin >> drone_id >> creature_id >> radar; cin.ignore();
            if (!Mnstrs.count(creature_id)) {
                if (drone_id == dr1.id)
                    dr1.rdrs[creature_id] = radar;
                else
                    dr2.rdrs[creature_id] = radar;
				creatureInsideMap.insert(creature_id);
            }
        }
		for (auto it = fshs.begin();it != fshs.end();) {
			if (!creatureInsideMap.count(it->first))
				enemyFshs.erase(it->first),fishes.erase(it->first), it = fshs.erase(it);
			else
				++it;
		}
		calcFishPos(DontUpdateThisFishes);
		updateClosestIDS();
		if (dr1.y >= dr1.FirstTy) dr1.hitTarget = 1;
		if (dr2.y >= dr2.FirstTy) dr2.hitTarget = 1;
		// dr1.PrntErr(), dr2.PrntErr();
		// cerr << "closest creature id " << dr1.id << " " << dr1.ClosestFsh << endl; 
		// cerr << "closest creature id " << dr2.id << " " << dr2.ClosestFsh << endl; 
		if (firstIsFirst) 
			dr1.displayMove(), dr2.displayMove();
		else 
			dr2.displayMove(), dr1.displayMove();
    }
}