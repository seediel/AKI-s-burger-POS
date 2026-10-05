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
int expirycountdown(string);
void manage_ingredients();


class ingredients {
public:
    string name;
    int quantity;
    string expiry;
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
class item_details{
public:
    string name;
    int qty;
    double total_price;
};
class Receipt{
public: 
    vector<item_details> items;  
    double payment;
    double change;
    string method;
    string checkout_time;
};
vector <Receipt> receipts;
vector <int> real_number;

vector <ingredients> load_ingredients(){
    vector <ingredients> ingredientsinv;
    ifstream file("ingredients.csv");
    if (!file.is_open()) {
        cout << "Error: Could not open ingredients.csv" << endl;
        return ingredientsinv;
    }
    string line;
    while (getline(file,line)){
        if (line.empty()) {continue;}
        stringstream ss(line);
        string ingname, ingqty, ingexpiry;

        if (getline(ss,ingname,',') &&
            getline(ss,ingqty,',')&&
            getline(ss,ingexpiry,',')){
            ingredientsinv.push_back(ingredients{ingname, stoi(ingqty), ingexpiry});
            }    
    }
    return ingredientsinv;
}

void saveingredients (vector<ingredients> ingredientsinv){
    ofstream file("ingredients.csv", ios::trunc);
    if(!file.is_open()){
        cout << "Error: Could not open ingredients.csv for writing" << endl;
        return;
    }
    for (const ingredients& ing : ingredientsinv) {
        file << ing.name << "," << ing.quantity << "," << ing.expiry << "\n";
    }
    file.close();
}
vector <Receipt> load_receipts(){
    vector <Receipt> Receiptinv;
    ifstream file("receipt.csv");
    if (!file.is_open()) {
        cout << "Error: Could not open ingredients.csv" << endl;
        return Receiptinv;
    }
    string line;
    while (getline(file,line)){
        stringstream part(line);
        string sep;
        Receipt recs;
        while(getline(part,sep,'|')){
        int commas = count(sep.begin(),sep.end(),',');
        stringstream chunk(sep);
            if(commas == 2){
                string name,qtystr,totalstr;
                getline(chunk,name,',') &&
                getline(chunk,qtystr,',') &&
                getline(chunk, totalstr);
                int qty = stoi(qtystr);
                double total = stod(totalstr);

                recs.items.push_back(item_details{name,qty,total});
            }else{
                string paystr,changestr,method,checkoutime,receivetime;
                getline(chunk,paystr,',') &&
                getline(chunk,changestr,',') &&
                getline(chunk,method,',') &&
                getline(chunk,checkoutime);
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
    return Receiptinv;
}
void display_receipts() {
    auto rec = load_receipts();
    
    cout<<"\n================ RECEIPTS ===================\n";
    for (auto reciept : rec) {
        int num = 1;
        double total_price = 0;
        cout<<"--------------------------------------------\n";

        for (auto a : reciept.items){
            ostringstream price;
            price<<"₱"<<fixed<<setprecision(2)<< a.total_price;

            cout<<left<<setw(25)<<to_string(num++)+ "." +a.name
            <<setw(10)<<"QTY: "+to_string(a.qty)
            <<price.str()<<"\n";
            total_price += a.total_price;
        }
        cout<<"--------------------------------------------\n";
        ostringstream total, payment, change;
        total<<"₱"<<fixed<<setprecision(2)<<total_price;
        payment<<"₱"<<fixed<<setprecision(2)<<reciept.payment;
        change<<"₱"<<fixed<<setprecision(2)<<reciept.change;
        cout<<left<<setw(35)<<"Total: "<<total.str()<<"\n";
        cout<<left<<setw(35)<<"Payment: "<<payment.str()<<"\n";
        cout<<left<<setw(35)<<"Change: "<<change.str()<<"\n";
        cout<<left<<"Payment method: "+reciept.method<<"\n";
        cout<<left<<"Checkout time: "+reciept.checkout_time<<"\n";
        cout<<"=============================================\n";
    }    
    cout<<"\n==================================================\n";
    inventory_manager();
}
void display_ingredients() {
    auto ing = load_ingredients();
    cout<<"\n========================= INGREDIENTS ============================";
    for (int i = 0; i < ing.size(); i++) {
        cout<< left << setw(23) <<"\n" + to_string(i + 1) + ". "+ ing[i].name;
        cout<< left << setw(20) << "Quantity: " + to_string(ing[i].quantity);
        if (expirycountdown(ing[i].expiry) <= 0){
            cout<<"Expiry: (Expired)";
        }else{
            cout<< "Expiry: (" + to_string(expirycountdown(ing[i].expiry)) + ") Days left";
        }
    }
    cout<<"\n=================================================================\n";
}

void add_ingredients(){
    auto inginv = load_ingredients();
    display_ingredients(); 
    ingredients newing;
    cin.ignore();
    cout<<"enter ingredient name: ";
    getline(cin, newing.name);

    while(true){
        cout<<"enter ingredient quantity: ";
        cin>> newing.quantity;
        if(cin.fail() || newing.quantity <= 0){
            clear_inputs();
            cout<<"invalid quantity!\n";
        } else {
            break;
        }
    }    
    while(true){
    cout<<"enter ingredient expiry date (MM-DD-YYYY): ";
    cin>> newing.expiry;
    stringstream ss(newing.expiry);
    using namespace std::chrono;
    int y;
    unsigned int m,d;
    char dash1, dash2;
        if (!(ss >> m >> dash1 >> d >> dash2 >> y)){
            cout<<"invalid date";
            year_month_day expiry_date{year{y},month{m}, day{d}};
            year_month_day today{floor<days>(system_clock::now())};
            if (!expiry_date.ok()){
                cout<<"invalid date";
                if (expiry_date < today){
                    cout<<"invalid date";
                }
            }        
        }else{break;}
    }
    auto ingredientsinv = load_ingredients();
    ingredientsinv.push_back(newing);
    saveingredients(ingredientsinv);
    cout<<"ingredient added successfully!"<<endl;
    cout<<"do you want to add another ingredient? (y/n): ";
    char choice;
    cin>>choice;
    switch (choice){
        case 'y':
            add_ingredients();
            break;
        case 'n':
            return;
        default:
            cout<<"invalid choice!"<<endl;
            return;
    }
}

void delete_ingredients(){
    int deleteding;
    auto save = load_ingredients();

    display_ingredients();
    cout<<"\nenter ingredient number to delete: ";
    cin>>deleteding;
    string deleted_name = save[deleteding - 1].name;
    save.erase(save.begin() + deleteding - 1);
    saveingredients(save);
    cout<<deleted_name<<" deleted successfully!"<<endl;

    cout<<"do you want to delete another ingredient? (y/n): ";
    char choice;
    cin>>choice;
    switch (choice){
        case 'y':
            display_ingredients();
            delete_ingredients();
            break;
        case 'n':
            return;
        default:
            cout<<"invalid choice!"<<endl;
            return;
    }
}

void update_ingredients(){
    auto inginv = load_ingredients();
    int qty;
    string name, expiry;
    display_ingredients(); 
    cout<<"0. Back";
    cin.ignore();
    cout<<"\nenter ingredient id: ";
    int id;
    cin>>id;
        if (id > inginv.size() || id < 0){
            cout<<"invalid ingredient id";
            update_ingredients();
        }else if(id == 0){
            manage_ingredients();
        }else
    while(true){
        cout<<"add quantity: ";
        cin>> qty;
        if(cin.fail() || qty <= 0){
            clear_inputs();
            cout<<"invalid quantity!\n";
        } else {
            break;
        }
    }    
    while(true){
    cout<<"enter new expiry date (MM-DD-YYYY): ";
    cin>> expiry;
    stringstream ss(expiry);
    using namespace std::chrono;
    int y;
    unsigned int m,d;
    char dash1, dash2;
        if (!(ss >> m >> dash1 >> d >> dash2 >> y)){
            cout<<"invalid date";
            year_month_day expiry_date{year{y},month{m}, day{d}};
            year_month_day today{floor<days>(system_clock::now())};
            if (!expiry_date.ok()){
                cout<<"invalid date";
                if (expiry_date < today){
                    cout<<"invalid date";
                }
            }        
        }else{break;}
    }
    inginv[id-1].quantity += qty;
    inginv[id-1].expiry = expiry;
    saveingredients(inginv);
    cout<<"ingredient successfully updated!"<<endl;
    cout<<"do you want to update another ingredient? (y/n): ";
    char choice;
    cin>>choice;
    switch (choice){
        case 'y':
            update_ingredients();
            break;
        case 'n':
            return;
        default:
            cout<<"invalid choice!"<<endl;
            return;
    }
}

void manage_ingredients(){
    int choice;
    cout<<"---------------MANAGE INGREDIENTS--------------";
    cout<<"\n1. Display ingredients"<<"\n";
    cout<<"2. Add ingredient"<<"\n";
    cout<<"3. Delete ingredient"<<"\n";
    cout<<"4. Update ingredient"<<"\n";
    cout<<"5. Back"<<"\n";
    cout<<"enter your choice: ";
    cin>>choice;
    switch (choice){
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
        case 5:
            inventory_manager();
        default:
            cout<<"invalid choice!"<<endl;
            return;
    }
}

vector <MenuItem> load_menu(){
    vector <MenuItem> menuinv;
    ifstream file("menu.csv");
    if (!file.is_open()) {
        cout << "Error: Could not open menu.csv" << endl;
        return menuinv;
    }
    string line;
    while (getline(file,line)){
        if (line.empty()) {continue;}
        stringstream ss(line);
        string category,itemname, itemprice, ingreq, quantity_required;

        if (getline(ss,category,',') &&
            getline(ss,itemname,',') &&
            getline(ss,itemprice,',')&&
            getline(ss,ingreq,',') &&
            getline(ss,quantity_required,',')){
            bool found = false;
            for (auto& dup : menuinv){
                if (dup.item_name == itemname){
                    found = true;
                    dup.item_recipe.push_back(recipe{ingreq,stoi(quantity_required)});
                    break;
                }
            }
            if (found == false){
                MenuItem menu;
                menu.item_recipe.push_back(recipe{ingreq,stoi(quantity_required)});
                menuinv.push_back(MenuItem{category,itemname,stod(itemprice),menu.item_recipe});
            }
        }
    }
    return menuinv;
}

void save_menu (vector<MenuItem> menuinv){
    ofstream file("menu.csv", ios::trunc);
    if(!file.is_open()){
        cout << "Error: Could not open menu.csv for writing" << endl;
        return;
    }
    for (const MenuItem& item : menuinv) { 
        for(const recipe& req : item.item_recipe ){
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
    auto ing = load_ingredients();
    auto menu = load_menu();
    int num = 1;
    string title[] = {"BURGERS","HAM SANDWICHES","EGG SANDWICHES","FOOTLONGS","DRINKS","ADD-ONS"};
    string category[] = {"burger","ham sandwich","egg sandwich","footlong","drink","add-on"};
    cout<<"\n====================== BURGER SHOP MENU =======================";
    for(int label = 0; label < 6; label++ ){
        cout<<"\n"<<title[label]<<"\n";
        for (int i = 0; i < menu.size(); i++){
            if (menu[i].category == category[label]){
                {
                    ostringstream price;
                    price<< "price \u20B1"<< fixed << setprecision(2) << menu[i].price;
                    cout<<left << setw(25) << to_string(num++) + ". " + menu[i].item_name;
                    cout<<left << setw(22) << price.str();

                    string quantity;
                    bool expired = false;
                    for(auto exp : menu[i].item_recipe){
                        for(auto ingex : ing){
                            if (exp.ingredient_required == ingex.name){
                                if (expirycountdown(ingex.expiry) <= 0){
                                    expired = true;
                                    break;
                                }
                            }
                        }
                    }
                    if (stockcalculate(i) <= 0 || expired == true){
                        quantity = "out of stock";
                    }else{
                        quantity = to_string(stockcalculate(i));
                    }
                    cout<<left << "QTY: ("+ quantity +")"<<endl;
                    cout<<left <<setw(2) <<"Recipe: ";

                    auto menu_recipe = menu[i].item_recipe;
                    for (int j = 0; j < menu_recipe.size();j++ ){
                        cout<<left<<menu_recipe[j].ingredient_required +
                        "("+to_string(menu_recipe[j].quantity_required)+")  ";
                    }
                    cout<<"\n\n"; 
                    real_number.push_back(i);
                }
            }
            
        }
    }
}

void add_menu(){
    auto menuinv = load_menu();
    auto inginv = load_ingredients();
    int category;
    string categories[] = {"burger","ham sandwich","egg sandwich","footlong","drink","add-on"};
    MenuItem newitem;

    cout<<"\nCATEGORIES\n";
    for(int n = 0; n < 6; n++){
        cout<<n+1<<". "<<categories[n]<<"\n";
    }
    while (true){
        cout<<"enter item category(1-6): ";
        cin>> category;
        if(cin.fail() || category > 6 || category <= 0){
            clear_inputs();
        }else {     
            newitem.category = categories[category - 1];
            break;
        }    
    }      
    cin.ignore();
    cout<<"enter item name: ";
    getline(cin, newitem.item_name);

    while(true){
        cout<<"enter item price: ";
        cin>> newitem.price;
        if (cin.fail() || newitem.price <= 0){
            clear_inputs();
        } 
        else{break;}
    }

    recipe req;
    ingredients ingredient_choice;
    int ingchoice ;
    char choice1 = 'y';
    display_ingredients(); 
    while(choice1 == 'y'){  
        cout<<"enter ingredient id for recipe(1 only): ";
        cin>>ingchoice;
        if (ingchoice >= 1 && ingchoice <= inginv.size()){
            req.ingredient_required = inginv[ingchoice - 1].name; 
        }else{
            cout<<"invalid ingredient";
            return;
        }
        cout<<"enter quantity: ";
        cin>>req.quantity_required;
        newitem.item_recipe.push_back(req);

        cout<<"["<<newitem.category<<"]"
        <<"\t"<<newitem.item_name
        <<"\t ₱"<<left<<setw(6)<<newitem.price;
        for(auto recipe : newitem.item_recipe){
            cout<<recipe.ingredient_required<<"("<<recipe.quantity_required<<")\t";
        }

        cout<<"\ndo you want to add more? (y/n): ";
        cin>>choice1;
    }
    cout<<"save menu item(y/n)?: ";
    char saveitem;
    cin>>saveitem;
        if (saveitem == 'y'){
        cout<<"item added successfully!"<<endl;
        menuinv.push_back(newitem);
        save_menu(menuinv);
    } else {
        cout<<"item not added\n";
        manage_menu();
    }

    cout<<"do you want to add another item? (y/n): ";
    char choice2;
    cin>>choice2;
    switch (choice2){
        case 'y':
            add_menu();
            break;
        case 'n':
            return;
        default:
            cout<<"invalid choice!"<<endl;
            return;
    }
}


void delete_menu(){
    int deleteding = 0;
    auto menu = load_menu();
    display_menu();
    cout<<"0. back";
    cout<<"\nenter menu item number to delete: ";
    cin>>deleteding;
    if(deleteding < 1 || deleteding > menu.size()){
        manage_menu();
    }
    string deleted_name = menu[real_number[deleteding-1]].item_name;
    menu.erase(menu.begin() + real_number[deleteding - 1]);
    save_menu(menu);
    cout<<deleted_name<<" deleted successfully!"<<endl;
    cout<<"do you want to delete another menu item? (y/n): ";
    char choice;
    cin>>choice;
    switch (choice){
        case 'y':
            display_menu();
            delete_menu();
            break;
        case 'n':
            return;
        default:
            cout<<"invalid choice!"<<endl;
            return;
    }
}

void update_recipe(){
    recipe req;
    ingredients ingredient_choice;
    auto menuinv = load_menu();
    auto inginv = load_ingredients();
    int ingchoice;
    char choice1 = 'y';
    while(choice1 == 'y'){
        display_menu();   
        cout<<"enter menu id: ";
        cin>>ingchoice;
        if (ingchoice < 1 && ingchoice > menuinv.size()){
            cout<<"invalid menu id";
            return;
        }

        int num = 1;
        cout<<"\n["<<menuinv[real_number[ingchoice-1]].item_name<<"]\n";;
        for (auto i : menuinv[real_number[ingchoice-1]].item_recipe){
            cout<<left<<setw(25)<<to_string(num++)+". "+i.ingredient_required
            <<"Qty: "+to_string(i.quantity_required)<<"\n";
        }
        
        cout<<"\n1. delete an ingredient";
        cout<<"\n2. add an ingredient";
        cout<<"\n3. back";
        cout<<"\n enter your choice(1-3): ";
        int ingch;
        cin>>ingch;
        switch(ingch){
            case 1:
                int ids;
                cout<<"enter ingredient id from the recipe (1 - "<< menuinv[real_number[ingchoice-1]].item_recipe.size()<<"): ";
                cin>>ids;
                if(ids < 1 || ids > menuinv[real_number[ingchoice-1]].item_recipe.size()){
                    cout<<"invalid choice";
                    update_recipe();
                    break;
                }else{
                    auto& recing = menuinv[real_number[ingchoice-1]].item_recipe;
                    string deleteding = menuinv[real_number[ingchoice-1]].item_recipe[ids-1].ingredient_required;
                    recing.erase(recing.begin() + ids - 1);
                    cout<<deleteding<<" Successfully removed\n";
                    save_menu(menuinv);
                    manage_menu();
                    break;
                }
            case 2:
                display_ingredients();
                cout<<"enter ingredient id: ";
                int id;
                cin>>id;
                if(id < 1 || id > inginv.size()){
                    cout<<"invalid choice";
                    update_recipe();
                    break;
                }else{
                    recipe addeditem;
                    addeditem.ingredient_required = inginv[id-1].name;
                    cout<<"quantity: ";
                    int qty;
                    cin>>qty;
                    addeditem.quantity_required = qty;
                    menuinv[real_number[ingchoice-1]].item_recipe.push_back(addeditem);
                    cout<<addeditem.ingredient_required + " Successfully added\n";
                    }
                save_menu(menuinv);
                manage_menu();
                break;
            default:
                manage_menu();
        }
        cout<<"\ndo you want to add more? (y/n): ";
        cin>>choice1;
    }
    
}

void manage_menu(){
    int choice;
    cout<<"------------------MENU MANAGER------------------";
    cout<<"\n1. Display menu"<<"\n";
    cout<<"2. Add menu item"<<"\n";
    cout<<"3. Delete menu item"<<"\n";
    cout<<"4. update menu item recipe"<<"\n";
    cout<<"5. Back"<<"\n";
    cout<<"enter your choice: ";
    cin>>choice;
    switch (choice){
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
            inventory_manager();
            break;
        case 4:
            update_recipe();
        case 5:
            inventory_manager();
        default:
            cout<<"invalid choice!"<<endl;
            inventory_manager();
    }
}
void inventory_manager(){
int choice;
    cout<<"---------------Inventory manager----------------"<<endl;
    cout<<"1. Manage ingredients"<<"\n";
    cout<<"2. Manage menu"<<"\n";
    cout<<"3. View reciepts"<<"\n";
    cout<<"4. Exit"<<"\n";
    cout<<"enter your choice: ";
    cin>>choice;
    switch (choice){
        case 1:
            manage_ingredients();
            break;
        case 2:
            manage_menu();
            break;
        case 3:
            display_receipts();
        case 4:
            return;
        default:
            cout<<"invalid choice!"<<endl;
            return;
    }
}

int stockcalculate(int menuposition) {
    int max_portions = 999999999;
    auto menuinv = load_menu();
    auto stock = load_ingredients();
    
    const auto& selected_item = menuinv[menuposition];

        for (const auto& req : selected_item.item_recipe) {
            bool found = false;
            int stockqty = 0;
            for (const auto& ing : stock){
                if (ing.name == req.ingredient_required){
                    found = true;
                    stockqty = ing.quantity;
                    break;
                }                 
            }
            if (!found || req.quantity_required <= 0){
                return 0;
            }
        int portions = stockqty / req.quantity_required;
        max_portions = min(portions, max_portions);    
        }
    return max_portions;    
}

int expirycountdown(string expiry){
    int y;
    unsigned int m,d;
    char dash1,dash2;
    stringstream ss(expiry);
    using namespace std::chrono;

    
    if (!(ss >> m >> dash1 >> d >> dash2 >> y)){
        return 0;
    }
    year_month_day expiry_date{year{y}, month{m}, day{d}};
    if (!(expiry_date.ok())){
        return 0;
    }
    sys_days expiry_day = expiry_date;
    sys_days today = {floor<days>(system_clock::now())};
    auto days_left = expiry_day - today;

    return static_cast<int>(days_left.count());
}

void clear_inputs(){
    cin.clear();
    cin.ignore(1000,'\n');
    cout<<"invalid input!\n";
}
void checkrecipe(){
    auto load = load_menu();
    for(int i; i<load.size(); i++){
        for(auto recipes : load[i].item_recipe){
            for(auto ingredient : recipes.ingredient_required){
                cout<<ingredient;
            }
        }
    }
}
int main(){
    inventory_manager();
    return 0;
}

