//
// Created by omri on 08/11/2021.
//
#include <string>
#include <iostream>
#include "../include/Workout.h"


Workout::Workout(int w_id, std::string w_name, int w_price, WorkoutType w_type):
    id(w_id), name(w_name), price(w_price), type(w_type){

}

int Workout::getId() const {
    return id;
}

std::string Workout::getName() const {
    return name;
}

int Workout::getPrice() const {
    return price;
}

WorkoutType Workout::getType() const {
    return type;
}








