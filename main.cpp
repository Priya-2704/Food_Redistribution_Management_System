// main.cpp
// FOOD REDISTRIBUTION MANAGEMENT SYSTEM (FRMS)
// Joins the work of all four members into one menu.
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

// print the menu grouped by member
void showMenu(SimulatedClock *clk)
{
    cout << "\n=========================================================" << endl;
    cout << "   FOOD REDISTRIBUTION MANAGEMENT SYSTEM" << endl;
    cout << "   Time : ";  clk->display();  cout << endl;
    cout << "=========================================================" << endl;
    cout << "\n  -- User Directory --" << endl;
    cout << "   1. Register Donor            4. Display All Users" << endl;
    cout << "   2. Register NGO / Shelter    5. Donor Trust Rating" << endl;
    cout << "   3. Search User by ID         6. Show Hash Table Buckets" << endl;
    cout << "\n  -- Food Inventory --" << endl;
    cout << "   7. Add Food Donation        10. Show Hygiene Risk Tiers" << endl;
    cout << "   8. Show Active Inventory    11. Remove Expired Food" << endl;
    cout << "   9. Check One Food Item" << endl;
    cout << "\n  -- Matching Engine --" << endl;
    cout << "  12. Raise NGO Request        14. RUN PRIORITY MATCHING" << endl;
    cout << "  13. Show Request Queue       15. Serve Next Request (FIFO)" << endl;
    cout << "\n  -- City Map, Route and Clock --" << endl;
    cout << "  16. Show City Zone Map       18. Build Delivery Route" << endl;
    cout << "  17. Distance Between Zones   19. Move Clock Forward" << endl;
    cout << "\n  -- System --" << endl;
    cout << "  20. Load Demo Data           21. Run Module Demo" << endl;
    cout << "   0. Exit" << endl;
    cout << "\n  Enter choice : ";
}

// fill the system with sample data in one keypress
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

// all four members work together here :
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

// run any one member's own demo
void memberDemoMenu()
{
    cout << "\n  --- RUN MODULE DEMO ---" << endl;
    cout << "   1. User Directory and Trust Rating" << endl;
    cout << "   2. Food Inventory and Decay" << endl;
    cout << "   3. Priority Matching Engine" << endl;
    cout << "   4. City Map, Route and Clock" << endl;
    cout << "  Choice : ";
    int n = readChoice();
    if (n == 1) demoMember1();
    else if (n == 2) demoMember2();
    else if (n == 3) demoMember3();
    else if (n == 4) demoMember4();
    else cout << "  Wrong choice." << endl;
    cout << fixed << setprecision(1);    // the demos change this, set it back
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
    cout << "\n  Welcome. Press 20 to load demo data first." << endl;
    int choice;
    do
    {
        showMenu(&clk);
        choice = readChoice();
        int now = clk.getTotalHours();      // re-read so the clock takes effect
        switch (choice)
        {
            case 1:  registerDonor(&dir);                       break;
            case 2:  registerRecipient(&dir);                   break;
            case 3:  findUserMenu(&dir);                        break;
            case 4:  displayAll(&dir);                          break;
            case 5:  trustRatingMenu(&dir);                     break;
            case 6:  displayBuckets(&dir);                      break;
            case 7:  addFoodMenu(&food, now);                   break;
            case 8:  displayFoodList(&food, now);               break;
            case 9:  checkFoodMenu(&food, now);                 break;
            case 10: showRiskTiers(&food, now);                 break;
            case 11:
                cout << "\n  --- REMOVING EXPIRED FOOD ---" << endl;
                cout << "  " << removeExpired(&food, now) << " item(s) removed." << endl;
                break;
            case 12: addRequestMenu(&engine, now);              break;
            case 13: displayQueue(engine.getQueue(), now);      break;
            case 14: runFullMatching(&food, &engine, &city, now); break;
            case 15: serveNextMenu(&engine, now);               break;
            case 16: displayZones(&city);  displayMatrix(&city); break;
            case 17: distanceMenu(&city);                       break;
            case 18: routeMenu(&city);                          break;
            case 19: clockMenu(&clk);                           break;
            case 20: loadAllDemoData(&dir, &food, &engine, now); break;
            case 21: memberDemoMenu();                          break;
            case 0:  cout << "\n  Closing the system..." << endl; break;
            default: cout << "\n  Wrong choice, please try again." << endl; break;
        }
    } while (choice != 0);
    freeTable(&dir);
    freeList(&food);
    cout << "  All memory freed. Goodbye." << endl;
    return 0;
}
