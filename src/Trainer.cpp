//
// Created by omri on 08/11/2021.
//

#include <vector>
#include <algorithm>
#include "../include/Customer.h"
#include "../include/Workout.h"
#include "../include/Trainer.h"
#include "../include/Studio.h"
using namespace std;

typedef std::pair<int, Workout> OrderPair;

Trainer::Trainer(int t_capacity){
    capacity = t_capacity;
    open = false;
    customersList = std::vector<Customer*>();
    orderList = std::vector<OrderPair>();
    salary = 0;
}

int Trainer::getCapacity() const {
    return capacity;
}

void Trainer::addCustomer(Customer *customer) {
    customersList.push_back(customer);
}

void Trainer::removeCustomer(int id) {
    for(int i = 0; i < (int)customersList.size(); i++){
        if(customersList.at(i)->getId() == id){
            customersList.erase(customersList.begin()+i);

        }
    }
    vector<OrderPair> newOrderList;
    for(int i = 0; i < (int)orderList.size(); i++) {
        OrderPair x = orderList.at(i);
        if (x.first != id) {
            newOrderList.push_back(x);
        }
    }
    orderList.clear();
    for(int i = 0; i < (int)newOrderList.size(); i++){
        orderList.push_back(newOrderList.at(i));
    }
}

Customer* Trainer::getCustomer(int id) {
    for(int i = 0; i < (int)customersList.size(); i++){
        if(customersList.at(i)->getId() == id){
            return customersList.at(i);
        }
    }
    return nullptr;
}

std::vector<Customer *> &Trainer::getCustomers() {
    return customersList;
}

std::vector<OrderPair> &Trainer::getOrders() {
    return orderList;
}

void Trainer::openTrainer() {
    open = true;
}

void Trainer::closeTrainer() {
    open = false;
    for(int i = 0; i < (int)customersList.size(); i++){
        delete this->customersList[i];
        this->customersList[i] = nullptr;
    }
    customersList.erase(customersList.begin(), customersList.end());
    orderList.clear();
    customersList.clear();
}

int Trainer::getSalary() {
    return salary;
}

bool Trainer::isOpen() {
    return open;
}

void Trainer::order(const int customer_id, const std::vector<int> workout_ids, const std::vector<Workout> &workout_options) {
    for(auto workout_id : workout_ids){
        orderList.push_back(std::make_pair(customer_id, workout_options.at(workout_id)));
        salary += workout_options.at(workout_id).getPrice();
    }
}

void Trainer::reduceSalary(int customer_id) {
    for(auto wrk: orderList) {
        if(wrk.first == customer_id){
            salary -= wrk.second.getPrice();
        }
    }
}

Trainer::~Trainer() { //Distructor
    for(int i = 0; i < (int)customersList.size(); i++){
        delete this->customersList[i];
        this->customersList[i] = nullptr;
    }
    customersList.erase(customersList.begin(), customersList.end());
    orderList.clear();
    customersList.clear();
}


Trainer::Trainer(const Trainer &other) { //Copy constructor
    capacity = other.capacity;
    open = other.open;
    for(auto customer : other.customersList){
        Customer* c1 = makeNewCustomer(customer, customer->toString());
        customersList.push_back(c1);
    }
    for(auto order : other.orderList){
        orderList.push_back(order);
    }
    salary = other.salary;
}

Trainer::Trainer(Trainer &&other) { //Move constructor
    capacity = other.capacity;
    open = other.open;
    salary = other.salary;
    customersList = move(other.customersList);
    orderList = move(other.orderList);
    other.customersList.clear();
    other.orderList.clear();
}

Trainer &Trainer::operator=(const Trainer &other) { //Copy assignment operator
    if(this != &other){
        for(Customer* customer : customersList){
            delete customer;
            customer = nullptr;
        }
        customersList.clear();
        orderList.clear();
        customersList.clear();
        capacity = other.capacity;
        open = other.open;
        salary = other.salary;
        for(auto customer : other.customersList){
            Customer* c1 = makeNewCustomer(customer, customer->toString());
            customersList.push_back(c1);
        }
        for(auto order : other.orderList){
            orderList.push_back(order);
        }

    }
    return *this;
}



Trainer &Trainer::operator=(Trainer &&other) {
    if(this != &other){
        for(Customer* customer : customersList){
            delete customer;
            customer = nullptr;
        }
        orderList.clear();
        customersList.clear();
        capacity = other.capacity;
        open = other.open;
        salary = other.salary;
        customersList = move(other.customersList);
        orderList = move(other.orderList);
        other.customersList.clear();
        other.orderList.clear();
    }
    return *this;
}

Customer *Trainer::makeNewCustomer(const Customer* other, string type) {
    Customer* c1;
    if(type == "swt"){
        c1 = new SweatyCustomer(other->getName(), other->getId());
    } else if(type == "chp"){
        c1 = new CheapCustomer(other->getName(), other->getId());
    } else if(type == "mcl"){
        c1 = new HeavyMuscleCustomer(other->getName(), other->getId());
    } else {
        c1 = new FullBodyCustomer(other->getName(), other->getId());
    }
    return c1;
}




