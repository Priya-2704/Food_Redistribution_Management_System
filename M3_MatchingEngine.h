// M3_MatchingEngine.h
// Member 3 - Priority Matching Engine
// Data Structures : Circular Queue (ngo requests) + Sorted Array (priority list)
// Feature F2 : Priority Score  P = 0.5E + 0.3W + 0.2Z
// Run alone :  g++ -x c++ -DM3_STANDALONE M3_MatchingEngine.h -o m3.exe
#ifndef M3_MATCHINGENGINE_H
#define M3_MATCHINGENGINE_H
#include <iostream>
#include <iomanip>
#include <cstring>
#include <cstdlib>
using namespace std;
#define MAX_REQUESTS 5
#define MAX_MATCHES 50
#ifndef TOTAL_ZONES
#define TOTAL_ZONES 6
#endif

// ================= PART B : C++ OOP PART =================
// one food request raised by an ngo
class Request
{
private:
    char reqId[20];
    char ngoId[20];
    int mealsNeeded;
    int urgency;
    int arrivalHour;
    int zone;
public:
    Request(const char *rid, const char *nid, int meals, int urg, int arrival, int z)
    {
        strcpy(reqId, rid);  strcpy(ngoId, nid);
        mealsNeeded = meals;  urgency = urg;  arrivalHour = arrival;  zone = z;
    }
    char* getReqId() { return reqId; }
    char* getNgoId() { return ngoId; }
    int getMeals() { return mealsNeeded; }
    int getUrgency() { return urgency; }
    int getArrivalHour() { return arrivalHour; }
    int getZone() { return zone; }
    // E part, urgency 1-10 becomes 10-100
    float getE() { return urgency * 10; }
    // W part, every hour of waiting adds 10, stopped at 100
    float getW(int currentHour)
    {
        int waited = currentHour - arrivalHour;
        if (waited < 0) waited = 0;
        float w = waited * 10;
        if (w > 100) w = 100;
        return w;
    }
    void display(int currentHour)
    {
        cout << "  " << reqId << " | NGO " << ngoId << " | " << mealsNeeded
             << " meals | Zone " << zone << " | Urgency " << urgency
             << " | Waiting " << (currentHour - arrivalHour) << " hrs" << endl;
    }
};

// Z part, nearer zone gets a higher score
float getZoneScore(int distance)
{
    float z = 100 - (distance * 5);
    if (z < 0) z = 0;
    return z;
}

// one food to ngo pairing with its priority score
class MatchPair
{
private:
    char foodId[20];
    char ngoId[20];
    int meals;
    int distance;
    float E, W, Z;
    float priority;
public:
    MatchPair(const char *fid, const char *nid, int m, int dist,
              float e, float w, float z)
    {
        strcpy(foodId, fid);  strcpy(ngoId, nid);
        meals = m;  distance = dist;  E = e;  W = w;  Z = z;
        priority = 0.5 * E + 0.3 * W + 0.2 * Z;     // Feature F2
    }
    char* getFoodId() { return foodId; }
    char* getNgoId() { return ngoId; }
    float getPriority() { return priority; }
    // compare two pairs by their priority score
    int operator > (MatchPair &other) { return (priority > other.priority) ? 1 : 0; }
    int operator < (MatchPair &other) { return (priority < other.priority) ? 1 : 0; }
    void display()
    {
        cout << "  " << foodId << " -> " << ngoId << " | " << meals
             << " meals | " << distance << " km | P = " << priority << endl;
    }
    void showFormula()
    {
        cout << "  " << foodId << " -> " << ngoId << endl;
        cout << "      P = 0.5(" << E << ") + 0.3(" << W << ") + 0.2(" << Z
             << ") = " << priority << endl;
    }
};

// ============== PART A : C DATA STRUCTURE PART ==============
// 1) CIRCULAR QUEUE - ngo requests in first come first serve order
struct RequestQueue
{
    Request *items[MAX_REQUESTS];
    int front;
    int rear;
    int count;
};

// rear starts at -1 because enqueue increases it first
void initQueue(struct RequestQueue *q)
{
    q->front = 0;  q->rear = -1;  q->count = 0;
}

int isQueueFull(struct RequestQueue *q) { return (q->count == MAX_REQUESTS) ? 1 : 0; }

int isQueueEmpty(struct RequestQueue *q) { return (q->count == 0) ? 1 : 0; }

// add at the rear, % makes it come back to 0
int enqueue(struct RequestQueue *q, Request *r)
{
    if (isQueueFull(q) == 1) { cout << "  Queue is full." << endl;  return 0; }
    q->rear = (q->rear + 1) % MAX_REQUESTS;
    q->items[q->rear] = r;
    q->count++;
    return 1;
}

// remove from the front
Request* dequeue(struct RequestQueue *q)
{
    if (isQueueEmpty(q) == 1) return NULL;
    Request *r = q->items[q->front];
    q->front = (q->front + 1) % MAX_REQUESTS;
    q->count--;
    return r;
}

// walk count items, not front to rear, because it may have wrapped
void displayQueue(struct RequestQueue *q, int currentHour)
{
    cout << "\n  ---- REQUEST QUEUE (" << q->count << " waiting) ----" << endl;
    if (isQueueEmpty(q) == 1) { cout << "  queue is empty" << endl;  return; }
    cout << "  front index = " << q->front << " , rear index = " << q->rear << endl;
    int i = q->front;
    for (int n = 0; n < q->count; n++)
    {
        cout << "  [" << i << "] ";
        q->items[i]->display(currentHour);
        i = (i + 1) % MAX_REQUESTS;
    }
}

// 2) SORTED ARRAY - priority list, biggest score always at position 0
struct MatchList
{
    MatchPair *items[MAX_MATCHES];
    int size;
};

void initMatchList(struct MatchList *l) { l->size = 0; }

// insertion sort step, shift smaller ones right and drop the new one in
int insertSorted(struct MatchList *l, MatchPair *m)
{
    if (l->size == MAX_MATCHES) { cout << "  List is full." << endl;  return 0; }
    int i = l->size - 1;
    while (i >= 0 && *l->items[i] < *m) {       // uses our overloaded <
        l->items[i + 1] = l->items[i];
        i--;
    }
    l->items[i + 1] = m;
    l->size++;
    return 1;
}

// take the best match from the front and shift the rest left
MatchPair* removeBest(struct MatchList *l)
{
    if (l->size == 0) return NULL;
    MatchPair *best = l->items[0];
    for (int i = 0; i < l->size - 1; i++)
        l->items[i] = l->items[i + 1];
    l->size--;
    return best;
}

void freeMatchList(struct MatchList *l)
{
    for (int i = 0; i < l->size; i++) delete l->items[i];
    l->size = 0;
}

// ================= THE ENGINE CLASS =================
// joins the queue and the priority list together
class MatchingEngine
{
private:
    struct RequestQueue queue;
    struct MatchList matches;
public:
    MatchingEngine() { initQueue(&queue);  initMatchList(&matches); }
    ~MatchingEngine()
    {
        while (isQueueEmpty(&queue) == 0) delete dequeue(&queue);
        freeMatchList(&matches);
    }
    struct RequestQueue* getQueue() { return &queue; }
    struct MatchList* getMatches() { return &matches; }
    int getQueueCount() { return queue.count; }
    int getMatchCount() { return matches.size; }
    // returns 0 if the queue was full, so the caller can clean up
    int addRequest(Request *r) { return enqueue(&queue, r); }
    // build one pair and put it in the sorted list
    void addCandidate(const char *foodId, Request *r, int distance,
                      int mealsGiven, int currentHour)
    {
        MatchPair *m = new MatchPair(foodId, r->getNgoId(), mealsGiven, distance,
                                     r->getE(), r->getW(currentHour),
                                     getZoneScore(distance));
        if (insertSorted(&matches, m) == 0)
            delete m;               // list was full, do not leak it
    }
    // take out matches one by one, best first
    void runMatching()
    {
        cout << "\n  ---- MATCHING RESULT (highest priority first) ----" << endl;
        if (matches.size == 0) { cout << "  no candidates" << endl;  return; }
        int rank = 1;
        while (matches.size > 0)
        {
            MatchPair *best = removeBest(&matches);
            cout << "  " << rank << ". ";
            best->display();
            delete best;
            rank++;
        }
    }
};

// ================= MENU FUNCTIONS =================
// take request details from the user
void addRequestMenu(MatchingEngine *engine, int currentHour)
{
    char reqId[20], ngoId[20];
    int meals, urgency, zone;
    cout << "\n  --- RAISE NGO REQUEST ---" << endl;
    cout << "  Request ID : ";      cin >> setw(20) >> reqId;
    cout << "  NGO ID     : ";      cin >> setw(20) >> ngoId;
    cout << "  Meals needed   : ";  cin >> meals;
    cout << "  Urgency (1-10) : ";  cin >> urgency;
    cout << "  Zone (0-5)     : ";  cin >> zone;
    if (meals <= 0 || urgency < 1 || urgency > 10 || zone < 0 || zone >= TOTAL_ZONES)
    {
        cout << "  Wrong value entered, request not added." << endl;
        return;
    }
    Request *r = new Request(reqId, ngoId, meals, urgency, currentHour, zone);
    if (engine->addRequest(r) == 1)
        cout << "\n  Request added to the queue." << endl;
    else
        delete r;                   // queue was full, do not leak it
}

// serve the front request in fifo order
void serveNextMenu(MatchingEngine *engine, int currentHour)
{
    cout << "\n  --- SERVE NEXT REQUEST (FIFO) ---" << endl;
    Request *r = dequeue(engine->getQueue());
    if (r == NULL) { cout << "  Queue is empty." << endl;  return; }
    cout << "  Serving : ";
    r->display(currentHour);
    delete r;
}

#endif
