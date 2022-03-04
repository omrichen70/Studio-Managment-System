#ifndef STUDIO_H_
#define STUDIO_H_

#include <vector>
#include <string>
#include "../include/Workout.h"
#include "../include/Trainer.h"
#include "../include/Action.h"

class Studio{
public:
	Studio();
    Studio(const std::string &configFilePath);
    void start();
    int getNumOfTrainers() const;
    Trainer* getTrainer(int tid);
	const std::vector<BaseAction*>& getActionsLog() const; // Return a reference to the history of actions
    std::vector<Workout>& getWorkoutOptions();
    std::vector<std::string> splitInput(std::string input);
    std::vector<std::string> splitComma(std::string input);
    virtual ~Studio(); //DISTRUCTOR
    Studio(const Studio &other); //COPY CONSTUCTOR
    Studio& operator=(const Studio &other); //COPY ASSIGNMENT OPERATOR
    Studio(Studio &&other); //MOVE CONSTRUCTOR
    Studio& operator=(Studio &&other); //MOVE ASSIGNMENT OPERATOR

private:
    int id_counter;
    bool open;
    std::vector<Trainer*> trainers;
    std::vector<Workout> workout_options;
    std::vector<BaseAction*> actionsLog;
};

#endif
