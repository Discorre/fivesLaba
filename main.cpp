#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <iomanip>
#include <limits>
#include <fstream>
#include <filesystem>

// Класс товара
class Product {
public:
    std::string name;
    double price;
    int quantity;
    int seller_id;

    Product(std::string name, double price, int quantity, int seller_id)
        : name(std::move(name)), price(price), quantity(quantity), seller_id(seller_id) {}
};

// Класс продавца
class Seller {
public:
    int id;
    std::string name;

    Seller(int id, std::string name) : id(id), name(std::move(name)) {}

    void add_product(std::vector<Product>& products, const std::string& product_name, double price, int quantity) {
        products.emplace_back(product_name, price, quantity, id);
        std::cout << "Продукт \"" << product_name << "\" добавлен на площадку.\n";
    }
};

// Интерфейс для стратегии оплаты
class PaymentStrategy {
public:
    virtual ~PaymentStrategy() = default;
    virtual bool pay(double amount, double& balance) = 0;
    virtual std::string method_name() const = 0;
};

// Оплата наличными
class CashPayment : public PaymentStrategy {
public:
    bool pay(double amount, double& balance) override {
        if (balance >= amount) {
            balance -= amount;
            return true;
        }
        return false;
    }

    std::string method_name() const override {
        return "Оплата наличными";
    }
};

// Оплата картой
class CardPayment : public PaymentStrategy {
public:
    bool pay(double amount, double& balance) override {
        if (balance >= amount) {
            balance -= amount;
            return true;
        }
        return false;
    }

    std::string method_name() const override {
        return "Оплата картой";
    }
};

// Оплата криптовалютой
class CryptoPayment : public PaymentStrategy {
public:
    bool pay(double amount, double& balance) override {
        if (balance >= amount) {
            balance -= amount;
            return true;
        }
        return false;
    }

    std::string method_name() const override {
        return "Оплата криптовалютой";
    }
};

// Класс покупателя
class Customer {
public:
    int id;
    std::string name;
    double balance;
    std::vector<std::string> purchase_history;

    Customer(int id, std::string name, double balance)
        : id(id), name(std::move(name)), balance(balance) {}

    void buy_product(std::vector<Product>& products, int product_index, int quantity,
                     PaymentStrategy& payment_strategy) {
        if (product_index < 0 || product_index >= products.size()) {
            std::cout << "Неверный индекс товара.\n";
            return;
        }

        Product& product = products[product_index];
        double total_cost = product.price * quantity;

        if (product.quantity < quantity) {
            std::cout << "Недостаточно товара на складе.\n";
            return;
        }

        if (payment_strategy.pay(total_cost, balance)) {
            product.quantity -= quantity;
            purchase_history.push_back("Куплен товар: " + product.name + ", Количество: " + std::to_string(quantity) +
                                       ", Общая стоимость: " + std::to_string(total_cost));
            std::cout << "Покупка успешно завершена!\n";
            std::cout << "Товар: " << product.name << ", Количество: " << quantity
                      << ", Общая стоимость: " << std::fixed << std::setprecision(2) << total_cost
                      << ", Метод оплаты: " << payment_strategy.method_name()
                      << ", Остаток на балансе: " << balance << '\n';

            // Генерация чека и запись в файл
            generate_receipt(product, quantity, total_cost, payment_strategy.method_name());
        } else {
            std::cout << "Недостаточно средств для оплаты.\n";
        }
    }

    void view_purchase_history() const {
        std::cout << "История покупок:\n";
        for (const auto& purchase : purchase_history) {
            std::cout << purchase << '\n';
        }
    }

private:
    void generate_receipt(const Product& product, int quantity, double total_cost, const std::string& payment_method) {
        std::string directory = "checks";
        std::filesystem::create_directory(directory);  // Создание директории, если она не существует

        std::string filename = directory + "/" + name + "_receipt.txt";  // Имя файла - имя покупателя

        std::ofstream file(filename, std::ios::app);  // Открытие файла в режиме добавления
        if (file.is_open()) {
            file << "Чек для покупателя: " << name << "\n";
            file << "-------------------------------------\n";
            file << "Товар: " << product.name << "\n";
            file << "Количество: " << quantity << "\n";
            file << "Цена: " << product.price << "\n";
            file << "Общая стоимость: " << total_cost << "\n";
            file << "Метод оплаты: " << payment_method << "\n";
            file << "Остаток на балансе: " << balance << "\n";
            file << "-------------------------------------\n";
            file << "Спасибо за покупку!\n\n";

            std::cout << "Чек сохранен в файл: " << filename << "\n";
        } else {
            std::cout << "Не удалось сохранить чек.\n";
        }
    }
};

// Торговая площадка
class Marketplace {
private:
    std::vector<Seller> sellers;
    std::vector<Customer> customers;
    std::vector<Product> products;

public:
    void add_seller(const std::string& name) {
        sellers.emplace_back(sellers.size() + 1, name);
        std::cout << "Продавец \"" << name << "\" добавлен с ID " << sellers.back().id << ".\n";
    }

    void add_customer(const std::string& name, double balance) {
        customers.emplace_back(customers.size() + 1, name, balance);
        std::cout << "Покупатель \"" << name << "\" добавлен с ID " << customers.back().id << ".\n";
    }

    Seller* get_seller(int id) {
        if (id > 0 && id <= sellers.size()) {
            return &sellers[id - 1];
        }
        return nullptr;
    }

    Customer* get_customer(int id) {
        if (id > 0 && id <= customers.size()) {
            return &customers[id - 1];
        }
        return nullptr;
    }

    void list_products() const {
        std::cout << "\nСписок товаров:\n";
        if (products.empty()) {
            std::cout << "Нет товаров для отображения.\n";
            return;
        }
        for (size_t i = 0; i < products.size(); ++i) {
            const auto& product = products[i];
            std::cout << i + 1 << ". " << product.name << " (Цена: " << product.price
                      << ", Количество: " << product.quantity
                      << ", Продавец ID: " << product.seller_id << ")\n";
        }
    }

    std::vector<Product>& get_products() {
        return products;
    }

    void list_seller_products(int seller_id) const {
        std::cout << "\nТовары продавца ID " << seller_id << ":\n";
        bool found = false;
        for (const auto& product : products) {
            if (product.seller_id == seller_id) {
                found = true;
                std::cout << "- " << product.name << " (Цена: " << product.price
                          << ", Количество: " << product.quantity << ")\n";
            }
        }
        if (!found) {
            std::cout << "У продавца нет товаров.\n";
        }
    }

    void delete_product(int product_index) {
        if (product_index < 0 || product_index >= products.size()) {
            std::cout << "Неверный индекс товара.\n";
            return;
        }
        products.erase(products.begin() + product_index);
        std::cout << "Товар удалён.\n";
    }

    void update_product(int product_index, double new_price, int new_quantity) {
        if (product_index < 0 || product_index >= products.size()) {
            std::cout << "Неверный индекс товара.\n";
            return;
        }
        products[product_index].price = new_price;
        products[product_index].quantity = new_quantity;
        std::cout << "Товар обновлён.\n";
    }

    void filter_products_by_price(double min_price, double max_price) const {
        std::cout << "\nТовары в диапазоне цен от " << min_price << " до " << max_price << ":\n";
        bool found = false;
        for (const auto& product : products) {
            if (product.price >= min_price && product.price <= max_price) {
                found = true;
                std::cout << "- " << product.name << " (Цена: " << product.price
                          << ", Количество: " << product.quantity << ")\n";
            }
        }
        if (!found) {
            std::cout << "Нет товаров в данном ценовом диапазоне.\n";
        }
    }

    void list_all_sellers() const {
        std::cout << "\nВсе продавцы:\n";
        if (sellers.empty()) {
            std::cout << "Нет продавцов.\n";
            return;
        }
        for (const auto& seller : sellers) {
            std::cout << "ID: " << seller.id << ", Имя: " << seller.name << '\n';
        }
    }

    void list_all_customers() const {
        std::cout << "\nВсе покупатели:\n";
        if (customers.empty()) {
            std::cout << "Нет покупателей.\n";
            return;
        }
        for (const auto& customer : customers) {
            std::cout << "ID: " << customer.id << ", Имя: " << customer.name << '\n';
        }
    }
};

// Функция для безопасного ввода целого числа
int safe_input_int(const std::string& prompt) {
    int value;
    while (true) {
        std::cout << prompt;
        std::cin >> value;
        if (std::cin.fail()) {
            std::cin.clear(); // Очистка флага ошибки
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Игнорируем некорректный ввод
            std::cout << "Ошибка ввода! Пожалуйста, введите число.\n";
        } else {
            return value;
        }
    }
}

// Функция для безопасного ввода числа с плавающей точкой
double safe_input_double(const std::string& prompt) {
    double value;
    while (true) {
        std::cout << prompt;
        std::cin >> value;
        if (std::cin.fail()) {
            std::cin.clear(); // Очистка флага ошибки
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Игнорируем некорректный ввод
            std::cout << "Ошибка ввода! Пожалуйста, введите число.\n";
        } else {
            return value;
        }
    }
}

// Основная программа
int main() {
    Marketplace marketplace;

    while (true) {
        std::cout << "\nМеню:\n"
                  << "1. Добавить продавца\n"
                  << "2. Добавить покупателя\n"
                  << "3. Добавить товар\n"
                  << "4. Список товаров\n"
                  << "5. Купить товар\n"
                  << "6. Просмотреть товары продавца\n"
                  << "7. Удалить товар\n"
                  << "8. Обновить товар\n"
                  << "9. Просмотреть баланс покупателя\n"
                  << "10. Фильтрация товаров по цене\n"
                  << "11. Просмотреть историю покупок покупателя\n"
                  << "12. Просмотреть всех продавцов\n"
                  << "13. Просмотреть всех покупателей\n"
                  << "0. Выйти\n";

        int choice = safe_input_int("Выберите действие: ");

        if (choice == 1) {
            std::string seller_name;
            std::cout << "Введите имя продавца: ";
            std::cin.ignore();
            std::getline(std::cin, seller_name);
            marketplace.add_seller(seller_name);
        } 
        else if (choice == 2) {
            std::string customer_name;
            std::cout << "Введите имя покупателя: ";
            std::cin.ignore();
            std::getline(std::cin, customer_name);
            double balance = safe_input_double("Введите баланс покупателя: ");
            marketplace.add_customer(customer_name, balance);
        } 
        else if (choice == 3) {
            int seller_id = safe_input_int("Введите ID продавца: ");
            Seller* seller = marketplace.get_seller(seller_id);
            if (seller) {
                std::string product_name;
                std::cout << "Введите имя товара: ";
                std::cin.ignore();
                std::getline(std::cin, product_name);
                double price = safe_input_double("Введите цену товара: ");
                int quantity = safe_input_int("Введите количество товара: ");
                seller->add_product(marketplace.get_products(), product_name, price, quantity);
            } 
            else {
                std::cout << "Продавец с таким ID не найден.\n";
            }
        } 
        else if (choice == 4) {
            marketplace.list_products();
        } 
        else if (choice == 5) {
            int customer_id = safe_input_int("Введите ID покупателя: ");
            Customer* customer = marketplace.get_customer(customer_id);

            if (customer) {
                int product_index = safe_input_int("Введите индекс товара для покупки: ") - 1;
                int quantity = safe_input_int("Введите количество товара для покупки: ");
                int payment_choice = safe_input_int("Выберите метод оплаты (1 - наличными, 2 - картой, 3 - криптовалютой): ");
                PaymentStrategy* payment_strategy = nullptr;

                if (payment_choice == 1) {
                    payment_strategy = new CashPayment();
                } 
                else if (payment_choice == 2) {
                    payment_strategy = new CardPayment();
                } 
                else if (payment_choice == 3) {
                    payment_strategy = new CryptoPayment();
                }

                if (payment_strategy) {
                    customer->buy_product(marketplace.get_products(), product_index, quantity, *payment_strategy);
                    delete payment_strategy;
                } 
                else {
                    std::cout << "Некорректный выбор метода оплаты.\n";
                }
            }
             else {
                std::cout << "Покупатель с таким ID не найден.\n";
            }
        } 
        else if (choice == 6) {
            int seller_id = safe_input_int("Введите ID продавца: ");
            marketplace.list_seller_products(seller_id);

        } 
        else if (choice == 7) {
            int product_index = safe_input_int("Введите индекс товара для удаления: ") - 1;
            marketplace.delete_product(product_index);

        } 
        else if (choice == 8) {
            int product_index = safe_input_int("Введите индекс товара для обновления: ") - 1;
            double new_price = safe_input_double("Введите новую цену товара: ");
            int new_quantity = safe_input_int("Введите новое количество товара: ");
            marketplace.update_product(product_index, new_price, new_quantity);

        } 
        else if (choice == 9) {
            int customer_id = safe_input_int("Введите ID покупателя: ");
            Customer* customer = marketplace.get_customer(customer_id);
            if (customer) {
                std::cout << "Баланс покупателя: " << customer->balance << "\n";
            } 
            else {
                std::cout << "Покупатель с таким ID не найден.\n";
            }
        } 
        else if (choice == 10) {
            double min_price = safe_input_double("Введите минимальную цену товара: ");
            double max_price = safe_input_double("Введите максимальную цену товара: ");
            marketplace.filter_products_by_price(min_price, max_price);

        } 
        else if (choice == 11) {
            int customer_id = safe_input_int("Введите ID покупателя: ");
            Customer* customer = marketplace.get_customer(customer_id);
            if (customer) {
                customer->view_purchase_history();
            } 
            else {
                std::cout << "Покупатель с таким ID не найден.\n";
            }
        } 
        else if (choice == 12) {
            marketplace.list_all_sellers();
        } 
        else if (choice == 13) {
            marketplace.list_all_customers();
        } 
        else if (choice == 0) {
            break;
        } 
        else {
            std::cout << "Некорректный выбор. Попробуйте снова.\n";
        }
    }

    return 0;
}
