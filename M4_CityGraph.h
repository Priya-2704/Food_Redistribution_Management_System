// M4_CityGraph.h
// Member 4 - City Zone Map, Delivery Route and Simulated Clock
// Data Structures : Graph Adjacency Matrix + Singly Linked List (waypoints)
// Feature F5 : Multi Stop Route      Feature F8 : Simulated Clock
// Run alone :  g++ -x c++ -DM4_STANDALONE M4_CityGraph.h -o m4.exe
#ifndef M4_CITYGRAPH_H
#define M4_CITYGRAPH_H
#include <iostream>
#include <iomanip>
#include <cstring>
#include <cstdlib>
using namespace std;
#ifndef TOTAL_ZONES
#define TOTAL_ZONES 6
#endif
#define VAN_SPEED 4     // km the van covers in one hour in city traffic

// ============== PART A : C DATA STRUCTURE PART ==============
// 1) GRAPH as an adjacency matrix, dist[a][b] = km between zone a and b

struct CityGraph {
    int dist[TOTAL_ZONES][TOTAL_ZONES];
    char zoneName[TOTAL_ZONES][25];
};

// fill the zone names and the distance table
void initGraph(struct CityGraph *g) {
    strcpy(g->zoneName[0], "Clement Town");
    strcpy(g->zoneName[1], "Rajpur Road");
    strcpy(g->zoneName[2], "Prem Nagar");
    strcpy(g->zoneName[3], "ISBT");
    strcpy(g->zoneName[4], "Sahastradhara");
    strcpy(g->zoneName[5], "Ballupur");
    int table[6][6] = {
        {  0,  4,  7, 10,  6, 12 },
        {  4,  0,  5,  8,  9, 14 },
        {  7,  5,  0,  6, 11, 10 },
        { 10,  8,  6,  0,  7,  9 },
        {  6,  9, 11,  7,  0,  8 },
        { 12, 14, 10,  9,  8,  0 }
    };
    for (int i = 0; i < TOTAL_ZONES; i++)
        for (int j = 0; j < TOTAL_ZONES; j++)
            g->dist[i][j] = table[i][j];
}

// O(1), one index, no searching
int getDistance(struct CityGraph *g, int from, int to) {
    if (from < 0 || from >= TOTAL_ZONES || to < 0 || to >= TOTAL_ZONES) return -1;
    return g->dist[from][to];
}

char* getZoneName(struct CityGraph *g, int zone) { return g->zoneName[zone]; }

// change km into travel hours
int getTravelTime(struct CityGraph *g, int from, int to) {
    int km = getDistance(g, from, to);
    if (km <= 0) return 0;
    int hours = km / VAN_SPEED;
    if (hours < 1) hours = 1;       // any trip takes at least one hour
    return hours;
}

// scan one row of the matrix for the closest other zone
int findNearestZone(struct CityGraph *g, int from) {
    int best = -1, bestDist = 9999;
    for (int j = 0; j < TOTAL_ZONES; j++) {
        if (j == from) continue;
        if (g->dist[from][j] < bestDist) { bestDist = g->dist[from][j];  best = j; }
    }
    return best;
}

void displayZones(struct CityGraph *g) {
    cout << "\n  ---- CITY ZONES ----" << endl;
    for (int i = 0; i < TOTAL_ZONES; i++)
        cout << "  Zone " << i << " : " << g->zoneName[i] << endl;
}

// print the whole 2d table
void displayMatrix(struct CityGraph *g) {
    cout << "\n  ---- DISTANCE MATRIX (km) ----" << endl;
    cout << "        ";
    for (int j = 0; j < TOTAL_ZONES; j++) cout << setw(4) << j;
    cout << endl;
    for (int i = 0; i < TOTAL_ZONES; i++) {
        cout << "  Z" << i << "   ";
        for (int j = 0; j < TOTAL_ZONES; j++) cout << setw(4) << g->dist[i][j];
        cout << "   " << g->zoneName[i] << endl;
    }
}

// 2) SINGLY LINKED LIST - the driver's route, one node is one stop

struct Waypoint {
    int zone;
    int distFromPrev;
    struct Waypoint *next;
};

struct Route {
    struct Waypoint *head;
    int stops;
    int totalKm;
};

void initRoute(struct Route *r) { r->head = NULL;  r->stops = 0;  r->totalKm = 0; }

// add one stop at the end of the route
void addWaypoint(struct Route *r, int zone, int dist) {
    struct Waypoint *w = (struct Waypoint *) malloc(sizeof(struct Waypoint));
    w->zone = zone;
    w->distFromPrev = dist;
    w->next = NULL;
    if (r->head == NULL) {
        r->head = w;
    } else {
        struct Waypoint *p = r->head;
        while (p->next != NULL) p = p->next;    // walk to the last node
        p->next = w;
    }
    r->stops++;
    r->totalKm = r->totalKm + dist;
}

void freeRoute(struct Route *r) {
    struct Waypoint *p = r->head;
    while (p != NULL) {
        struct Waypoint *temp = p;
        p = p->next;
        free(temp);
    }
    r->head = NULL;  r->stops = 0;  r->totalKm = 0;
}

// Feature F5 : greedy nearest neighbour.
// from where we are, go to the closest zone not visited yet, repeat.
void buildRoute(struct CityGraph *g, struct Route *route, int start,
                int need[], int totalNeed) {
    int visited[TOTAL_ZONES];
    for (int i = 0; i < TOTAL_ZONES; i++) visited[i] = 0;
    int current = start;
    visited[start] = 1;
    addWaypoint(route, start, 0);
    for (int step = 0; step < totalNeed; step++) {
        int best = -1, bestDist = 9999;
        for (int k = 0; k < totalNeed; k++) {
            int z = need[k];
            if (visited[z] == 1) continue;
            if (g->dist[current][z] < bestDist) {
                bestDist = g->dist[current][z];
                best = z;
            }
        }
        if (best == -1) break;              // everything visited
        addWaypoint(route, best, bestDist);
        visited[best] = 1;
        current = best;
    }
}

// walk the linked list and print the itinerary
void displayRoute(struct CityGraph *g, struct Route *r) {
    cout << "\n  ---- DELIVERY ROUTE (" << r->stops << " stops) ----" << endl;
    if (r->head == NULL) { cout << "  no route" << endl;  return; }
    struct Waypoint *p = r->head;
    int n = 1;
    while (p != NULL) {
        cout << "  " << n << ". " << g->zoneName[p->zone];
        if (p->distFromPrev > 0) cout << "   (+" << p->distFromPrev << " km)";
        else cout << "   (start)";
        cout << endl;
        p = p->next;
        n++;
    }
    cout << "  Total distance : " << r->totalKm << " km" << endl;
}

// ================= PART B : C++ OOP PART =================

// Feature F8 : simulated clock that can be moved forward
class SimulatedClock {
private:
    int day;
    int hour;
public:
    SimulatedClock() { day = 1;  hour = 8; }
    SimulatedClock(int d, int h) { day = d;  hour = h; }
    int getDay() { return day; }
    int getHour() { return hour; }
    // counted from the start, so food age never goes negative after midnight
    int getTotalHours() { return (day - 1) * 24 + hour; }
    // while, not if, so a jump bigger than 24 hours still works
    void advance(int hours) {
        hour = hour + hours;
        while (hour >= 24) { hour = hour - 24;  day++; }
    }
    void addOneHour() { advance(1); }
    void addThreeHours() { advance(3); }
    void reset() { day = 1;  hour = 8; }
    const char* getPartOfDay() {
        if (hour >= 5 && hour <= 11) return "Morning";
        if (hour >= 12 && hour <= 16) return "Afternoon";
        if (hour >= 17 && hour <= 20) return "Evening";
        return "Night";
    }
    void display() {
        cout << "Day " << day << " , ";
        if (hour < 10) cout << "0";
        cout << hour << ":00 (" << getPartOfDay() << ")";
    }
};

// ================= MENU FUNCTIONS =================

// ask two zones and show the distance
void distanceMenu(struct CityGraph *g) {
    int a, b;
    cout << "\n  --- CHECK DISTANCE ---" << endl;
    displayZones(g);
    cout << "\n  From zone (0-5) : ";  cin >> a;
    cout << "  To zone   (0-5) : ";    cin >> b;
    int km = getDistance(g, a, b);
    if (km < 0) { cout << "  Wrong zone number." << endl;  return; }
    cout << "\n  " << g->zoneName[a] << "  ->  " << g->zoneName[b] << endl;
    cout << "  Distance    : " << km << " km" << endl;
    cout << "  Travel time : " << getTravelTime(g, a, b) << " hrs" << endl;
    int near = findNearestZone(g, a);
    cout << "  Nearest zone to " << g->zoneName[a] << " is " << g->zoneName[near]
         << " (" << g->dist[a][near] << " km)" << endl;
}

// ask the stops and build the greedy route
void routeMenu(struct CityGraph *g) {
    int start, howMany;
    int need[TOTAL_ZONES];
    cout << "\n  --- BUILD DELIVERY ROUTE ---" << endl;
    displayZones(g);
    cout << "\n  Van starts at zone (0-5) : ";  cin >> start;
    cout << "  How many stops (1-5)     : ";    cin >> howMany;
    if (start < 0 || start >= TOTAL_ZONES || howMany < 1 || howMany > 5) {
        cout << "  Wrong input." << endl;
        return;
    }
    for (int i = 0; i < howMany; i++) {
        cout << "  Stop " << (i + 1) << " zone : ";
        cin >> need[i];
        if (need[i] < 0 || need[i] >= TOTAL_ZONES) {
            cout << "  Wrong zone number." << endl;
            return;
        }
    }
    struct Route route;
    initRoute(&route);
    buildRoute(g, &route, start, need, howMany);
    displayRoute(g, &route);
    freeRoute(&route);
}

// move the clock forward
void clockMenu(SimulatedClock *clk) {
    int choice;
    cout << "\n  --- SIMULATED CLOCK ---" << endl;
    cout << "  Now : ";  clk->display();  cout << endl;
    cout << "\n  1. Forward 1 hour" << endl;
    cout << "  2. Forward 3 hours" << endl;
    cout << "  3. Forward custom hours" << endl;
    cout << "  4. Reset to Day 1, 08:00" << endl;
    cout << "  Choice : ";
    cin >> choice;
    if (choice == 1) clk->addOneHour();
    else if (choice == 2) clk->addThreeHours();
    else if (choice == 3) { int h;  cout << "  Hours : ";  cin >> h;  clk->advance(h); }
    else if (choice == 4) clk->reset();
    else { cout << "  Wrong choice." << endl;  return; }
    cout << "\n  New time : ";  clk->display();  cout << endl;
}

// ================= STANDALONE DEMO =================

void demoMember4() {
    struct CityGraph city;
    initGraph(&city);
    SimulatedClock clk;
    cout << "=========================================================" << endl;
    cout << "   MEMBER 4 : CITY MAP, DELIVERY ROUTE AND CLOCK" << endl;
    cout << "   Adjacency Matrix + Singly Linked List" << endl;
    cout << "=========================================================" << endl;
    cout << "\n1. The six city zones";
    displayZones(&city);
    cout << "\n2. The adjacency matrix";
    displayMatrix(&city);
    cout << "\n   diagonal is 0 (a zone is 0 km from itself)" << endl;
    cout << "   table is a mirror, dist[1][3] = dist[3][1] = 8" << endl;
    cout << "\n3. O(1) distance lookup" << endl;
    cout << "  dist[1][3] = " << getDistance(&city, 1, 3) << " km" << endl;
    cout << "  dist[0][5] = " << getDistance(&city, 0, 5) << " km" << endl;
    cout << "  travel time 1 -> 5 = " << getTravelTime(&city, 1, 5) << " hrs" << endl;
    cout << "\n4. Feature F5 : greedy nearest neighbour route" << endl;
    int need[3] = { 5, 1, 4 };
    cout << "\n  Van starts at " << city.zoneName[0] << endl;
    cout << "  Must deliver to zones 5, 1 and 4" << endl;
    int naive = city.dist[0][5] + city.dist[5][1] + city.dist[1][4];
    cout << "\n  Visiting them in request order 5 -> 1 -> 4 = " << naive << " km" << endl;
    struct Route route;
    initRoute(&route);
    buildRoute(&city, &route, 0, need, 3);
    displayRoute(&city, &route);
    cout << "\n  Greedy saved " << (naive - route.totalKm) << " km." << endl;
    freeRoute(&route);
    cout << "\n5. The simulated clock" << endl;
    cout << "  start        : ";  clk.display();  cout << endl;
    clk.addOneHour();
    cout << "  after +1 hr  : ";  clk.display();  cout << endl;
    clk.addThreeHours();
    cout << "  after +3 hr  : ";  clk.display();  cout << endl;
    cout << "\n6. Crossing midnight" << endl;
    cout << "  before       : ";  clk.display();
    cout << "   total = " << clk.getTotalHours() << " hrs" << endl;
    clk.advance(13);
    cout << "  after +13 hr : ";  clk.display();
    cout << "   total = " << clk.getTotalHours() << " hrs" << endl;
    cout << "\n  Hour wrapped back but total hours kept rising," << endl;
    cout << "  so food age never comes out negative." << endl;
    cout << "\n=========================================================" << endl;
}

#ifdef M4_STANDALONE
int main() {
    demoMember4();
    return 0;
}
#endif
#endif
