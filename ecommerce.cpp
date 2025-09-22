

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
using namespace std;

// interface Discount 
class Discount {
public:
    // apply: trả về giá/total sau khi giảm theo tỉ lệ rate (0..1)
    virtual double apply(double rate = 0) { return 0; }
    virtual ~Discount() = default; // destructor ảo
};

// template InventoryList<T>
template <typename T>
class InventoryList {
    vector<T> items;
public:
    void add(const T &item) { items.push_back(item); }

    // remove một phần tử (so sánh theo id, tránh so pointer trực tiếp)
    void remove(const T &item) {
        for (auto it = items.begin(); it != items.end(); ++it) {
            if ((*it) && item && (*it)->getId() == item->getId()) {
                items.erase(it);
                break;
            }
        }
    }

    void showAll() const {
  
        for (const auto &i : items) {
            if (i) i->displayInfo();
        }
    }

    vector<T>& getAll() { return items; } // cho phép truy cập bên ngoài
};

// Product class
class Product : public Discount {
protected:
    string id;
    string name;
    double price;
    int stock;
    vector<string> reviews;

public:
    // constructor
    Product(const string &i, const string &n, double p, int s)
        : id(i), name(n), price(p), stock(s) {
        // note: khởi tạo cơ bản
    }

    virtual ~Product() {
        // note: destructor ảo
    }

    // cap nhat kho (giảm stock khi bán)
    virtual bool updatestock(int hang) {
        // neu so luong <= 0 => invalid
        if (hang <= 0) {
            cout << "Invalid quantity for update!" << endl;
            return false;
        }
        // neu hang > stock => ko cap nhat
        if (hang > stock) {
            cout << "Not enough stock for product: " << name
                 << " | Available: " << stock
                 << " | Requested: " << hang << endl;
            return false;
        }
        stock -= hang; // cap nhat kho
        cout << "Sold " << hang << " units of " << name << endl;
        cout << "Remaining: " << stock << endl;
        if (stock == 0) {
            cout << "Remind to restock " << name << endl;
        }
        return true;
    }

    // hien thi thong tin san pham 
    virtual void displayInfo() const {
        cout << "ID: " << id << endl;
        cout << "Name: " << name << endl;
        cout << "Price (VND): " << price << endl;
        cout << "Stock: " << stock << endl;
        cout << endl;
    }

    // them review
    void addreview(const string &review) {
        reviews.push_back(review);
    }

    // hien thi review
    void show() const {
        cout << "Reviews for " << name << ":" << endl;
        for (const auto &r : reviews) {
            cout << "- " << r << endl;
        }
        cout << endl;
    }

    // getters
    string getId() const { return id; }
    string getname() const { return name; }
    double getprice() const { return price; }
    int getstock() const { return stock; }

    // implement Discount (product-level): tra ve gia sau khi giam
    double apply(double rate = 0) override {
        if (rate <= 0) return price;
        if (rate >= 1.0) return 0.0;
        double discount = price * rate;
        cout << "Giam gia san pham " << name << ": -" << discount << " VND" << endl;
        return price - discount;
    }

    // operator== so sanh theo id
    bool operator==(const Product &other) const {
        return this->id == other.id;
    }
};

// Electronics ke thua Product, override updatestock
class Electronics : public Product {
    double fee; // warranty fee (ví dụ)
public:
    Electronics(const string &i, const string &n, double p, int s, double f)
        : Product(i, n, p, s), fee(f) {}

    bool updatestock(int hang) override {
        if (!Product::updatestock(hang)) return false;
       
        cout << "Fee applied : " << fee << " VND" << endl;
        return true;
    }

    void displayInfo() const override {
        cout << "ID: " << id << endl;
        cout << "Name: " << name << endl;
        cout << "Price (VND): " << price << endl;
        cout << "Stock: " << stock << endl;
        cout << "Warranty Fee: " << fee << " VND" << endl;
        cout << endl;
    }
};

// Order class (tracking)
class Order {
    static int count;
    int orderid;
    string status; // pending, shipped, delivered
public:
    Order() : orderid(++count), status("Pending") {}
    void tracking() const {
        cout << "Order#" << orderid << " | Status: " << status << endl;
    }
    void updateStatus(const string &s) {
        status = s;
    }
};
int Order::count = 0;

// Shoppingcart (implements Discount)
class Shoppingcart : public Discount {
    vector<Product*> cart; // danh sach product pointer
    double total;
    int loyaltypoint;
public:
    Shoppingcart() : total(0), loyaltypoint(0) {} // note: khoi tao

    // overload operator += de add product vao cart
    Shoppingcart& operator+=(Product *p) {
        if (!p) {
            cout << "Null product pointer" << endl;
            return *this;
        }
        if (p->getstock() <= 0) {
            cout << "Cannot add this product (out of stock): " << p->getname() << endl;
            return *this;
        }
        // note: o thiet ke nay ta khong tu dong giam stock khi add vao cart
        // neu muon giam ngay luc add => goi p->updatestock(1) tai day
        // Ở đây ta giữ nguyên flow: add vào cart, không giảm stock
        cart.push_back(p);
        total += p->getprice();
        loyaltypoint += 10; // cho diem thanh vien +10 moi lan add
        cout << p->getname() << " added to cart" << endl;
        return *this;
    }

    void displaycart() const {
        cout << "List shopping cart:" << endl;
        for (auto p : cart) if (p) p->displayInfo();
        cout << "Total: " << total << " VND" << endl;
        cout << "Loyalty Points: " << loyaltypoint << endl;
        cout << endl;
    }

    // implement Discount::apply (cho cart)
    double apply(double rate = 0) override {
        if (rate <= 0) return total;
        if (rate >= 1.0) { total = 0; return 0; }
        double discount = total * rate;
        total -= discount;
        cout << "Discount applied to cart: -" << discount << " VND" << endl;
        return total;
    }

    double getTotal() const { return total; }
};

int main() {
    // test cases  100% viết bằng LLM 
    cout << "TEST CASES START" << endl;

    // tao san pham va electronics
    Product p1("P001", "Book", 100000, 5);
    Product p2("P002", "Pen", 5000, 10);
    Electronics e1("E001", "Laptop", 20000000, 2, 500000);

    // Inventory
    InventoryList<Product*> inventory;
    inventory.add(&p1);
    inventory.add(&p2);
    inventory.add(&e1);

    inventory.showAll(); // hien thi inventory

    // so sanh operator==
    cout << "p1 == p2 ? " << ((p1 == p2) ? "YES" : "NO") << endl;
    Product p3("P001", "Book Copy", 120000, 3);
    cout << "p1 == p3 ? " << ((p1 == p3) ? "YES" : "NO") << endl;

    // shopping cart
    Shoppingcart cart;
    cart += &p1;
    cart += &e1;
    cart.displaycart();

    // apply discount
    cart.apply(0.10); // giam 10%
    cart.displaycart();

    // add review + show
    p1.addreview("Good quality!");
    p1.addreview("Affordable price.");
    p1.show();

    // updatestock
    cout << "Update stock for Book: " << endl;
    p1.updatestock(2);   // ban 2 cuon
    p1.updatestock(10);  // qua so luong
    cout << "Update stock for Laptop (Electronics override): " << endl;
    e1.updatestock(1);   // ban 1 laptop

    // Order tracking
    Order order1;
    order1.tracking();
    order1.updateStatus("Shipped");
    order1.tracking();
    order1.updateStatus("Delivered");
    order1.tracking();


    return 0;
}
