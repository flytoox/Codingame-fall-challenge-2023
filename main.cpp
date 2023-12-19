#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <cmath>
#include <set>

using namespace std;
map<int, pair<int, int>> fshs;
int myX1, myX2, myY1, myY2, myID1, myID2;

/**
 * Score points by scanning valuable fish faster than your opponent.
 **/
int calcDist(int x1, int y1, int x2, int y2) {
    return sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
}

int getId() {
    int mx = 1e9, id;
    for (auto &i: fshs) {
        if (calcDist(myX1, myY1, i.second.first, i.second.second) < mx) {
            mx = calcDist(myX1, myY1, i.second.first, i.second.second);
            id = i.first;
        }
    }
    return id;
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
        cout << "MOVE "<< x << " 0 "<<l<<endl;
    else if (check == tr)
        cout << "MOVE 9999 0 "<<l<<endl;
    else if (check == tl)
        cout << "MOVE 0 0 "<<l<<endl;
    else if (check == bl)
        cout << "MOVE 0 9999 "<<l<<endl;
    else
        cout << "MOVE 9999 9999 "<<l<<endl;
}

int main()
{
    int creature_count;
    cin >> creature_count; cin.ignore();
    for (int i = 0, x = 0, y = 0; i < creature_count; i++, x+=830, y+=830) {
        int creature_id;
        int color;
        int type;
        cin >> creature_id >> color >> type; cin.ignore();
        if (creature_id != -1)
            fshs[creature_id] = {x, y};
    }

    // game loop
    int flg = 1000;
    while (1) {
        int l0=1,l1=1;
        vector<int> indxs;
        int my_score;
        cin >> my_score; cin.ignore();
        int foe_score;
        cin >> foe_score; cin.ignore();
        int my_scan_count;
        cin >> my_scan_count; cin.ignore();
        for (int i = 0; i < my_scan_count; i++) {
            int creature_id;
            cin >> creature_id; cin.ignore();
            cerr << "mySCAN " << creature_id << endl;
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
                myID2 = drone_id, myX2 = drone_x, myY2 = drone_y;
                // if (emergency) l1 = 0;
            }
            else {
                myID1 = drone_id, myX1 = drone_x, myY1 = drone_y;
                // if (emergency) l0 = 0;
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
            if ((drone_id == myID1 || drone_id == myID2) && fshs.count(creature_id))
                fshs.erase(creature_id), cerr << "drone scan count " << creature_id << endl;
            indxs.push_back(creature_id);
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
            cerr << "VISIBLE " << creature_id << endl;
            // if (creature_id == -1) {
            //     emergen = 1;
            // }
        }
        int radar_blip_count;
        cin >> radar_blip_count; cin.ignore();
        vector<string> rdrs1, rdrs2;
        for (int i = 0; i < radar_blip_count; i++) {
            int drone_id;
            int creature_id;
            string radar;
            cin >> drone_id >> creature_id >> radar; cin.ignore();
            if (creature_id != -1 && (drone_id == myID1 || drone_id == myID2) && fshs.count(creature_id)) {
                if (drone_id == myID1)
                    rdrs1.push_back(radar);
                else
                    rdrs2.push_back(radar);
            }
        }
        displayMove(rdrs1, myX1, myY1, l0);
        // displayMove(rdrs2, myX2, myY2, l1);
        cout << "MOVE " << myX2 << " " << flg << " 0\n";
        if (myY2 == 1000) flg = 0;
        else if (myY2 == 0)flg = 1000;

    }
}