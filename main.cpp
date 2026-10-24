// main.cpp
// FOOD REDISTRIBUTION MANAGEMENT SYSTEM (FRMS)
// Joins the work of all into one menu.
// Compile :  g++ -std=c++11 main.cpp -o frms.exe
#include "M1_UserDirectory.h"
#include "M2_FoodInventory.h"
#include "M3_MatchingEngine.h"
#include "M4_CityGraph.h"
using namespace std;

// read a menu number, recover if the user types a letter
int readChoice()
{
    int choice;
    cin >> choice;
    if (cin.eof()) return 0;        // no input left, close the program
    if (cin.fail())
    {
        cin.clear();                // remove the error flag
        cin.ignore(100, '\n');      // throw away the bad letters
        return -1;
    }
    return choice;
}

void showMenu(SimulatedClock *clk)
{
    cout << "   FOOD REDISTRIBUTION MANAGEMENT SYSTEM" << endl;
    cout << "   Time : ";  clk->display();  cout << endl;
    cout << "\n  -- User Directory --" << endl;
    cout << "   1. Register Donor            5. Donor Trust Rating" << endl;
    cout << "   2. Register NGO / Shelter    6. Show Hash Table Buckets" << endl;
    cout << "   3. Search User by ID         7. Delete a User" << endl;
    cout << "   4. Display All Users" << endl;
    cout << "\n  -- Food Inventory --" << endl;
    cout << "   8. Add Food Donation        11. Check All Food vs Travel" << endl;
    cout << "   9. Show Active Inventory    12. Show Hygiene Risk Tiers" << endl;
    cout << "  10. Check One Food Item      13. Remove Expired Food" << endl;
    cout << "\n  -- Matching Engine --" << endl;
    cout << "  14. Raise NGO Request        16. RUN PRIORITY MATCHING" << endl;
    cout << "  15. Show Request Queue       17. Serve Next Request (FIFO)" << endl;
    cout << "\n  -- City Map, Route and Clock --" << endl;
    cout << "  18. Show City Zone Map       20. Build Delivery Route" << endl;
    cout << "  19. Distance Between Zones   21. Move Clock Forward" << endl;
    cout << "\n  -- System --" << endl;
    cout << "  22. Load Demo Data            0. Exit" << endl;
    cout << "\n  Enter choice : ";
}

void loadAllDemoData(struct UserDirectory *dir, struct ActiveFoodList *food,
                     MatchingEngine *engine, int nowHours)
{
    cout << "\n  --- LOADING DEMO DATA ---" << endl;
    loadDemoUsers(dir);
    loadDemoFood(food, nowHours);
    engine->addRequest(new Request("R301", "N501", 60, 9, nowHours - 3, 2));
    engine->addRequest(new Request("R302", "N502", 100, 6, nowHours - 8, 5));
    engine->addRequest(new Request("R303", "N503", 40, 10, nowHours - 1, 1));
    cout << "  3 NGO requests added to the queue." << endl;
}

// the city graph gives the distance, the inventory checks the food
// survives the trip, the engine scores and ranks whatever is left
void runFullMatching(struct ActiveFoodList *food, MatchingEngine *engine,
                     struct CityGraph *city, int nowHours)
{
    cout << "\n  --- RUNNING PRIORITY MATCHING ---" << endl;
    if (food->count == 0) { cout << "  No food in the inventory." << endl;  return; }
    if (engine->getQueueCount() == 0) { cout << "  No NGO requests." << endl;  return; }
    struct RequestQueue *q = engine->getQueue();
    struct FoodNode *f = food->head;
    int good = 0, rejected = 0;
    while (f != NULL)
    {
        int i = q->front;
        for (int n = 0; n < q->count; n++)
        {
            Request *r = q->items[i];
            int travel = getTravelTime(city, f->food->getZone(), r->getZone());
            int km = getDistance(city, f->food->getZone(), r->getZone());
            if (f->food->isDeliverable(nowHours, travel) == 1)
            {
                engine->addCandidate(f->food->getId(), r, km,
                                     f->food->getQuantity(), nowHours);
                good++;
            } else
            {
                cout << "  skipped " << f->food->getId() << " -> " << r->getNgoId()
                     << " (only " << f->food->getUsableLife(nowHours, travel)
                     << " hrs usable after " << travel << " hr trip)" << endl;
                rejected++;
            }
            i = (i + 1) % MAX_REQUESTS;     // circular queue step
        }
        f = f->next;
    }
    cout << "\n  " << good << " pairs accepted, " << rejected
         << " rejected by shelf life." << endl;
    engine->runMatching();
}

void deleteUserMenu(struct UserDirectory *dir)
{
    char id[20];
    cout << "\n  --- DELETE A USER ---" << endl;
    cout << "  Enter ID : ";  cin >> setw(20) >> id;
    if (deleteUser(dir, id) == 1)
        cout << "  Removed " << id << ". Users left : " << dir->count << endl;
    else
        cout << "  No user with this ID." << endl;
}

int main()
{
    struct UserDirectory dir;
    struct ActiveFoodList food;
    struct CityGraph city;
    MatchingEngine engine;
    SimulatedClock clk(1, 18);          // start at Day 1, 6 pm
    initTable(&dir);
    initList(&food);
    initGraph(&city);
    cout << fixed << setprecision(1);
    cout << "\n  Welcome. Press 22 to load demo data first." << endl;
    int choice;
    do
    {
        showMenu(&clk);
        choice = readChoice();
        int now = clk.getTotalHours();      // re-read so the clock takes effect
        switch (choice)
        {
            case 1:  registerDonor(&dir); 
                     break;
            case 2:  registerRecipient(&dir);  
                     break;
            case 3:  findUserMenu(&dir); 
                     break;
            case 4:  displayAll(&dir);                         
                     break;
            case 5:  trustRatingMenu(&dir);                     
                     break;
            case 6:  displayBuckets(&dir);                      
                     break;
            case 7:  deleteUserMenu(&dir);                      
                     break;
            case 8:  addFoodMenu(&food, now);                   
                     break;
            case 9:  displayFoodList(&food, now);               
                     break;
            case 10: checkFoodMenu(&food, now);
                     break;
            case 11:
            {
                int travel;
                cout << "\n  Travel hours to the NGO : ";
                cin >> travel;
                showUsableLife(&food, now, travel);
                break;
            }
            case 12: showRiskTiers(&food, now);
                 break;
            case 13:
                cout << "\n  --- REMOVING EXPIRED FOOD ---" << endl;
                cout << "  " << removeExpired(&food, now) << " item(s) removed." << endl;
                break;
            case 14: addRequestMenu(&engine, now);              break;
            case 15: displayQueue(engine.getQueue(), now);      break;
            case 16: runFullMatching(&food, &engine, &city, now); break;
            case 17: serveNextMenu(&engine, now);               break;
            case 18: displayZones(&city);  displayMatrix(&city); break;
            case 19: distanceMenu(&city);                       break;
            case 20: routeMenu(&city);                          break;
            case 21: clockMenu(&clk);                           break;
            case 22: loadAllDemoData(&dir, &food, &engine, now); break;
            case 0:  cout << "\n  Closing the system..." << endl; break;
            default: cout << "\n  Wrong choice, please try again." << endl; break;
        }
    } while (choice != 0);
    freeTable(&dir);
    freeList(&food);
    cout << "  All memory freed. Goodbye." << endl;
    return 0;
}
