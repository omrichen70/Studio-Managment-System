//
// Created by omri on 09/11/2021.
//
#include "../include/Action.h"
#include <iostream>
#include "../include/Trainer.h"
#include "../include/Studio.h"
#include "../include/Customer.h"
#include <algorithm>
using namespace std;
extern Studio* backup;

BaseAction::BaseAction() {
    status = ERROR;
}

ActionStatus BaseAction::getStatus() const {
    return status;
}

void BaseAction::complete() {
    status = COMPLETED;
}

void BaseAction::error(std::string errorMsg) {
    this->errorMsg = errorMsg;
}

std::string BaseAction::getErrorMsg() const {
    return errorMsg;
}


OpenTrainer::OpenTrainer(int id, std::vector<Customer *> &customersList) : trainerId(id), customers(customersList){
    error("Workout session does not exist or is already open");
}

void OpenTrainer::act(Studio &studio) {
    Trainer* trainer = studio.getTrainer(trainerId);
    if(trainer == nullptr || trainer->isOpen()){
        cout << getErrorMsg() << endl;
    } else {
        trainer->openTrainer();
        for(auto customer: customers){
            trainer->addCustomer(makeNewCustomer(customer, customer->toString()));
        }
        complete();
    }
}

Customer* OpenTrainer:: makeNewCustomer(const Customer* other, string type) {
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

std::string OpenTrainer::toString() const {
    string str;
    str = "open " + to_string(trainerId);
    for(auto customer: customers){
        str +=" " + customer->getName() + "," + customer->toString();
    }
    if(getStatus() == COMPLETED) {
        str += " Completed";
    } else {
        str += " Error: " + getErrorMsg();
    }
    return str;
}

BaseAction *OpenTrainer::clone() {
    vector<Customer*> newCustomersList;
    for(auto customer: customers){
        newCustomersList.push_back(makeNewCustomer(customer, customer->toString()));
    }
    OpenTrainer* ot = new OpenTrainer(trainerId, newCustomersList);
    if(this->getStatus() == COMPLETED){
        ot->complete();
    }
    return ot;
}

OpenTrainer::~OpenTrainer() {
    for(auto customer: customers){
        delete customer;
        customer = nullptr;
    }
    customers.clear();
}

Order::Order(int id): trainerId(id) {
    error("Trainer does not exist or is not open");
}

void Order::act(Studio &studio) {
    Trainer* trainer = studio.getTrainer(trainerId);
    if(trainer == nullptr || trainer->isOpen() == false){
        cout << getErrorMsg() << endl;

    } else {
        for(auto customer : trainer->getCustomers()){
            trainer->order(customer->getId(), customer->order(studio.getWorkoutOptions()), studio.getWorkoutOptions());
        }
        for(auto order : trainer->getOrders()){
            cout << trainer->getCustomer(order.first)->getName() + " Is Doing " + order.second.getName() << endl;
        }
        complete();
    }
}

std::string Order::toString() const {
    string str;
    str = "order " + to_string(trainerId);
    if(getStatus() == COMPLETED) {
        str += " Completed";
    } else {
        str += " Error: " + getErrorMsg();
    }
    return str;
}

BaseAction *Order::clone() {
    Order* ord = new Order(trainerId);
    if(this->getStatus() == COMPLETED){
        ord->complete();
    }
    return ord;
}

MoveCustomer::MoveCustomer(int src, int dst, int customerId): srcTrainer(src), dstTrainer(dst), id(customerId){
    error("Cannot move customer");
}


void MoveCustomer::act(Studio &studio) {
    Trainer* sourceTrainer = studio.getTrainer(srcTrainer);
    Trainer* destTrainer = studio.getTrainer(dstTrainer);
    if(sourceTrainer == nullptr || destTrainer == nullptr || !sourceTrainer->isOpen() || !destTrainer->isOpen() || sourceTrainer->getCustomer(id) == nullptr || (int)destTrainer->getCapacity() == (int)destTrainer->getCustomers().size()){
        cout << getErrorMsg() << endl;
    } else {
        Customer* customer = sourceTrainer->getCustomer(id);
        sourceTrainer->reduceSalary(id);
        sourceTrainer->removeCustomer(id);
        destTrainer->addCustomer(customer);
        destTrainer->order(id, customer->order(studio.getWorkoutOptions()), studio.getWorkoutOptions());
        sort(destTrainer->getCustomers().begin(), destTrainer->getCustomers().end(), [ ]( const Customer* c1, const Customer* c2 )
        {
            return c1->getId()< c2->getId();
        });
        destTrainer->getOrders().clear();
        for(auto customer : destTrainer->getCustomers()){
            destTrainer->order(customer->getId(), customer->order(studio.getWorkoutOptions()), studio.getWorkoutOptions());
            destTrainer->reduceSalary(customer->getId());
        }
        if(sourceTrainer->getCustomers().empty()){
            sourceTrainer->closeTrainer();
        }
        complete();
    }
}

std::string MoveCustomer::toString() const {
    string str;
    str = "move " + to_string(srcTrainer) + " " + to_string(dstTrainer) + " " + to_string(id);
    if(getStatus() == COMPLETED) {
        str += " Completed";
    } else {
        str += " Error: " + getErrorMsg();
    }
    return str;
}

BaseAction *MoveCustomer::clone() {
    MoveCustomer* mc = new MoveCustomer(srcTrainer, dstTrainer, id);
    if(this->getStatus() == COMPLETED){
        mc->complete();
    }
    return mc;
}

Close::Close(int id): trainerId(id){
    error("Trainer does not exist or is not open");
}

void Close::act(Studio &studio) {
    Trainer* trainer = studio.getTrainer(trainerId);
    if(trainer == nullptr || !trainer->isOpen()) {
        cout << getErrorMsg() << endl;
    } else{
        trainer->closeTrainer();
        cout << "Trainer " + to_string(trainerId) + " closed. Salary " + to_string(trainer->getSalary()) + "NIS" << endl;
        complete();
    }
}

std::string Close::toString() const {
    string str = "close " + to_string(trainerId);
    if(getStatus() == COMPLETED) {
        str += " Completed";
    } else {
        str += " Error: " + getErrorMsg();
    }
    return str;
}

BaseAction *Close::clone() {
    Close* cl = new Close(trainerId);
    if(this->getStatus() == COMPLETED){
        cl->complete();
    }
    return cl;
}

CloseAll::CloseAll() {
}

void CloseAll::act(Studio &studio) {
    for(int i = 0; i< studio.getNumOfTrainers(); i++) {
        if(studio.getTrainer(i)->isOpen()) {
            BaseAction* close = new Close(i);
            close->act(studio);
            delete close;
        }
        complete();
    }

}

std::string CloseAll::toString() const {
    return "closeall Completed";
}

BaseAction *CloseAll::clone() {
    CloseAll* ca = new CloseAll();
    if(this->getStatus() == COMPLETED){
        ca->complete();
    }
    return ca;
}

PrintWorkoutOptions::PrintWorkoutOptions() {

}

void PrintWorkoutOptions::act(Studio &studio) {
    vector<Workout> workouts = studio.getWorkoutOptions();
    for(auto workout : workouts){
        string type;
        if(workout.getType() == ANAEROBIC){
            type = "ANAEROBIC";
        } else if(workout.getType() == MIXED){
            type = "MIXED";
        } else {
            type = "CARDIO";
        }
        cout << workout.getName() + ", " + type + ", " + to_string(workout.getPrice()) << endl;
    }
    complete();
}

std::string PrintWorkoutOptions::toString() const {
    return "workout_options Completed";
}

BaseAction *PrintWorkoutOptions::clone() {
    PrintWorkoutOptions* pwo = new PrintWorkoutOptions();
    if(this->getStatus() == COMPLETED){
        pwo->complete();
    }
    return pwo;
}

PrintTrainerStatus::PrintTrainerStatus(int id): trainerId(id) {

}

void PrintTrainerStatus::act(Studio &studio) {
    Trainer* trainer = studio.getTrainer(trainerId);
    if(!trainer->isOpen()){
        cout << "Trainer " + to_string(trainerId) + " status: closed" << endl;
    } else {
        cout << "Trainer " + to_string(trainerId) + " status: open" << endl;
        if(!trainer->getOrders().empty()){
            cout << "Customers:" << endl;
            for(auto customer : trainer->getCustomers()){
                cout << to_string(customer->getId()) + " " + customer->getName() << endl;
            }
            cout << "Orders:" << endl;
            for(auto workout: trainer->getOrders()){
                cout << workout.second.getName() + " " + to_string(workout.second.getPrice()) + "NIS " + to_string(workout.first) << endl;
            }
            cout << "Current Trainer's Salary: " + to_string(trainer->getSalary()) + "NIS" << endl;
        }
        complete();
    }
}

std::string PrintTrainerStatus::toString() const {
    return "status " + to_string(trainerId) + " Completed";
}

BaseAction *PrintTrainerStatus::clone() {
    PrintTrainerStatus* pts = new PrintTrainerStatus(trainerId);
    if(this->getStatus() == COMPLETED){
        pts->complete();
    }
    return pts;
}

PrintActionsLog::PrintActionsLog() {

}

void PrintActionsLog::act(Studio &studio) {
    vector<BaseAction*> actions = studio.getActionsLog();
    for(auto action : actions){
        cout << action->toString() << endl;
    }
    complete();
}

std::string PrintActionsLog::toString() const {
    return "log Completed";
}

BaseAction *PrintActionsLog::clone() {
    PrintActionsLog* pal = new PrintActionsLog();
    if(this->getStatus() == COMPLETED){
        pal->complete();
    }
    return pal;
}

BackupStudio::BackupStudio() {

}

void BackupStudio::act(Studio &studio) {
    if(backup != nullptr){
        backup->operator=(studio);
    } else {
        backup = new Studio(studio);
    }
    complete();
}

std::string BackupStudio::toString() const {
    return "backup Completed";
}

BaseAction *BackupStudio::clone() {
    BackupStudio* bu = new BackupStudio();
    if(this->getStatus() == COMPLETED){
        bu->complete();
    }
    return bu;
}

BackupStudio::~BackupStudio() {

} //******ADDED TODAY*****//


RestoreStudio::RestoreStudio() {
    error("No backup available");
}

void RestoreStudio::act(Studio &studio) {
    if(backup == nullptr){
        cout << getErrorMsg() << endl;
    } else {
        studio = *backup;
        complete();
    }

}

std::string RestoreStudio::toString() const {
    if(getStatus() == COMPLETED){
        return "restore Completed";
    }else {
        return "restore Error: " + getErrorMsg();
    }
}

BaseAction *RestoreStudio::clone() {
    RestoreStudio* res = new RestoreStudio();
    if(this->getStatus() == COMPLETED){
        res->complete();
    }
    return res;
}
