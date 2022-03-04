//
// Created by omri on 08/11/2021.
// trying to edit
#include <iostream>
#include "../include/Studio.h"
#include <string>
#include <fstream>
#include <istream>
#include <algorithm>
#include <sstream>
#include <iterator>
#include <vector>
using namespace std;
using std::cout; using std::cin;
using std::endl; using std::string;
using std::vector; using std::istringstream;
using std::stringstream;
Studio::Studio() {

}

Studio::Studio(const std::string &configFilePath){
    open = false;
    std::ifstream myFile;
    myFile.open(configFilePath);
    vector<string> configString;
    std::string myLine;
    if(myFile.is_open()){
        while(myFile){
            std::getline(myFile, myLine);
            if(not myLine.empty() && myLine.substr(0,1)!="#"){
                configString.push_back(myLine);
            }
        }
    }
    else {
        std::cout << "Couldn't open file." << endl;
    }
    string cap;
    istringstream ss(configString.at(1));
    trainers = vector<Trainer*>{};
    while(getline(ss, cap, ',')){
        Trainer* trainer = new Trainer(stoi(cap));
        trainers.push_back(trainer);
    }
    workout_options = vector<Workout>{};
    for(int i = 2, j = 0 ; i < (int)configString.size(); i++,j++){
        vector<string> split;
        string word;
        istringstream ss(configString.at(i));
        while(getline(ss, word, ',')){
            split.push_back(word);
        }
        WorkoutType type;
        if(split.at(1) == " Anaerobic") {
            type = WorkoutType(0);
        } else if ( split.at(1) == " Mixed") {
            type = WorkoutType(1);
        } else if( split.at(1) == " Cardio"){
            type = WorkoutType(2);
        }
        Workout wrk = Workout(j, split.at(0), stoi(split.at(2)), type);
        workout_options.push_back(wrk);
    }
    actionsLog = vector<BaseAction*>{};

}

void Studio::start() {
    id_counter = 0;
    std::cout << "Studio is now open!" << endl;
    open = true;
    std::string input;
    std::getline(cin, input);
    vector<string> words = splitInput(input);
    string command = words.at(0);
    while(command != "closeall"){
        //case 1:
        if(command == "open"){
            //OpenTrainer
            vector<Customer*> customers;
            int id = stoi(words.at(1));
            int counter = 0;
            for(int i = 2; (i < (int)words.size()) && getNumOfTrainers() > id && (counter < trainers.at(id)->getCapacity()); i++){
                vector<string> cus = splitComma(words.at(i));
                string cus_name = cus.at(0);
                if(cus.at(1) == "swt"){
                    customers.push_back(new SweatyCustomer(cus_name, id_counter));
                } else if(cus.at(1) == "chp"){
                    customers.push_back(new CheapCustomer(cus_name, id_counter));
                } else if(cus.at(1) == "mcl"){
                    customers.push_back(new HeavyMuscleCustomer(cus_name, id_counter));
                } else {
                    customers.push_back(new FullBodyCustomer(cus_name, id_counter));
                }
                id_counter++;
                counter++;
            }
            BaseAction* openTrainer = new OpenTrainer(id, customers);
            if(getTrainer(id) != nullptr){
                if(getTrainer(id)->getCapacity() != 0){
                openTrainer->act(*this);
                }
            } else {
                openTrainer->act(*this);
            }
            
            actionsLog.push_back(openTrainer);
        } else if(command == "order"){
            //Order
            int id = stoi(words.at(1));
            BaseAction* order = new Order(id);
            order->act(*this);
            actionsLog.push_back(order);
        } else if(command == "move"){
            //Move Customer
            int src = stoi(words.at(1));
            int dst = stoi(words.at(2));
            int id = stoi(words.at(3));
            BaseAction* move = new MoveCustomer(src,dst,id);
            move->act(*this);
            actionsLog.push_back(move);
        } else if(command == "close"){
            //Closes a Trainer
            int id = stoi(words.at(1));
            BaseAction* closeTrainer = new Close(id);
            closeTrainer->act(*this);
            actionsLog.push_back(closeTrainer);
        } else if(command == "workout_options"){
            //Prints workout options
            BaseAction* printWorkouts = new PrintWorkoutOptions();
            printWorkouts->act(*this);
            actionsLog.push_back(printWorkouts);
        } else if(command == "status"){
            //Status of trainer
            int id = stoi(words.at(1));
            BaseAction* printTrainer = new PrintTrainerStatus(id);
            printTrainer->act(*this);
            actionsLog.push_back(printTrainer);
        } else if(command == "log"){
            //Print actions log
            BaseAction* printLog = new PrintActionsLog();
            printLog->act(*this);
            actionsLog.push_back(printLog);
        } else if(command == "backup"){
            //Backup the studio
            BaseAction* back = new BackupStudio();
            back->act(*this);
            actionsLog.push_back(back);
        } else if(command == "restore"){
            //Restore from backup
            BaseAction* res = new RestoreStudio();
            res->act(*this);
            actionsLog.push_back(res);
        }
        getline(cin, input);
        words = splitInput(input);
        command = words.at(0);
    }
    if(command == "closeall"){
        //call closeall function
        BaseAction* closeAll = new CloseAll();
        closeAll->act(*this);
        actionsLog.push_back(closeAll);
    }
}

int Studio::getNumOfTrainers() const {
    return trainers.size();
}

Trainer *Studio::getTrainer(int tid) {
    if(tid >= getNumOfTrainers()){
        return nullptr;
    }
    return trainers.at(tid);
}

const std::vector<BaseAction *> &Studio::getActionsLog() const {
    return actionsLog;
}

std::vector<Workout> &Studio::getWorkoutOptions() {
    return workout_options;
}


std::vector<std::string> Studio::splitInput(std::string input) {
    string word;
    vector<string> splittedInput{};
    istringstream ss(input);
    while(getline(ss, word, ' ')){
        splittedInput.push_back(word);
    }
    return splittedInput;
}

std::vector<std::string> Studio::splitComma(std::string input) {
    string word;
    vector<string> splittedInput{};
    istringstream ss(input);
    while(getline(ss, word, ',')){
        splittedInput.push_back(word);
    }
    return splittedInput;
}

Studio::~Studio() { //DISTRUCTOR
    for(int i = 0; i < (int)trainers.size(); i++){
        trainers[i]->closeTrainer();
        delete this->trainers[i];
        this->trainers[i] = nullptr;
    }
    trainers.erase(trainers.begin(), trainers.end());
    for(BaseAction* act : actionsLog){
        delete act;
        act = nullptr;
    }
    actionsLog.erase(actionsLog.begin(), actionsLog.end());
    actionsLog.clear();
    workout_options.clear();
    id_counter = 0;

}

Studio::Studio(const Studio &other) { //COPY CONSTRUCTOR
    open = other.open;
    for(auto workout : other.workout_options){
        workout_options.push_back(workout);
    }
    for(auto trainer : other.trainers){
        Trainer* t = new Trainer(*trainer);
        trainers.push_back(t);
    }
    for(auto action : other.actionsLog){
        BaseAction* b = action->clone();
        this->actionsLog.push_back(b);
    }
    id_counter = other.id_counter;
}


Studio &Studio::operator=(const Studio &other) { //COPY ASSIGNMENT OPERATOR
    if(this != &other){
        for(auto trainer: trainers){
            delete trainer;
            trainer = nullptr;
        }
        trainers.clear();
        for(auto act : actionsLog){
            delete act;
            act = nullptr;
        }
        actionsLog.clear();
        workout_options.clear();
        id_counter = 0;
        id_counter = other.id_counter;
        open = other.open;
        for(auto workout : other.workout_options){
            workout_options.push_back(workout);
        }
        for(auto trainer : other.trainers){
            Trainer* t = new Trainer(*trainer);
            trainers.push_back(t);
        }
        for(auto action : other.actionsLog){
            BaseAction* b = action->clone();
            actionsLog.push_back(b);
        }
    }
    return *this;
}

Studio::Studio(Studio &&other){ //MOVE CONSTRUCTOR
    id_counter = other.id_counter;
    open = other.open;
    trainers = move(other.trainers);
    workout_options = move(other.workout_options);
    actionsLog = move(other.actionsLog);
    other.trainers.clear();
    other.workout_options.clear();
    other.actionsLog.clear();
    other.id_counter = 0;
    other.open = false;
}

Studio &Studio::operator=(Studio &&other) { //MOVE ASSIGNMENT OPERATOR
    if(this != &other){
        for(auto trainer: trainers){
            delete trainer;
            trainer = nullptr;
        }
        trainers.clear();
        for(auto act : actionsLog){
            delete act;
            act = nullptr;
        }
        actionsLog.clear();
        workout_options.clear();
        id_counter = 0;
        id_counter = other.id_counter;
        open = other.open;
        trainers = move(other.trainers);
        workout_options = move(other.workout_options);
        actionsLog = move(other.actionsLog);
        other.trainers.clear();
        other.workout_options.clear();
        other.actionsLog.clear();
        other.id_counter = 0;
        other.open = false;
    }
    return *this;
}




