#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <cmath>
#include <set>

#define DISTANCE 1978

using namespace std;

map<int, pair<int, int>> mnstr1, mnstr2;
map<int, string> RaMnstr1, RaMnstr2;
map<int, int> fshs;
int myX1, myX2, myY1, myY2, myID1, myID2;
set<int> enemies;

string nextR1 = "", nextR2 = "";


int calcDist(int x1, int y1, int x2, int y2) {
    return sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
}

void MoveDrone(int id, int x, int y, string Dir, int l, bool itsEmergency) {

    if (id == myID1) {
        if (nextR1 != "") Dir = nextR1, nextR1 = "";
        else if(itsEmergency) nextR1 = Dir;
    }
    else {
        if (nextR2 != "") Dir = nextR2, nextR2 = "";
        else if(itsEmergency) nextR2 = Dir;
    }
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

void displayMove(int id, vector<string> &rdrs, int x, int y, int l, bool first) {

    if (fshs.empty()) {
        MoveDrone(id, x, y, "UP", l, false);
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
    MoveDrone(id, x, y, Direction, l, false);
}

bool find2Elem(vector<string> &v, string s1, string s2) {
    return (find(v.begin(), v.end(), s1) != v.end() && find(v.begin(), v.end(), s2) != v.end());
}

void emergencyMove(int id, map<int, pair<int, int>> &mnstr, int x, int y, map<int, string> &RaMnstr) {
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
    if (s.empty()) MoveDrone(id, x, y, "UP", 0, true);
    else {
        if (s.size() == 3) {
            if (sup == "BR")  MoveDrone(id, x, y, "BL", 0, true);
            if (sup == "BL")  MoveDrone(id, x, y, "BR", 0, true);
            if (sup == "TL")  MoveDrone(id, x, y, "TR", 0, true);
            if (sup == "TR")  MoveDrone(id, x, y, "TL", 0, true);
        }
        else
            MoveDrone(id, x, y, s[0], 0, true);
    }
} 

int main()
{
    int cnt = 0;
    set<int> fishesScaned1, fishesScaned2;
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
        mnstr1.clear(), mnstr2.clear();
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
            cerr << "EMERGENCY " << emergency << endl;
            if (i) {
                if(emergency) {
                    cerr << "WTF2\n";
                    for (auto &h:fishesScaned2) fshs[h] = cnt;
                    fishesScaned2.clear();
                }
                myID2 = drone_id, myX2 = drone_x, myY2 = drone_y;
            }
            else {
                if(emergency) {
                    cerr << "WTF1\n";
                    for (auto &h:fishesScaned1) fshs[h] = cnt;
                    fishesScaned1.clear();
                }
                myID1 = drone_id, myX1 = drone_x, myY1 = drone_y;
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
            if ((drone_id == myID1 || drone_id == myID2) && fshs.count(creature_id)) {
                if (drone_id == myID1) fishesScaned1.insert(creature_id);
                else fishesScaned2.insert(creature_id);
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
                int dstFromId1 = calcDist(creature_x, creature_y, myX1, myY1);
                int dstFromId2 = calcDist(creature_x, creature_y, myX2, myY2);
                // int dstFromLight1, dstFromLight12;
                // if (!l) {
                // }
                if (dstFromId1 < dstFromId2) mnstr1.insert({creature_id, {creature_x, creature_y}});
                else mnstr2.insert({creature_id, {creature_x, creature_y}});
            }
        }
        int radar_blip_count;
        cin >> radar_blip_count; cin.ignore();
        vector<string> rdrs1, rdrs2;
        for (int i = 0; i < radar_blip_count; i++) {
            int drone_id;
            int creature_id;
            string radar;
            cin >> drone_id >> creature_id >> radar; cin.ignore();
            if ((drone_id == myID1 || drone_id == myID2) && fshs.count(creature_id)) {
                if (drone_id == myID1)
                    rdrs1.push_back(radar);
                else
                    rdrs2.push_back(radar);
            }
            if (fshs.count(creature_id)) fshs[creature_id] = cnt;
            if (enemies.count(creature_id)) {
                if (drone_id == myID1)
                    RaMnstr1[creature_id] = radar;
                else RaMnstr2[creature_id] = radar;
            }
        }
        for (auto it=fshs.begin(); it != fshs.end();) {
            if (it->second != cnt)
                fshs.erase(it), it = fshs.begin();
            else
                ++it;
        }
        if (mnstr1.size()) emergencyMove(myID1, mnstr1, myX1, myY1, RaMnstr1);
        else displayMove(myID1, rdrs1, myX1, myY1, l, 1);

        if (mnstr2.size()) emergencyMove(myID2, mnstr2, myX2, myY2, RaMnstr2);
        else displayMove(myID2, rdrs2, myX2, myY2, l, 0);
        l = !l;
        // for (auto &i:fshs) cerr << "FISHES " << i.first << endl;
    }
}