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
#define UGLY_EAT_RANGE 330
#define DRONE_MOVE_SPEED 600
#define UGLY_ATTACK_SPEED (int)(DRONE_MOVE_SPEED * 0.9)
#define UGLY_SEARCH_SPEED (int)(UGLY_ATTACK_SPEED / 2)
#define FISH_AVOID_RANGE 600
#define FISH_SWIM_SPEED 200
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
class Drone;

struct Monster {
	int id, x, y, vx=0, vy=0;
	bool foundTarget = false;
	pair<int, int> target={0, 0};
	bool inRange(int x, int y, int range) {
		return (x - this->x) * (x - this->x) + (y - this->y) * (y - this->y) <= range * range;
	}
	bool DroneInRange(Drone &dr);
};
void updateMonsterPos();
void updateMonsterSpeed(map<int, Monster> &DontUpdateThisMnstr);

double calcDist(int x1, int y1, int x2, int y2) {
    return sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
}
map<int, Monster> mnstr;
class Drone {
public:
	int id, x, y, emergency, battery, light, TargetUp = 0, batteryBefore = 30;
	int FirstTx, FirstTy;
	bool hitTarget, lightOn = 0,keepDoing = 0;
	int ClosestFsh = -1, ClosestEnemyFsh = -1;
	bool ScanedAll = false;
	map<int, string> EnemyFshsRdr;
	map<int, string> rdrs;
	map<int, int> fishesScaned;
	Drone():light(0), hitTarget(0){}

	void PrntErr() {
		cerr << "id: " << id << endl;
		cerr << "fishesScaned: " << endl;
		for (auto &i: fishesScaned) cerr << i.first << " " << i.second << endl;
		cerr << "---" << endl;
	}

	bool inRange(int x, int y, int range) {
		return ((x - this->x) * (x - this->x) + (y - this->y) * (y - this->y) <= range * range);
	}
	
	void MoveDrone(pair<int , int> t) {
		if ((light && y < 6400) || rndCnt < 5 || TargetUp) light = 0;
		else if (battery >= 5) light = 1;
		int trgetId = ClosestFsh;
		if (trgetId == -1) trgetId = ClosestEnemyFsh;
		if (hitTarget && ((fishes[trgetId].type == 0 && y < 1000) || (fishes[trgetId].type == 1 && y < 3500)
							|| (fishes[trgetId].type == 2 && y < 5500) )) light = 0;
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
		int targetFsh = ClosestFsh;
		if (Enemy || targetFsh == -1)
			targetFsh = ClosestEnemyFsh;
		if (targetFsh == -1) {
			ScanedAll = true;
			emergencyMove({x, 500});
			return ;
		}
		int tx = fishes[targetFsh].x, ty = fishes[targetFsh].y;
		if (Enemy) {
			if (tx < 5000) tx += 600;
			else tx -= 600;
		}
		if (targetFsh == ClosestFsh && (ty >= y - 500 && ty <= y + 500 && ((tx > x && x >= 8300) || (tx < x && x <= 1700))))
			tx = x, ty = y;
		emergencyMove({tx, ty});
	}

	void displayMove(Drone &EnemyDr) {
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
			if ((EnemyDr.y < y - 570 && EnemyDr.fishesScaned.size() >= fishesScaned.size() - 1) || ( ClosestEnemyFsh!= -1 && keepDoing)) {
				keepDoing  = 1;
				targetWithRdr(enemyFshs, 1);
				return ;
			}
			hitTarget = 1;
			TargetUp = 1;
			// if (ClosestEnemyFsh != -1) {
			// 	targetWithRdr(enemyFshs, 1);
			// 	return ;
			// }
			cerr << "NO TARGET\n";
			return (emergencyMove({x, 500}));
		}

		if (hitTarget) {
			targetWithRdr(fshs, 0);
			return ;
		}
		emergencyMove({FirstTx, 8000});
	}

	bool moveIsGood(int nextX, int nextY, double distance) {
		if (mnstr.empty()) return true;
		for (auto &i: mnstr) {
			if (this->getCollision(i.second, nextX, nextY)) return false;
		}
		int tmpX = x, tmpY = y;
		x = nextX, y = nextY;
		auto TmpMnst = mnstr;
		updateMonsterPos();
		map<int, Monster> khawi;
		updateMonsterSpeed(khawi);
		for (int i = 0; i < 100; i++) {
			bool flag = true;
			double theta = 2 * PI * i / 100;
			int tx = round(x + distance * cos(theta));
			int ty = round(y + distance * sin(theta));
			auto tmp = calcNextLoc(tx, ty);
			tx = tmp.first, ty = tmp.second;
			if (tx < 100 || !ty || tx >= 9900 || ty >= 9900) continue;
			for (auto &j: mnstr) {
				if (this->getCollision(j.second, tx, ty)) {
					flag = false;
					break;
				}
			}
			if (flag) return (mnstr = TmpMnst, x = tmpX, y = tmpY, true);
		}
		for (int i = 0; i < 100; i++) {
			bool flag = true;
			double theta = 2 * PI * i / 100;
			int tx = round(x + 620 * cos(theta));
			int ty = round(y + 620 * sin(theta));
			auto tmp = calcNextLoc(tx, ty);
			tx = tmp.first, ty = tmp.second;
			if (tx < 100 || !ty || tx >= 9900 || ty >= 9900) continue;
			for (auto &j: mnstr) {
				if (this->getCollision(j.second, tx, ty)) {
					flag = false;
					break;
				}
			}
			if (flag) return (mnstr = TmpMnst, x = tmpX, y = tmpY, true);
		}
		mnstr = TmpMnst, x = tmpX, y = tmpY;
		return false;
	}

	void emergencyMove(pair<int, int> T) {
		auto T1 = calcNextLoc(T.first, T.second);
		map<int, pair<int, int>> mvs;
		int tx = T1.first, ty = T1.second;
		double distance = calcDist(x, y, tx, ty);
		if (moveIsGood(tx, ty, distance)) return MoveDrone({tx, ty});
		for (int i = 0; i < 100; i++) {
			double theta = 2 * PI * i / 100;
			tx = round(x + distance * cos(theta));
			ty = round(y + distance * sin(theta));
			auto tmp = calcNextLoc(tx, ty);
			tx = tmp.first, ty = tmp.second;
			if (tx < 100 || !ty || tx >= 9900 || ty >= 9900) continue;
			if (moveIsGood(tx, ty, distance)) mvs[calcDist(tx, ty, T.first, T.second)] = {tx, ty};
    	}
		for (int i = 0; i < 100; i++) {
			double theta = 2 * PI * i / 100;
			tx = round(x + 620 * cos(theta));
			ty = round(y + 620 * sin(theta));
			auto tmp = calcNextLoc(tx, ty);
			tx = tmp.first, ty = tmp.second;
			if (tx < 100 || !ty || tx >= 9900 || ty >= 9900) continue;
			if (moveIsGood(tx, ty, 620)) mvs[calcDist(tx, ty, T.first, T.second)] = {tx, ty};
    	}
		if (mvs.empty()) {
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
bool Monster::DroneInRange(Drone &dr) {
	if (dr.emergency) return false;
	return (dr.inRange(x, y, dr.lightOn ? 2000 : 800));
}

Drone dr1, dr2, EnemyDr1, EnemyDr2;
bool updateUglyTarget (Monster &ugly) {
	vector<Drone> targetableDrones ;
	if (ugly.DroneInRange(dr1)) targetableDrones.push_back(dr1);
	if (ugly.DroneInRange(dr2)) targetableDrones.push_back(dr2);
	if (ugly.DroneInRange(EnemyDr1)) targetableDrones.push_back(EnemyDr1);
	if (ugly.DroneInRange(EnemyDr2)) targetableDrones.push_back(EnemyDr2);
	if (targetableDrones.empty()) {
		ugly.target = {0, 0};
		ugly.foundTarget = false;
		return false;
	}
	vector<pair<int, int>> closests;
	double mn = 1e9;
	for (auto &i: targetableDrones) {
		double dst = calcDist(ugly.x, ugly.y, i.x, i.y);
		if (dst < mn) {
			mn = dst;
			closests.clear();
			closests.push_back({i.x, i.y});
		}
		else if (dst == mn) closests.push_back({i.x, i.y});
	}
	double targetX = 0, targetY = 0;
	for (auto &i: closests) {
		targetX += i.first, targetY += i.second;
	}
	ugly.target = {targetX / closests.size(), targetY / closests.size()};
	ugly.foundTarget = true;
	return true;
}

void updateUglySpeed(Monster &Mnst) {
	updateUglyTarget(Mnst);
	if (Mnst.foundTarget) {
		pair<double, double> attackVec = {Mnst.target.first - Mnst.x, Mnst.target.second - Mnst.y};
		double length = sqrt(attackVec.first * attackVec.first + attackVec.second * attackVec.second);
		if (length > UGLY_ATTACK_SPEED) {
			attackVec.first = attackVec.first / length, attackVec.second = attackVec.second / length;
			attackVec.first *= UGLY_ATTACK_SPEED, attackVec.second *= UGLY_ATTACK_SPEED;
		}
		Mnst.vx = round(attackVec.first), Mnst.vy = round(attackVec.second);
	} else {
		double length = sqrt(Mnst.vx * Mnst.vx + Mnst.vy * Mnst.vy);
		if (length > UGLY_SEARCH_SPEED) {
			Mnst.vx = round(Mnst.vx / length * UGLY_SEARCH_SPEED);
			Mnst.vy = round(Mnst.vy / length * UGLY_SEARCH_SPEED);
		}
		length = sqrt(Mnst.vx * Mnst.vx + Mnst.vy * Mnst.vy);
		if (length) {
			Monster closestMnstr = Mnst;
			int mn = 1e9;
			for (auto &j: mnstr) {
				if (Mnst.id == j.first) continue;
				int dst = calcDist(Mnst.x, Mnst.y, j.second.x, j.second.y);
				if (dst < mn) mn = dst, closestMnstr = j.second;
			}
			if (closestMnstr.id != Mnst.id && mn <= FISH_AVOID_RANGE) {
				pair<double, double> avoid = {closestMnstr.x, closestMnstr.y};
				pair<double, double> avoidDir = {Mnst.x - avoid.first, Mnst.y - avoid.second};
				double length = sqrt(avoidDir.first * avoidDir.first + avoidDir.second * avoidDir.second);
				if (length) {
					avoidDir.first = avoidDir.first / length, avoidDir.second = avoidDir.second / length;
					avoidDir.first *= FISH_SWIM_SPEED, avoidDir.second *= FISH_SWIM_SPEED;
					Mnst.vx = round(avoidDir.first), Mnst.vy = round(avoidDir.second);
				}
			} 
		}
		pair<double, double> nextLoc = {Mnst.x + Mnst.vx, Mnst.y + Mnst.vy};
		if (nextLoc.first < 0 && nextLoc.first < Mnst.x || nextLoc.first > 9999 && nextLoc.first > Mnst.x)
			Mnst.vx = -Mnst.vx;
		if (nextLoc.second < 2500 && nextLoc.second < Mnst.y || nextLoc.second > 9999 && nextLoc.second > Mnst.y)
			Mnst.vy = -Mnst.vy;
	}
}

void updateMonsterSpeed(map<int, Monster> &DontUpdateThisMnstr) {
	for (auto &i: DontUpdateThisMnstr)
		mnstr[i.first] = i.second;
	for (auto &i: mnstr) {
		if (DontUpdateThisMnstr.count(i.first)) continue;
		updateUglySpeed(i.second);
	}
}

void updateMonsterPos() {
	for (auto &i: mnstr) {
		i.second.x += i.second.vx;
		i.second.y += i.second.vy;
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
		if (i.second.type == 0) i.second.y = 3750;
		else if (i.second.type == 1) i.second.y = 6250;
		else if (i.second.type == 2) i.second.y = 8750;
		int MinY = 0, mxY = 0;
		if (i.second.type == 0) mxY = 5000, MinY = 2500;
		else if (i.second.type == 1) mxY = 7500, MinY = 5000;
		else if (i.second.type == 2) mxY = 9999, MinY = 7500;
		if (Tmp2.rdrs[i.first] == "BL" || Tmp2.rdrs[i.first] == "BR" && Tmp2.y <= mxY && Tmp2.y >= MinY)
			i.second.y = ((mxY - Tmp2.y) / 2) + Tmp2.y;
		else if (Tmp1.rdrs[i.first] == "TL" || Tmp1.rdrs[i.first] == "TR" && Tmp1.y <= mxY && Tmp1.y >= MinY)
			i.second.y = Tmp1.y - ((Tmp1.y - MinY) / 2);
		else if (Tmp1.rdrs[i.first] == "BL" || Tmp1.rdrs[i.first] == "BR" && Tmp1.y <= mxY && Tmp1.y >= MinY)
			i.second.y = ((mxY - Tmp1.y) / 2) + Tmp1.y;
		else if (Tmp2.rdrs[i.first] == "TL" || Tmp2.rdrs[i.first] == "TR" && Tmp2.y <= mxY && Tmp2.y >= MinY)
			i.second.y = Tmp2.y - ((Tmp2.y - MinY) / 2);
		
			// if (Tmp2.rdrs[i.first] == "BL" || Tmp2.rdrs[i.first] == "BR")
			// 	i.second.y = ((mxY - Tmp2.y) / 2) + Tmp2.y;
			// else
			// 	i.second.y = Tmp2.y - ((Tmp2.y - Tmp1.y) / 2);
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

void updateClosestIDS(map<int, int> &Creatures, int &dr1Closest, int &dr2Closest) {
	// for (auto &i: fishes) cerr << i.first << " " << i.second.x << " " << i.second.y << endl;
	if (dr1.y == 500) dr1.TargetUp = 0;
	if (dr2.y == 500) dr2.TargetUp = 0;
	int closest1 = getClosestCreature(-1, dr1, Creatures);
	int closest2 = getClosestCreature(-1, dr2, Creatures);
	if (dr1.TargetUp) closest1 = -1;
	if (dr2.TargetUp) closest2 = -1;
	if (closest1 != -1 && closest1 == closest2) {
		if (calcDist(dr1.x, dr1.y, fishes[closest1].x, fishes[closest1].y) < calcDist(dr2.x, dr2.y, fishes[closest2].x, fishes[closest2].y))
			closest2 = getClosestCreature(closest1, dr2, Creatures);
		else
			closest1 = getClosestCreature(closest2, dr1, Creatures);
	}
	// if (dr1.TargetUp) closest1 = -1;
	// if (dr2.TargetUp) closest2 = -1;
	// if (dr1Closest != -1 && Creatures.count(dr1.ClosestFsh)) closest1 = dr1Closest;
	// if (dr2Closest != -1 && Creatures.count(dr2.ClosestFsh)) closest2 = dr2Closest;
	dr1Closest = closest1, dr2Closest = closest2;
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
			if (dr2.batteryBefore - battery == 5) dr2.lightOn = 1;
			else dr2.lightOn = 0;
			dr2.batteryBefore = battery;
			dr2.id = drone_id, dr2.x = drone_x, dr2.y = drone_y, dr2.emergency = emergency, dr2.battery = battery;
		} else {
			if(emergency) {
				for (auto &h:dr1.fishesScaned) fshs[h.first] = h.second;
				dr1.fishesScaned.clear();
			}
			if (dr1.batteryBefore - battery == 5) dr1.lightOn = 1;
			else dr1.lightOn = 0;
			dr1.batteryBefore = battery;
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
		if (drone_id == 0 || drone_id == 3) {
			if (EnemyDr1.batteryBefore - battery == 5) EnemyDr1.lightOn = 1;
			else EnemyDr1.lightOn = 0;
			EnemyDr1.batteryBefore = battery;
			if(emergency) {
				for (auto &h:EnemyDr1.fishesScaned) enemyFshs[h.first] = h.second;
				EnemyDr1.fishesScaned.clear();
			}
			EnemyDr1.id = drone_id, EnemyDr1.x = drone_x, EnemyDr1.y = drone_y, EnemyDr1.emergency = emergency, EnemyDr1.battery = battery;
		} else {
			if (EnemyDr2.batteryBefore - battery == 5) EnemyDr2.lightOn = 1;
			else EnemyDr2.lightOn = 0;
			EnemyDr2.batteryBefore = battery;
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
	dr1.FirstTy = 6600, dr2.FirstTy = 6600;
	dr1.FirstTx = 2000, dr2.FirstTx = 8000;
	set<int> creatureInsideMap;
    while (1) {
		map<int, Monster> DontUpdateThisMnstr;
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
				DontUpdateThisMnstr[creature_id] = {creature_id, creature_x, creature_y, creature_vx, creature_vy};
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
		for (auto it = fishes.begin();it != fishes.end();) {
			if (!creatureInsideMap.count(it->first))
				enemyFshs.erase(it->first),fshs.erase(it->first), it = fishes.erase(it);
			else
				++it;
		}
		// for (auto &i:enemyFshs) cerr << i.first << endl;
		updateMonsterSpeed(DontUpdateThisMnstr);
		// for (auto &i:mnstr) cerr << i.first << " " << i.second.x<< " " << i.second.y << " " << i.second.vx << " " << i.second.vy<<endl;
		calcFishPos(DontUpdateThisFishes);
		updateClosestIDS(fshs, dr1.ClosestFsh, dr2.ClosestFsh);
		updateClosestIDS(enemyFshs, dr1.ClosestEnemyFsh, dr2.ClosestEnemyFsh);
		if (dr1.y >= dr1.FirstTy) dr1.hitTarget = 1;
		if (dr2.y >= dr2.FirstTy) dr2.hitTarget = 1;
		// dr1.PrntErr(), dr2.PrntErr();
		// cerr << "closest creature id " << dr1.id << " " << dr1.ClosestEnemyFsh << endl; 
		// cerr << "closest creature id " << dr2.id << " " << dr2.ClosestEnemyFsh << endl; 
		if (firstIsFirst) 
			dr1.displayMove(EnemyDr1), dr2.displayMove(EnemyDr2);
		else 
			dr2.displayMove(EnemyDr2), dr1.displayMove(EnemyDr1);
		updateMonsterPos();
    }
}