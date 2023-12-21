#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <cmath>
#include <set>

#define DISTANCE 9248
#define DRONE_HIT_RANGE 200
#define UGLY_EAT_RANGE 300

using namespace std;

map<int, int> fshs;
set<int> enemies;



class Drone {
public:
	int id, x, y, emergency, battery, light;
	map<int, pair<int, int>> mnstr;
	map<int, string> RaMnstr;
	vector<string> rdrs;
	pair<int, int> speed;
	set<int> fishesScaned;
	Drone() {}

	void calculateSpeed(int newX, int newY) {
		speed.first = newX - x;
		speed.second = newY - y;
	}

	void MoveDrone(string Dir, int l) {
    int PlusX, MinusX, PlusY, MinusY;
    PlusX = min(x + DISTANCE, 9999), MinusX = max(x - DISTANCE, 0);
    PlusY = min(y + DISTANCE, 9999), MinusY = max(y - DISTANCE, 0);

    if (Dir == "TR")
        cout << "MOVE "<< PlusX << ' ' << MinusY << ' ' << l<<"\n";
    else if (Dir == "TL")
        cout << "MOVE "<< MinusX << ' ' << MinusY << ' ' << l<<"\n";
    else if (Dir == "BL")
        cout << "MOVE "<< MinusX << ' ' << PlusY << ' ' << l<<"\n";
    else if (Dir == "BR")
        cout << "MOVE "<< PlusX << ' ' << PlusY << ' ' << l<<"\n";
    else
        cout << "MOVE "<< x << " 0 "<< l << "\n";
	}

	void displayMove(int l) {

    if (fshs.empty()) {
        MoveDrone("UP", l);
        return ;
    }
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
    cerr << maxElemnts << " and Direction " << Direction << endl;
    MoveDrone(Direction, l);
	}

	void emergencyMove() {
		for (auto &i: RaMnstr) cerr << i.first << " " << i.second << endl;

		vector<string> s = {"TR", "TL", "BR", "BL"};
		string sup ="";
		for (auto &i : mnstr) {
			cerr << "ID " << id << " i == " << i.first << endl;
			for (auto j = s.begin(); j != s.end(); ++j) {
				if (*j == RaMnstr[i.first]) {
					cerr << "J == " << *j << endl;
					s.erase(j);
					sup = RaMnstr[i.first];
					break;
				}
			}
		}
		if (s.empty()) MoveDrone("UP", 0);
		else {
			if (s.size() == 3) {
				if (sup == "BR")  MoveDrone("BL", 0);
				if (sup == "BL")  MoveDrone("BR", 0);
				if (sup == "TL")  MoveDrone("TR", 0);
				if (sup == "TR")  MoveDrone("TL", 0);
			}
			else
				MoveDrone(s[0], 0);
		}
	}
};


Drone dr1, dr2;


int calcDist(int x1, int y1, int x2, int y2) {
    return sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
}

bool find2Elem(vector<string> &v, string s1, string s2) {
    return (find(v.begin(), v.end(), s1) != v.end() && find(v.begin(), v.end(), s2) != v.end());
}



int main()
{
    int cnt = 0;
    int creature_count;
    cin >> creature_count; cin.ignore();
    for (int i = 0; i < creature_count; i++) {
        int creature_id;
        int color;
        int type;
        cin >> creature_id >> color >> type; cin.ignore();
        if (type != -1)
           fshs[creature_id] = cnt;
        else
            enemies.insert(creature_id);
    }
    // game loop
    int l = 0;
    while (1) {
        dr1.mnstr.clear(), dr2.mnstr.clear();
        cnt++;
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
            if (i) {
                if(emergency) {
                    for (auto &h:dr2.fishesScaned) fshs[h] = cnt;
                    dr2.fishesScaned.clear();
                }
                dr2.id = drone_id, dr2.x = drone_x, dr2.y = drone_y;
            }
            else {
                if(emergency) {
                    cerr << "WTF1\n";
                    for (auto &h:dr1.fishesScaned) fshs[h] = cnt;
                    dr1.fishesScaned.clear();
                }
                dr1.id = drone_id, dr1.x = drone_x, dr1.y = drone_y;
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
            cerr << "VISIBLE CREATURE " << creature_id << endl;
            if (enemies.count(creature_id)) {
                int dstFromId1 = calcDist(creature_x, creature_y, dr1.x, dr1.y);
                int dstFromId2 = calcDist(creature_x, creature_y, dr2.x, dr2.y);
                // int dstFromLight1, dstFromLight12;
                // if (!l) {
                // }
                if (dstFromId1 < dstFromId2) dr1.mnstr.insert({creature_id, {creature_x, creature_y}});
                else dr2.mnstr.insert({creature_id, {creature_x, creature_y}});
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
				fshs[creature_id] = cnt;
            }
            if (enemies.count(creature_id)) {
                if (drone_id == dr1.id)
                    dr1.RaMnstr[creature_id] = radar;
                else dr2.RaMnstr[creature_id] = radar;
            }
        }
        for (auto it=fshs.begin(); it != fshs.end();) {
            if (it->second != cnt)
                fshs.erase(it), it = fshs.begin();
            else
                ++it;
        }
        if (dr1.mnstr.size()) dr1.emergencyMove();
        else dr1.displayMove(l);

        if (dr2.mnstr.size()) dr2.emergencyMove();
        else dr2.displayMove(l);
        l = !l;
        // for (auto &i:fshs) cerr << "FISHES " << i.first << endl;
    }
}