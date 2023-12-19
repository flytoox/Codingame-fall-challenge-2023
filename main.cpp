#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <cmath>
#include <set>

using namespace std;
set<int> fshs;
int myX1, myX2, myY1, myY2, myID1, myID2;
set<int> enemies;
/**
 * Score points by scanning valuable fish faster than your opponent.
 **/
int calcDist(int x1, int y1, int x2, int y2) {
    return sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
}

void displayMove(vector<string> &rdrs, int x, int y, int l) {
    int tr=0,tl=1,bl=2,br=3;
    map<string, int> mp;
    for(auto &i: rdrs)
        mp[i]++;
    int check = 0, mn = 1e9;
    for (auto &i: mp) {
        if (i.first == "TR" && calcDist(x, y, 9999, 0) < mn) mn = calcDist(x, y, 9999, 0), check = 0;
        if (i.first == "TL" && calcDist(x, y, 0, 0) < mn) mn = calcDist(x, y, 0, 0), check = 1;
        if (i.first == "BL" && calcDist(x, y, 0, 9999) < mn) mn = calcDist(x, y, 0, 9999), check = 2;
        if (i.first == "BR" && calcDist(x, y, 9999, 9999) < mn) mn = calcDist(x, y, 9999, 9999), check = 3;
    }
    if (fshs.empty())
        cout << "MOVE "<< x << " 0 "<< l << "\n";
    else if (check == tr)
        cout << "MOVE 9999 0 "<<l<<"\n";
    else if (check == tl)
        cout << "MOVE 0 0 " << l << "\n";
    else if (check == bl)
        cout << "MOVE 0 9999 " << l << "\n";
    else
        cout << "MOVE 9999 9999 " << l << "\n";
}

int main()
{
    set<int> fishesScaned1, fishesScaned2;
    int creature_count;
    cin >> creature_count; cin.ignore();
    for (int i = 0; i < creature_count; i++) {
        int creature_id;
        int color;
        int type;
        cin >> creature_id >> color >> type; cin.ignore();
        if (type != -1)
           fshs.insert(creature_id);
        else
            enemies.insert(creature_id);
    }
    // game loop
    int l = 0;
    while (1) {
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
                    for (auto &h:fishesScaned2) cerr << "FISHES SCANED2 " << h << endl;
                    fshs.insert(fishesScaned2.begin(), fishesScaned2.end());
                    fishesScaned2.clear();
                }
                myID2 = drone_id, myX2 = drone_x, myY2 = drone_y;
            }
            else {
                if(emergency) {
                    cerr << "WTF1\n";
                    for (auto &h:fishesScaned1) cerr << "FISHES SCANED1 " << h << endl;
                    fshs.insert(fishesScaned1.begin(), fishesScaned1.end());
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
        cerr << "num drone scan count " << drone_scan_count << endl;
        for (int i = 0; i < drone_scan_count; i++) {
            int drone_id;
            int creature_id;
            cin >> drone_id >> creature_id; cin.ignore();
            if ((drone_id == myID1 || drone_id == myID2) && fshs.count(creature_id)) {
                if (drone_id == myID1) fishesScaned1.insert(creature_id);
                else fishesScaned2.insert(creature_id);
                fshs.erase(creature_id), cerr << "drone scan count " << creature_id << endl;
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
            if (enemies.count(creature_id)) emergency1=1, emergency2=1;
        }
        int radar_blip_count;
        cin >> radar_blip_count; cin.ignore();
        vector<string> rdrs1, rdrs2;
        multiset<pair<int,int>> st;
        for (int i = 0; i < radar_blip_count; i++) {
            int drone_id;
            int creature_id;
            string radar;
            cin >> drone_id >> creature_id >> radar; cin.ignore();
            if (enemies.count(creature_id)) continue;
            if ((drone_id == myID1 || drone_id == myID2) && fshs.count(creature_id)) {
                if (drone_id == myID1)
                    rdrs1.push_back(radar);
                else
                    rdrs2.push_back(radar);
            }
        }
        if (emergency1)
            displayMove(rdrs1, myX1, myY1, 0);
        else
            displayMove(rdrs1, myX1, myY1, l);
        if (emergency2)
            displayMove(rdrs2, myX2, myY2, 0);
        else
            displayMove(rdrs2, myX2, myY2, l);
        l = !l;
        for (auto &i:fshs) cerr << "FISHES " << i << endl;
    }
}