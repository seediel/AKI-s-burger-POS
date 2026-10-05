#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <iomanip>
#include <algorithm>
#include <chrono>
#include <ctime>
#include <cmath>
using namespace std;

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
class Cart {
public:
    MenuItem item;
    int amount;
    double total;
};
class Receipt{
public: 
    vector<Cart> items;  
    double payment;
    double change;
    string method;
    string checkout_time;
};

void inventory_manager();
int stockcalculate(int menuposition);
void manage_menu();
void clear_inputs();
int expirycountdown(string expiry);
void subtracted_qty(vector<Cart>cart);
void POS();
void display_cart();
void manage_cart();
vector <int> real_number;
vector<Cart>cart;
Receipt receipt;

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
void save_receipt (Receipt item){
    ofstream file ("receipt.csv", ios::app);
    if(!file.is_open()){
        cout<<"Error, could not open receipt.csv";
        return;
    }
    for(const Cart& rec : cart){
            file<<rec.item.item_name<<","
            <<rec.amount<<","
            <<rec.total<<"|";
        }
    file<<receipt.payment<<","
    <<receipt.change<<","
    <<receipt.method<<","
    <<receipt.checkout_time<<"\n";
}

void display_menu() {
    auto ing = load_ingredients();
    auto menu = load_menu();
    int num = 1;
    string title[] = {"BURGERS","HAM SANDWICHES","EGG SANDWICHES","FOOTLONGS","DRINKS","ADD-ONS"};
    string category[] = {"burger","ham sandwich","egg sandwich","footlong","drink","add-on"};
    cout<<"\n=================== AKI'S burger MENU ========================";
    for(int label = 0; label < 6; label++ ){
        cout<<"\n"<<title[label]<<"\n";
        for (int i = 0; i < menu.size(); i++){
            if (menu[i].category == category[label]){
                {
                    ostringstream price;
                    price<< "price \u20B1"<< fixed << setprecision(2) << menu[i].price;
                    cout<<left << setw(30) << to_string(num++) + ". " + menu[i].item_name;
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
                    real_number.push_back(i);
                }
            }
            
        }
    }
    cout<<"=============================================================\n";
}
void order(){
    auto menu = load_menu();
    int choice = 0;
    bool checked = false;

    cout<<"Enter your order code: ";
    cin>>choice;
    if(choice > menu.size() || choice <= 0 || stockcalculate(choice-1) == 0){
        cout<<"invalid order\n";
        order();
    }

    int qty = 0;
    bool invalid = true;
    while(invalid == true){
        cout<<"enter quantity: ";
        cin>>qty;
        if(qty < 0 || qty > stockcalculate(real_number[choice-1])){
            cout<<"invalid amount\n";
            continue;
        }
        int cart_amount = 0;
        for(auto o : cart){
            if(o.item.item_name == menu[real_number[choice-1]].item_name){
                cart_amount = o.amount;
                break;
            }else{
                cart_amount = 0;
            }
        }
        if (cart_amount + qty > stockcalculate(real_number[choice-1])){
            cout<<"invalid amount, exceeded stock\n";
        }else if(qty == cart_amount + qty){
            invalid = false;
        }else{
            invalid = false;
        }
    }

    for (auto& c : cart){
        if(c.item.item_name == menu[real_number[choice-1]].item_name){
            c.amount += qty;
            c.total += menu[real_number[choice-1]].price * qty;
            checked = true;
            break;
        }
    }
    
    if(checked == false){
        Cart newitem;
        newitem.item = menu[real_number[choice-1]];
        newitem.amount = qty;
        newitem.total = menu[real_number[choice-1]].price * qty;
        cart.push_back(newitem);
    }

    char des = 'n';
    cout<<"do you want to order again?(y/n): ";
    cin>>des;
    switch (des){
        case 'y':
            order();
            break;
        case 'n':
            POS();
            break;
        default:
            cout<<"invalid input\n";
    }
}
void remove(){
    display_cart();
    cout<<"\nenter item number to remove(1-"<<cart.size()<<"): ";
    int choice;
    cin>>choice;
    if(choice < 1 || choice > cart.size()){
        manage_cart();
    }
    string removed = cart[choice-1].item.item_name;
    cart.erase(cart.begin() + choice-1);
    cout<<removed<<" has been removed";
    cout<<"\n do you want to remove another item(y/n)?: ";
    char des = 'y';
    cin>>des;
    if (des == 'y'){
        remove();
    }else {
        manage_cart();
    }
}
void checkout(){
    display_cart();
    double total = 0.00;
    for (auto store : cart){
        total += store.total;
    }
    int choice;
    string method;
    cout<<"\n1. Gcash";
    cout<<"\n2. Cash";
    cout<<"\nenter payment method(1-2): ";
    cin>>choice;
    switch(choice){
        case 1: 
            method = "gcash";
            break;
        case 2:
            method = "cash";
            break;
        default:
            cout<<"invalid payent method";
            checkout();
    }
    double payment = 0;
    while(true){
        cout<<"enter payment amount: ";
        cin>>payment;
            if (payment < total || payment <= 0){
                cout<<"insufficient amount";
            }else {break;}
    }
    
    double change = payment - total;
    cout<<"change: "<<left<<setw(30)<<change;
    char final;    
    cout<<"\nconfirm checkout(y/n): ";
    cin>>final;
        if(final != 'y'){
            manage_cart();
        }
    using namespace std::chrono;
    time_t time_now = time(nullptr);
    tm* local_time = localtime(&time_now);
    stringstream time;
    time<<put_time(local_time,"%m-%d-%Y %H:%M:%S" );
    string current_time = time.str();

    for(auto item : cart){
        receipt.items.push_back(item);
    }
    receipt.payment = payment;
    receipt.change = change;
    receipt.method = method;
    receipt.checkout_time = current_time;
    subtracted_qty(cart);
    save_receipt(receipt);
    cart.clear();

}
void display_cart(){
    int num = 1;
    bool ch = false;
    cout<<"============================== CART ==========================\n";
    for (auto list : cart){
        if(list.amount == 0){continue;}
        cout<<num<<". "<<left<<setw(30)<<list.item.item_name;
        cout<<left<<setw(10)<<"Qty: " + to_string(list.amount);
        cout<<"₱"<<list.total<<"\n";
        num++;
    }
    double total = 0.00;
    for (auto store : cart){
        total += store.total;
    }
    cout<<left<<setw(43)<<"TOTAL: "<<"₱"<<total;
    cout<<"\n--------------------------------------------------------------";
}
void manage_cart(){
    display_cart();
    cout<<"\n1. add item";
    cout<<"\n2. remove item";
    cout<<"\n3. check out";
    cout<<"\n4. return";
    cout<<"\nenter your choice(1-4): ";
    int choice;
    cin>>choice;
    switch(choice){
        case 1: 
            display_menu();
            order();
            break;
        case 2:
            remove();
            break;
        case 3:
            checkout();
            break;
        case 4:
            POS();
        default:
            POS();

    }
    
}
void POS(){
    int choice;
    display_menu();
    cout<<"\n--------------------Aki's Burger POS-------------------------";
    cout<<"\n1. order item";
    cout<<"\n2. manage cart";
    cout<<"\n3. exit";
    cout<<"\nenter choice(1-3): ";
    cin>>choice;
    switch(choice){
        case 1: 
            order();
            break;
        case 2: 
            manage_cart();
            break;
        case 3:
            return;
    }
}
void subtracted_qty(vector<Cart> cart){
    auto ing = load_ingredients();
    auto menu = load_menu();
    
    for(auto shopcart : cart){
        for (auto& quant : shopcart.item.item_recipe){
            int qty = shopcart.amount * quant.quantity_required;
            for(auto& rec : ing){
                if(quant.ingredient_required == rec.name){
                    rec.quantity -= qty;
                    saveingredients(ing);
                    save_menu(menu);
                } 
            }    
        }
    }
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

int stockcalculate(int menuposition) {
    int max_portions = INT_MAX;
    auto menuinv = load_menu();
    auto stock = load_ingredients();
    
    const auto& selected_item = menuinv[menuposition];

        for (const auto& req : selected_item.item_recipe) {
            bool found = false;
            int stockqty = 0;
            for (const auto& ing : stock){
                if (ing.name == req.ingredient_required){
                    if(expirycountdown(ing.expiry) > 0){
                        found = true;
                        stockqty = ing.quantity;
                        break;
                    }else{
                        stockqty = 0;
                    }
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

void clear_inputs(){
    cin.clear();
    cin.ignore(1000,'\n');
    cout<<"invalid input!\n";
}

int main(){
    POS();
    return 0;
}

