// M1_UserDirectory.h
// Member 1 - User Directory and Donor Trust Rating
// Data Structure : Hash Table with Separate Chaining
// Feature S4 : Trust Score  S = 0.6F + 0.4Q
#ifndef M1_USERDIRECTORY_H
#define M1_USERDIRECTORY_H
#include <iostream>
#include <iomanip>
#include <cstring>
#include <cstdlib>
using namespace std;
#define TABLE_SIZE 11
#define TOTAL_ZONES 6

// ================= PART B : C++ OOP PART =================
class User
{
protected:
    char id[20];
    char name[50];
    int zone;
public:
    User(const char *uid, const char *uname, int z)
    {
        strcpy(id, uid);  strcpy(name, uname);  zone = z;
    }
    virtual ~User() { }
    char* getId() { return id; }
    char* getName() { return name; }
    int getZone() { return zone; }
    virtual void display() { cout << "  [USER] " << id << " | " << name << endl; }
    // 'D' for donor, 'R' for ngo
    virtual char getTag() { return 'U'; }
};

class DonorTrustProfile
{
private:
    int pledged;
    int fulfilled;
    float quality;
public:
    DonorTrustProfile() { pledged = 0;  fulfilled = 0;  quality = 0; }
    void setData(int p, int f, float q) { pledged = p;  fulfilled = f;  quality = q; }
    int getPledged() { return pledged; }
    int getFulfilled() { return fulfilled; }
    float getQuality() { return quality; }
    // fulfilment percentage
    float getF()
    {
        if (pledged == 0) return 50;
        return (fulfilled * 100.0) / pledged;
    }
    // S = 0.6F + 0.4Q
    float getScore() { return 0.6 * getF() + 0.4 * quality; }
    int isApproved() { return (getScore() >= 75) ? 1 : 0; }
    void addPledge(int done, float q)
    {
        quality = (quality * pledged + q) / (pledged + 1);
        pledged++;
        if (done == 1) fulfilled++;
    }
    void showScore()
    {
        cout << "    Pledges : " << fulfilled << " out of " << pledged << endl;
        cout << "    S = 0.6(" << getF() << ") + 0.4(" << quality
             << ") = " << getScore() << endl;
        cout << "    Status  : " << (isApproved() ? "AUTO-APPROVED" : "MANUAL CHECK") << endl;
    }
};

class Donor : public User
{
private:
    int type;
    DonorTrustProfile trust;
public:
    Donor(const char *uid, const char *uname, int z, int t, int p, int f, float q)
        : User(uid, uname, z) { type = t;  trust.setData(p, f, q); }
    int getType() { return type; }
    DonorTrustProfile& getTrust() { return trust; }
    const char* typeName()
    {
        if (type == 1) return "Restaurant";
        if (type == 2) return "Hostel Mess";
        return "Caterer";
    }
    void display()
    {
        cout << "  [DONOR] " << id << " | " << name << " | Zone " << zone
             << " | " << typeName() << " | Trust " << trust.getScore()
             << (trust.isApproved() ? " (AUTO-APPROVED)" : " (MANUAL CHECK)") << endl;
    }
    char getTag() { return 'D'; }
};

class Recipient : public User
{
private:
    int capacity;
    int urgency;
public:
    Recipient(const char *uid, const char *uname, int z, int cap, int urg)
        : User(uid, uname, z) { capacity = cap;  urgency = urg; }
    int getCapacity() { return capacity; }
    int getUrgency() { return urgency; }
    void display()
    {
        cout << "  [NGO  ] " << id << " | " << name << " | Zone " << zone
             << " | Capacity " << capacity << " | Urgency " << urgency << endl;
    }
    char getTag() { return 'R'; }
};

// ============== PART A : C DATA STRUCTURE PART ==============
// hash table with separate chaining, only struct and functions
struct Node
{
    char id[20];
    User *user;
    struct Node *next;
};

struct UserDirectory
{
    struct Node *bucket[TABLE_SIZE];
    int count;
};

// add ascii values and take remainder
int hashFunction(const char *key)
{
    int sum = 0;
    for (int i = 0; key[i] != '\0'; i++) sum = sum + key[i];
    return sum % TABLE_SIZE;
}

void initTable(struct UserDirectory *dir)
{
    for (int i = 0; i < TABLE_SIZE; i++) dir->bucket[i] = NULL;
    dir->count = 0;
}

// go to the bucket and search its chain
User* searchUser(struct UserDirectory *dir, const char *id)
{
    struct Node *p = dir->bucket[hashFunction(id)];
    while (p != NULL)
    {
        if (strcmp(p->id, id) == 0) return p->user;
        p = p->next;
    }
    return NULL;
}

// insert at the front of the chain
int insertUser(struct UserDirectory *dir, const char *id, User *u)
{
    if (searchUser(dir, id) != NULL) return 0;
    int index = hashFunction(id);
    struct Node *newNode = (struct Node *) malloc(sizeof(struct Node));
    strcpy(newNode->id, id);
    newNode->user = u;
    newNode->next = dir->bucket[index];
    dir->bucket[index] = newNode;
    dir->count++;
    return 1;
}

int deleteUser(struct UserDirectory *dir, const char *id)
{
    int index = hashFunction(id);
    struct Node *p = dir->bucket[index];
    struct Node *prev = NULL;
    while (p != NULL)
    {
        if (strcmp(p->id, id) == 0)
        {
            if (prev == NULL) dir->bucket[index] = p->next;
            else prev->next = p->next;
            delete p->user;
            free(p);
            dir->count--;
            return 1;
        }
        prev = p;
        p = p->next;
    }
    return 0;
}

void freeTable(struct UserDirectory *dir)
{
    for (int i = 0; i < TABLE_SIZE; i++)
    {
        struct Node *p = dir->bucket[i];
        while (p != NULL)
        {
            struct Node *temp = p;
            p = p->next;
            delete temp->user;
            free(temp);
        }
        dir->bucket[i] = NULL;
    }
    dir->count = 0;
}

// display() is virtual so the correct version runs
void displayAll(struct UserDirectory *dir)
{
    cout << "\n  ---- ALL USERS (" << dir->count << ") ----" << endl;
    if (dir->count == 0) { cout << "  directory is empty" << endl;  return; }
    for (int i = 0; i < TABLE_SIZE; i++)
    {
        struct Node *p = dir->bucket[i];
        while (p != NULL) { p->user->display();  p = p->next; }
    }
}

void displayBuckets(struct UserDirectory *dir)
{
    cout << "\n  ---- HASH TABLE (size " << TABLE_SIZE << ") ----" << endl;
    for (int i = 0; i < TABLE_SIZE; i++)
    {
        cout << "  Bucket[" << i << "] : ";
        struct Node *p = dir->bucket[i];
        if (p == NULL) cout << "empty";
        while (p != NULL)
        {
            cout << p->id;
            if (p->next != NULL) cout << " -> ";
            p = p->next;
        }
        cout << endl;
    }
}

// ================= MENU FUNCTIONS =================
void registerDonor(struct UserDirectory *dir)
{
    char id[20], name[50];
    int zone, type, pledged, fulfilled;
    float quality;
    cout << "\n  --- REGISTER DONOR ---" << endl;
    cout << "  Donor ID : ";   cin >> setw(20) >> id;
    if (searchUser(dir, id) != NULL) { cout << "  ID already used." << endl;  return; }
    cin.ignore();
    cout << "  Name     : ";                         cin.getline(name, 50);
    cout << "  Zone (0-5) : ";                       cin >> zone;
    cout << "  Type (1=Rest 2=Hostel 3=Caterer) : "; cin >> type;
    cout << "  Total pledges     : ";                cin >> pledged;
    cout << "  Pledges fulfilled : ";                cin >> fulfilled;
    cout << "  Avg quality (0-100) : ";              cin >> quality;
    if (zone < 0 || zone >= TOTAL_ZONES || type < 1 || type > 3 || pledged < 0 ||
        fulfilled < 0 || fulfilled > pledged || quality < 0 || quality > 100)
        {
        cout << "  Wrong value entered, donor not added." << endl;
        return;
    }
    Donor *d = new Donor(id, name, zone, type, pledged, fulfilled, quality);
    insertUser(dir, id, d);
    cout << "\n  Donor registered." << endl;
    d->getTrust().showScore();
}

void registerRecipient(struct UserDirectory *dir)
{
    char id[20], name[50];
    int zone, capacity, urgency;
    cout << "\n  --- REGISTER NGO / SHELTER ---" << endl;
    cout << "  Recipient ID : ";  cin >> setw(20) >> id;
    if (searchUser(dir, id) != NULL) { cout << "  ID already used." << endl;  return; }
    cin.ignore();
    cout << "  Name       : ";        cin.getline(name, 50);
    cout << "  Zone (0-5) : ";        cin >> zone;
    cout << "  Capacity (meals) : ";  cin >> capacity;
    cout << "  Urgency (1-10)   : ";  cin >> urgency;
    if (zone < 0 || zone >= TOTAL_ZONES || capacity <= 0 || urgency < 1 || urgency > 10)
    {
        cout << "  Wrong value entered, recipient not added." << endl;
        return;
    }
    insertUser(dir, id, new Recipient(id, name, zone, capacity, urgency));
    cout << "\n  Recipient registered." << endl;
}

void findUserMenu(struct UserDirectory *dir)
{
    char id[20];
    cout << "\n  --- SEARCH USER ---" << endl;
    cout << "  Enter ID : ";  cin >> setw(20) >> id;
    cout << "  Bucket number = " << hashFunction(id) << endl;
    User *u = searchUser(dir, id);
    if (u == NULL) cout << "  User not found." << endl;
    else u->display();
}

void trustRatingMenu(struct UserDirectory *dir)
{
    char id[20];
    cout << "\n  --- DONOR TRUST RATING ---" << endl;
    cout << "  Enter donor ID : ";  cin >> setw(20) >> id;
    User *u = searchUser(dir, id);
    if (u == NULL || u->getTag() != 'D') { cout << "  No such donor." << endl;  return; }
    Donor *d = (Donor *) u;
    cout << "\n  Donor : " << d->getName() << endl;
    d->getTrust().showScore();
}

void loadDemoUsers(struct UserDirectory *dir)
{
    insertUser(dir, "D101", new Donor("D101", "Hotel Sunrise", 1, 1, 10, 9, 88));
    insertUser(dir, "D102", new Donor("D102", "GEU Boys Hostel", 3, 2, 20, 12, 70));
    insertUser(dir, "D103", new Donor("D103", "Royal Caterers", 4, 3, 5, 5, 92));
    insertUser(dir, "D104", new Donor("D104", "Cafe Mountain", 0, 1, 8, 4, 60));
    insertUser(dir, "N501", new Recipient("N501", "Asha Shelter", 2, 80, 9));
    insertUser(dir, "N502", new Recipient("N502", "Bal Ashram", 5, 120, 6));
    insertUser(dir, "N503", new Recipient("N503", "Seva Shelter", 1, 60, 10));
    cout << "  Demo data loaded (4 donors, 3 NGOs)." << endl;
}

#endif
