#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <iomanip>
#include <algorithm>
#include <chrono>
#include <cmath>

using namespace std;

void inventory_manager();
int stockcalculate(int);
void manage_menu();
void clear_inputs();
void manage_ingredients();

struct InventoryBatch {
    string expirationDate; // Format: MM-DD-YYYY
    int quantity;
};

class ingredients {
public:
    string name;
    vector<InventoryBatch> batches;

    string getCategory() const {
        string n = name;
        transform(n.begin(), n.end(), n.begin(), ::tolower);
        
        if (n.find("patty") != string::npos || n.find("footlong") != string::npos || n.find("ham") != string::npos) {
            if (n.find("bun") == string::npos) return "Meat";
        }
        if (n.find("bun") != string::npos) {
            return "Breads";
        }
        if (n.find("cheese") != string::npos || n.find("egg") != string::npos || 
            n.find("milk") != string::npos || n.find("chuckie") != string::npos || 
            n.find("yault") != string::npos || n.find("delight") != string::npos || 
            n.find("choc-o") != string::npos) {
            return "Dairy";
        }
        if (n.find("coke") != string::npos || n.find("sprite") != string::npos || 
            n.find("royal") != string::npos || n.find("rc") != string::npos || 
            n.find("juicy") != string::npos || n.find("fruit soda") != string::npos || 
            n.find("zesto") != string::npos || n.find("c2") != string::npos || 
            n.find("mineral") != string::npos || n.find("sting") != string::npos || 
            n.find("cobra") != string::npos) {
            return "Beverages";
        }
        return "Condiments";
    }
};

class recipe {
public:    
    string ingredient_required;
    int quantity_required;
};

class MenuItem {
public:
    string category;
    string item_name;
    double price;
    vector<recipe> item_recipe;
};

class item_details {
public:
    string name;
    int qty;
    double total_price;
};

class Receipt {
public: 
    vector<item_details> items;  
    double payment;
    double change;
    string method;
    string checkout_time;
};

vector<Receipt> receipts;
vector<int> real_number;

// Helper function to get current system date as MM-DD-YYYY
string getCurrentDate() {
    auto now = chrono::system_clock::now();
    time_t now_c = chrono::system_clock::to_time_t(now);
    tm* local_tm = localtime(&now_c);

    ostringstream oss;
    oss << put_time(local_tm, "%m-%d-%Y");
    return oss.str();
}

// Load ingredients from CSV (Expected format: Name,Quantity,MM-DD-YYYY)
vector<ingredients> load_ingredients() {
    vector<ingredients> ingredientsinv;
    ifstream file("ingredients.csv");
    if (!file.is_open()) {
        return ingredientsinv; 
    }
    string line;
    while (getline(file, line)) {
        if (line.empty()) { continue; }
        stringstream ss(line);
        string ingName, ingQtyStr, ingExpiry;

        if (getline(ss, ingName, ',') &&
            getline(ss, ingQtyStr, ',') &&
            getline(ss, ingExpiry, ',')) {
            
            int qty = stoi(ingQtyStr);
            
            bool found = false;
            for (auto& ing : ingredientsinv) {
                if (ing.name == ingName) {
                    ing.batches.push_back(InventoryBatch{ingExpiry, qty});
                    found = true;
                    break;
                }
            }
            if (!found) {
                ingredients newIng;
                newIng.name = ingName;
                newIng.batches.push_back(InventoryBatch{ingExpiry, qty});
                ingredientsinv.push_back(newIng);
            }
        }
    }
    file.close();
    return ingredientsinv;
}

// Save ingredients to CSV (Format: Name,Quantity,MM-DD-YYYY)
void saveingredients(vector<ingredients> ingredientsinv) {
    ofstream file("ingredients.csv", ios::trunc);
    if (!file.is_open()) {
        cout << "Error: Could not open ingredients.csv for writing" << endl;
        return;
    }
    for (const ingredients& ing : ingredientsinv) {
        for (const auto& batch : ing.batches) {
            file << ing.name << "," 
                 << batch.quantity << "," 
                 << batch.expirationDate << "\n";
        }
    }
    file.close();
}

vector<Receipt> load_receipts() {
    vector<Receipt> Receiptinv;
    ifstream file("receipt.csv");
    if (!file.is_open()) {
        return Receiptinv;
    }
    string line;
    while (getline(file, line)) {
        stringstream part(line);
        string sep;
        Receipt recs;
        while (getline(part, sep, '|')) {
            int commas = count(sep.begin(), sep.end(), ',');
            stringstream chunk(sep);
            if (commas == 2) {
                string name, qtystr, totalstr;
                getline(chunk, name, ',') &&
                getline(chunk, qtystr, ',') &&
                getline(chunk, totalstr);
                int qty = stoi(qtystr);
                double total = stod(totalstr);

                recs.items.push_back(item_details{name, qty, total});
            } else {
                string paystr, changestr, method, checkoutime;
                getline(chunk, paystr, ',') &&
                getline(chunk, changestr, ',') &&
                getline(chunk, method, ',') &&
                getline(chunk, checkoutime);
                double pay = stod(paystr);
                double change = stod(changestr);
                
                recs.payment = pay;
                recs.change = change;
                recs.method = method;
                recs.checkout_time = checkoutime;

                Receiptinv.push_back(recs);
            }
        }
    }
    file.close();
    return Receiptinv;
}

void display_receipts() {
    auto rec = load_receipts();
    
    cout << "\n================ RECEIPTS ===================\n";
    for (auto reciept : rec) {
        int num = 1;
        double total_price = 0;
        cout << "--------------------------------------------\n";

        for (auto a : reciept.items) {
            ostringstream price;
            price << "PHP " << fixed << setprecision(2) << a.total_price;

            cout << left << setw(25) << to_string(num++) + "." + a.name
                 << setw(10) << "QTY: " + to_string(a.qty)
                 << price.str() << "\n";
            total_price += a.total_price;
        }
        cout << "--------------------------------------------\n";
        ostringstream total, payment, change;
        total << "PHP " << fixed << setprecision(2) << total_price;
        payment << "PHP " << fixed << setprecision(2) << reciept.payment;
        change << "PHP " << fixed << setprecision(2) << reciept.change;
        cout << left << setw(35) << "Total: " << total.str() << "\n";
        cout << left << setw(35) << "Payment: " << payment.str() << "\n";
        cout << left << setw(35) << "Change: " << change.str() << "\n";
        cout << left << "Payment method: " + reciept.method << "\n";
        cout << left << "Checkout time: " + reciept.checkout_time << "\n";
        cout << "=============================================\n";
    }    
    cout << "\n==================================================\n";
    inventory_manager();
}

void display_ingredients() {
    auto ing = load_ingredients();
    string categories[] = {"Meat", "Breads", "Dairy", "Beverages", "Condiments"};
    
    cout << "\n========================= INGREDIENTS ============================\n";
    int globalIndex = 1;

    for (const auto& cat : categories) {
        bool catHeaderPrinted = false;
        
        for (size_t i = 0; i < ing.size(); i++) {
            if (ing[i].getCategory() == cat) {
                if (!catHeaderPrinted) {
                    cout << "\n--- " << cat << " ---\n";
                    catHeaderPrinted = true;
                }
                
                int totalQty = 0;
                for (const auto& batch : ing[i].batches) {
                    totalQty += batch.quantity;
                }
                
                cout << "\n" << globalIndex << ". " << ing[i].name << "  |  Total Qty: " << totalQty << "\n";
                
                for (size_t b = 0; b < ing[i].batches.size(); b++) {
                    cout << "      Batch " << (b + 1) << " -> Qty: " << ing[i].batches[b].quantity 
                         << "  |  Exp: " << ing[i].batches[b].expirationDate << "\n";
                }
                globalIndex++;
            }
        }
    }
    cout << "\n=================================================================\n";
}

void add_ingredients() {
    display_ingredients(); 
    cin.ignore();
    
    string name, expiry;
    int quantity;

    cout << "Enter ingredient name: ";
    getline(cin, name);

    while (true) {
        cout << "Enter initial quantity: ";
        cin >> quantity;
        if (cin.fail() || quantity <= 0) {
            clear_inputs();
            cout << "Invalid quantity!\n";
        } else {
            break;
        }
    }    

    cin.ignore();
    while (true) {
        cout << "Enter expiration date (MM-DD-YYYY): ";
        getline(cin, expiry);
        if (expiry.length() == 10 && expiry[2] == '-' && expiry[5] == '-') {
            break;
        }
        cout << "Invalid format! Please use MM-DD-YYYY.\n";
    }

    auto ingredientsinv = load_ingredients();
    bool found = false;

    for (auto& ing : ingredientsinv) {
        if (ing.name == name) {
            ing.batches.push_back(InventoryBatch{expiry, quantity});
            found = true;
            break;
        }
    }

    if (!found) {
        ingredients newing;
        newing.name = name;
        newing.batches.push_back(InventoryBatch{expiry, quantity});
        ingredientsinv.push_back(newing);
    }

    saveingredients(ingredientsinv);
    cout << "Ingredient added successfully!\n";
    
    cout << "Do you want to add another ingredient? (y/n): ";
    char choice;
    cin >> choice;
    if (choice == 'y' || choice == 'Y') {
        add_ingredients();
    }
}

void delete_ingredients() {
    int deleteding;
    auto save = load_ingredients();

    if (save.empty()) {
        cout << "No ingredients available to delete.\n";
        return;
    }

    display_ingredients();
    cout << "\nEnter ingredient number to delete entirely: ";
    cin >> deleteding;
    
    if (deleteding < 1 || deleteding > save.size()) {
        cout << "Invalid ingredient number.\n";
        return;
    }

    string deleted_name = save[deleteding - 1].name;
    save.erase(save.begin() + deleteding - 1);
    saveingredients(save);
    cout << deleted_name << " deleted successfully!\n";

    cout << "Do you want to delete another ingredient? (y/n): ";
    char choice;
    cin >> choice;
    if (choice == 'y' || choice == 'Y') {
        delete_ingredients();
    }
}

void update_ingredients() {
    auto inginv = load_ingredients();
    if (inginv.empty()) {
        cout << "No ingredients found.\n";
        return;
    }

    display_ingredients(); 
    cout << "0. Back\n";
    cout << "\nEnter ingredient ID to restock (add new batch): ";
    int id;
    cin >> id;

    if (id == 0) {
        manage_ingredients();
        return;
    } else if (id < 0 || id > inginv.size()) {
        cout << "Invalid ingredient ID.\n";
        update_ingredients();
        return;
    }

    int qty;
    string expiry;

    while (true) {
        cout << "Enter quantity for new batch: ";
        cin >> qty;
        if (cin.fail() || qty <= 0) {
            clear_inputs();
            cout << "Invalid quantity!\n";
        } else {
            break;
        }
    }    

    cin.ignore();
    while (true) {
        cout << "Enter expiration date for this new batch (MM-DD-YYYY): ";
        getline(cin, expiry);
        if (expiry.length() == 10 && expiry[2] == '-' && expiry[5] == '-') {
            break;
        }
        cout << "Invalid format! Please use MM-DD-YYYY.\n";
    }

    inginv[id - 1].batches.push_back(InventoryBatch{expiry, qty});
    
    saveingredients(inginv);
    cout << "Ingredient batch successfully added!\n";
    
    cout << "Do you want to update another ingredient? (y/n): ";
    char choice;
    cin >> choice;
    if (choice == 'y' || choice == 'Y') {
        update_ingredients();
    }
}

void manage_ingredients() {
    int choice;
    cout << "---------------MANAGE INGREDIENTS--------------";
    cout << "\n1. Display ingredients" << "\n";
    cout << "\n2. Add ingredient / Restock Batch" << "\n";
    cout << "\n3. Delete ingredient" << "\n";
    cout << "\n4. Update ingredient batch/restock" << "\n";
    cout << "\n5. Back" << "\n";
    cout << "Enter your choice: ";
    cin >> choice;
    switch (choice) {
        case 1:
            display_ingredients();
            manage_ingredients();
            break;
        case 2:
            add_ingredients();
            manage_ingredients();
            break;
        case 3:
            delete_ingredients();
            manage_ingredients();
            break;
        case 4:
            update_ingredients();
            manage_ingredients();
            break;
        case 5:
            inventory_manager();
            break;
        default:
            cout << "Invalid choice!\n";
            manage_ingredients();
    }
}

vector<MenuItem> load_menu() {
    vector<MenuItem> menuinv;
    ifstream file("menu.csv");
    if (!file.is_open()) {
        return menuinv;
    }
    string line;
    while (getline(file, line)) {
        if (line.empty()) { continue; }
        stringstream ss(line);
        string category, itemname, itemprice, ingreq, quantity_required;

        if (getline(ss, category, ',') &&
            getline(ss, itemname, ',') &&
            getline(ss, itemprice, ',') &&
            getline(ss, ingreq, ',') &&
            getline(ss, quantity_required, ',')) {
            bool found = false;
            for (auto& dup : menuinv) {
                if (dup.item_name == itemname) {
                    found = true;
                    dup.item_recipe.push_back(recipe{ingreq, stoi(quantity_required)});
                    break;
                }
            }
            if (!found) {
                MenuItem menu;
                menu.category = category;
                menu.item_name = itemname;
                menu.price = stod(itemprice);
                menu.item_recipe.push_back(recipe{ingreq, stoi(quantity_required)});
                menuinv.push_back(menu);
            }
        }
    }
    file.close();
    return menuinv;
}

void save_menu(vector<MenuItem> menuinv) {
    ofstream file("menu.csv", ios::trunc);
    if (!file.is_open()) {
        cout << "Error: Could not open menu.csv for writing" << endl;
        return;
    }
    for (const MenuItem& item : menuinv) { 
        for (const recipe& req : item.item_recipe) {
            file << item.category << "," 
                 << item.item_name << "," 
                 << item.price << "," 
                 << req.ingredient_required << "," 
                 << req.quantity_required << "\n";
        }
    }
    file.close();
}

void display_menu() {
    auto menu = load_menu();
    int num = 1;
    real_number.clear();
    string title[] = {"BURGERS","HAM SANDWICHES","EGG SANDWICHES","FOOTLONGS","DRINKS","ADD-ONS"};
    string category[] = {"burger","ham sandwich","egg sandwich","footlong","drink","add-on"};
    cout << "\n====================== BURGER SHOP MENU =======================";
    for (int label = 0; label < 6; label++) {
        cout << "\n" << title[label] << "\n";
        for (int i = 0; i < menu.size(); i++) {
            if (menu[i].category == category[label]) {
                ostringstream price;
                price << "price PHP " << fixed << setprecision(2) << menu[i].price;
                cout << left << setw(25) << to_string(num++) + ". " + menu[i].item_name;
                cout << left << setw(22) << price.str();

                string quantity;
                int maxPortions = stockcalculate(i);
                if (maxPortions <= 0) {
                    quantity = "out of stock";
                } else {
                    quantity = to_string(maxPortions);
                }
                cout << left << "QTY: (" + quantity + ")" << endl;
                cout << left << setw(2) << "Recipe: ";

                auto menu_recipe = menu[i].item_recipe;
                for (size_t j = 0; j < menu_recipe.size(); j++) {
                    cout << left << menu_recipe[j].ingredient_required +
                    "(" + to_string(menu_recipe[j].quantity_required) + ")  ";
                }
                cout << "\n\n"; 
                real_number.push_back(i);
            }
        }
    }
}

void add_menu() {
    auto menuinv = load_menu();
    auto inginv = load_ingredients();
    int category;
    string categories[] = {"burger","ham sandwich","egg sandwich","footlong","drink","add-on"};
    MenuItem newitem;

    cout << "\nCATEGORIES\n";
    for (int n = 0; n < 6; n++) {
        cout << n + 1 << ". " << categories[n] << "\n";
    }
    while (true) {
        cout << "Enter item category (1-6): ";
        cin >> category;
        if (cin.fail() || category > 6 || category <= 0) {
            clear_inputs();
        } else {     
            newitem.category = categories[category - 1];
            break;
        }    
    }      
    cin.ignore();
    cout << "Enter item name: ";
    getline(cin, newitem.item_name);

    while (true) {
        cout << "Enter item price: ";
        cin >> newitem.price;
        if (cin.fail() || newitem.price <= 0) {
            clear_inputs();
        } else {
            break;
        }
    }

    recipe req;
    int ingchoice;
    char choice1 = 'y';
    display_ingredients(); 
    while (choice1 == 'y' || choice1 == 'Y') {  
        cout << "Enter ingredient ID for recipe: ";
        cin >> ingchoice;
        if (ingchoice >= 1 && ingchoice <= inginv.size()) {
            req.ingredient_required = inginv[ingchoice - 1].name; 
        } else {
            cout << "Invalid ingredient ID\n";
            return;
        }
        cout << "Enter quantity required: ";
        cin >> req.quantity_required;
        newitem.item_recipe.push_back(req);

        cout << "\nDo you want to add another ingredient to this recipe? (y/n): ";
        cin >> choice1;
    }

    cout << "Save menu item (y/n)?: ";
    char saveitem;
    cin >> saveitem;
    if (saveitem == 'y' || saveitem == 'Y') {
        cout << "Item added successfully!\n";
        menuinv.push_back(newitem);
        save_menu(menuinv);
    } else {
        cout << "Item not added\n";
        manage_menu();
        return;
    }

    cout << "Do you want to add another menu item? (y/n): ";
    char choice2;
    cin >> choice2;
    if (choice2 == 'y' || choice2 == 'Y') {
        add_menu();
    }
}

void delete_menu() {
    int deleteding = 0;
    auto menu = load_menu();
    if (menu.empty()) {
        cout << "No menu items available.\n";
        manage_menu();
        return;
    }

    display_menu();
    cout << "0. Back\n";
    cout << "Enter menu item number to delete: ";
    cin >> deleteding;
    
    if (deleteding == 0) {
        manage_menu();
        return;
    }
    if (deleteding < 1 || deleteding > real_number.size()) {
        cout << "Invalid selection.\n";
        manage_menu();
        return;
    }

    string deleted_name = menu[real_number[deleteding - 1]].item_name;
    menu.erase(menu.begin() + real_number[deleteding - 1]);
    save_menu(menu);
    cout << deleted_name << " deleted successfully!\n";

    cout << "Do you want to delete another menu item? (y/n): ";
    char choice;
    cin >> choice;
    if (choice == 'y' || choice == 'Y') {
        delete_menu();
    }
}

void update_recipe() {
    auto menuinv = load_menu();
    auto inginv = load_ingredients();
    if (menuinv.empty()) {
        cout << "No menu items available.\n";
        return;
    }

    display_menu();   
    cout << "Enter menu item ID to update recipe: ";
    int ingchoice;
    cin >> ingchoice;
    
    if (ingchoice < 1 || ingchoice > real_number.size()) {
        cout << "Invalid menu ID\n";
        return;
    }

    int targetIndex = real_number[ingchoice - 1];
    int num = 1;
    cout << "\n[" << menuinv[targetIndex].item_name << "]\n";
    for (auto i : menuinv[targetIndex].item_recipe) {
        cout << left << setw(25) << to_string(num++) + ". " + i.ingredient_required
             << "Qty: " << i.quantity_required << "\n";
    }
    
    cout << "\n1. Delete an ingredient from recipe";
    cout << "\n2. Add an ingredient to recipe";
    cout << "\n3. Back";
    cout << "\nEnter your choice (1-3): ";
    int ingch;
    cin >> ingch;
    
    switch (ingch) {
        case 1: {
            int ids;
            cout << "Enter recipe ingredient index to remove: ";
            cin >> ids;
            if (ids < 1 || ids > menuinv[targetIndex].item_recipe.size()) {
                cout << "Invalid index\n";
            } else {
                string removedName = menuinv[targetIndex].item_recipe[ids - 1].ingredient_required;
                menuinv[targetIndex].item_recipe.erase(menuinv[targetIndex].item_recipe.begin() + ids - 1);
                cout << removedName << " successfully removed from recipe.\n";
                save_menu(menuinv);
            }
            break;
        }
        case 2: {
            display_ingredients();
            cout << "Enter ingredient ID from inventory: ";
            int id;
            cin >> id;
            if (id < 1 || id > inginv.size()) {
                cout << "Invalid ingredient ID\n";
            } else {
                recipe addeditem;
                addeditem.ingredient_required = inginv[id - 1].name;
                cout << "Enter required quantity: ";
                int qty;
                cin >> qty;
                addeditem.quantity_required = qty;
                menuinv[targetIndex].item_recipe.push_back(addeditem);
                cout << addeditem.ingredient_required << " successfully added to recipe.\n";
                save_menu(menuinv);
            }
            break;
        }
        default:
            return;
    }
}

void manage_menu() {
    int choice;
    cout << "------------------MENU MANAGER------------------";
    cout << "\n1. Display menu" << "\n";
    cout << "\n2. Add menu item" << "\n";
    cout << "\n3. Delete menu item" << "\n";
    cout << "\n4. Update menu item recipe" << "\n";
    cout << "\n5. Back" << "\n";
    cout << "Enter your choice: ";
    cin >> choice;
    switch (choice) {
        case 1:
            display_menu();
            manage_menu();
            break;
        case 2:
            add_menu();
            manage_menu();
            break;
        case 3:
            delete_menu();
            manage_menu();
            break;
        case 4:
            update_recipe();
            manage_menu();
            break;
        case 5:
            inventory_manager();
            break;
        default:
            cout << "Invalid choice!\n";
            manage_menu();
    }
}

void inventory_manager() {
    int choice;
    cout << "---------------Inventory manager----------------" << endl;
    cout << "1. Manage ingredients" << "\n";
    cout << "2. Manage menu" << "\n";
    cout << "3. View receipts" << "\n";
    cout << "4. Exit" << "\n";
    cout << "Enter your choice: ";
    cin >> choice;
    switch (choice) {
        case 1:
            manage_ingredients();
            break;
        case 2:
            manage_menu();
            break;
        case 3:
            display_receipts();
            break;
        case 4:
            return;
        default:
            cout << "Invalid choice!\n";
            return;
    }
}

int stockcalculate(int menuposition) {
    int max_portions = 999999999;
    auto menuinv = load_menu();
    auto stock = load_ingredients();
    
    if (menuposition < 0 || menuposition >= menuinv.size()) {
        return 0;
    }

    const auto& selected_item = menuinv[menuposition];

    for (const auto& req : selected_item.item_recipe) {
        bool found = false;
        int totalAvailableQty = 0;
        
        for (const auto& ing : stock) {
            if (ing.name == req.ingredient_required) {
                found = true;
                for (const auto& batch : ing.batches) {
                    totalAvailableQty += batch.quantity;
                }
                break;
            }                 
        }
        if (!found || req.quantity_required <= 0) {
            return 0;
        }
        int portions = totalAvailableQty / req.quantity_required;
        max_portions = min(portions, max_portions);    
    }
    return max_portions;    
}

void clear_inputs() {
    cin.clear();
    cin.ignore(1000, '\n');
    cout << "Invalid input!\n";
}

int main() {
    inventory_manager();
    return 0;
}