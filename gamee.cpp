#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iomanip>

using namespace std;

// Commodity structure
struct Commodity {
    string name;
    int price;
};

// Trader class
class Trader {
public:
    string name;
    int gold;
    int carryingCapacity;
    map<string, int> inventory;

    Trader(string traderName, int startGold, int capacity)
        : name(traderName), gold(startGold), carryingCapacity(capacity) {}

    int getCurrentLoad() {
        int total = 0;
        for (const auto &item : inventory) {
            total += item.second;
        }
        return total;
    }

    void displayStats() {
        cout << "Trader: " << name << "\nGold: " << gold
             << "\nCurrent Load: " << getCurrentLoad() << "/" << carryingCapacity << endl;
        cout << "Inventory:\n";
        for (const auto &item : inventory) {
            cout << "  " << item.first << ": " << item.second << endl;
        }
        cout << endl;
    }
};

// Village class
class Village {
public:
    string name;
    vector<Commodity> commodities;

    Village(string villageName) : name(villageName) {}

    void generatePrices() {
        for (auto &commodity : commodities) {
            commodity.price = rand() % 90 + 10; // Random price between 10 and 100
        }
    }

    void displayCommodities() {
        cout << "Village: " << name << "\nCommodities:\n";
        for (size_t i = 0; i < commodities.size(); ++i) {
            cout << i + 1 << ". " << commodities[i].name << " - " << commodities[i].price << " gold\n";
        }
        cout << endl;
    }
};

// Game class
class Game {
    vector<Village> villages;
    Trader *trader;
    int day;
    const int travelCost = 10;

public:
    Game() : day(1) {}

    void initialize() {
        // Seed the random number generator
        srand(time(0));

        // Create villages
        villages.push_back(Village("Oakwood"));
        villages.push_back(Village("Maple Hill"));
        villages.push_back(Village("Pine Valley"));
        villages.push_back(Village("Willow Creek"));
        villages.push_back(Village("Cedar Town"));

        // Add commodities to villages
        for (auto &village : villages) {
            village.commodities = {
                {"Wheat", 0},
                {"Iron", 0},
                {"Gold", 0},
                {"Spices", 0},
                {"Silk", 0},
            };
        }

        // Create trader
        trader = new Trader("Alex", 100, 50);
    }

    void startDay() {
        cout << "\n--- Day " << day << " ---\n";
        for (auto &village : villages) {
            village.generatePrices();
        }
        trader->displayStats();
    }

    void travel() {
        cout << "Where would you like to travel? (Travel cost: " << travelCost << " gold)\n";
        for (size_t i = 0; i < villages.size(); ++i) {
            cout << i + 1 << ". " << villages[i].name << endl;
        }
        int choice;
        cin >> choice;

        if (choice >= 1 && choice <= villages.size()) {
            if (trader->gold >= travelCost) {
                trader->gold -= travelCost;
                cout << "Traveled to " << villages[choice - 1].name << ".\n";
                visitVillage(villages[choice - 1]);
            } else {
                cout << "Not enough gold to travel!\n";
            }
        } else {
            cout << "Invalid choice.\n";
        }
    }

    void visitVillage(Village &village) {
        while (true) {
            village.displayCommodities();
            trader->displayStats();

            cout << "What would you like to do? (1: Buy, 2: Sell, 3: Leave): ";
            int choice;
            cin >> choice;

            if (choice == 1) {
                buyCommodity(village);
            } else if (choice == 2) {
                sellCommodity(village);
            } else if (choice == 3) {
                cout << "Leaving the village.\n";
                break;
            } else {
                cout << "Invalid choice.\n";
            }
        }
    }

    void buyCommodity(Village &village) {
        cout << "Select a commodity to buy:\n";
        for (size_t i = 0; i < village.commodities.size(); ++i) {
            cout << i + 1 << ". " << village.commodities[i].name << " - "
                 << village.commodities[i].price << " gold\n";
        }
        int choice, quantity;
        cin >> choice;

        if (choice >= 1 && choice <= village.commodities.size()) {
            Commodity &commodity = village.commodities[choice - 1];
            cout << "Enter quantity: ";
            cin >> quantity;

            int cost = commodity.price * quantity;
            if (cost <= trader->gold &&
                trader->getCurrentLoad() + quantity <= trader->carryingCapacity) {
                trader->gold -= cost;
                trader->inventory[commodity.name] += quantity;
                cout << "Purchased " << quantity << " " << commodity.name << ".\n";
            } else {
                cout << "Not enough gold or carrying capacity!\n";
            }
        } else {
            cout << "Invalid choice.\n";
        }
    }

    void sellCommodity(Village &village) {
        cout << "Select a commodity to sell:\n";
        for (size_t i = 0; i < village.commodities.size(); ++i) {
            cout << i + 1 << ". " << village.commodities[i].name << " - "
                 << village.commodities[i].price << " gold\n";
        }
        int choice, quantity;
        cin >> choice;

        if (choice >= 1 && choice <= village.commodities.size()) {
            Commodity &commodity = village.commodities[choice - 1];
            cout << "Enter quantity: ";
            cin >> quantity;

            if (trader->inventory[commodity.name] >= quantity) {
                int earnings = commodity.price * quantity;
                trader->gold += earnings;
                trader->inventory[commodity.name] -= quantity;
                cout << "Sold " << quantity << " " << commodity.name << ".\n";
            } else {
                cout << "Not enough inventory!\n";
            }
        } else {
            cout << "Invalid choice.\n";
        }
    }

    void endDay() {
        cout << "\n--- End of Day " << day << " ---\n";
        trader->displayStats();
        cout << "Continue to Day " << (day + 1) << "? (1: Yes, 0: No): ";
        int choice;
        cin >> choice;
        if (choice == 1) {
            ++day;
        } else {
            cout << "Game Over. Thanks for playing!\n";
            exit(0);
        }
    }

    void saveGame() {
        ofstream saveFile("savegame.txt");
        saveFile << day << " " << trader->gold << " " << trader->carryingCapacity << "\n";
        for (const auto &item : trader->inventory) {
            saveFile << item.first << " " << item.second << "\n";
        }
        saveFile.close();
        cout << "Game saved.\n";
    }

    void loadGame() {
        ifstream saveFile("savegame.txt");
        if (saveFile.is_open()) {
            saveFile >> day >> trader->gold >> trader->carryingCapacity;
            trader->inventory.clear();
            string name;
            int quantity;
            while (saveFile >> name >> quantity) {
                trader->inventory[name] = quantity;
            }
            saveFile.close();
            cout << "Game loaded.\n";
        } else {
            cout << "No save file found.\n";
        }
    }
};

int main() {
    Game game;
    game.initialize();

    cout << "Would you like to load a saved game? (1: Yes, 0: No): ";
    int loadChoice;
    cin >> loadChoice;
    if (loadChoice == 1) {
        game.loadGame();
    }

    while (true) {
        game.startDay();
        game.travel();
        game.endDay();
    }

    return 0;
}
