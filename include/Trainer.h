#ifndef TRAINER_H_
#define TRAINER_H_

#include <vector>
#include "Customer.h"
#include "Workout.h"
using namespace std;

typedef std::pair<int, Workout> OrderPair;

class Trainer{
public:
    Trainer(int t_capacity);
    int getCapacity() const;
    void addCustomer(Customer* customer);
    void removeCustomer(int id);
    Customer* getCustomer(int id);
    std::vector<Customer*>& getCustomers();
    std::vector<OrderPair>& getOrders();
    void order(const int customer_id, const std::vector<int> workout_ids, const std::vector<Workout>& workout_options);
    void openTrainer();
    void closeTrainer();
    int getSalary();
    bool isOpen();
    void reduceSalary(int customer_id);
    virtual ~Trainer(); //DISTRUCTOR
    Trainer(const Trainer &other); //COPY CONSTRUCTOR
    Trainer(Trainer &&other); //MOVE CONSTRUCTOR
    Trainer& operator=(const Trainer &other); //COPY ASSIGNMENT OPERATOR
    Trainer& operator=(Trainer &&other); //MOVE ASSIGNMENT OPERATOR
    void deleteCustomers();
    Customer* makeNewCustomer(const Customer* other, string type);

private:
    int capacity;
    bool open;
    std::vector<Customer*> customersList;
    std::vector<OrderPair> orderList; //A list of pairs for each order for the trainer - (customer_id, Workout)
    int salary;
};


#endif