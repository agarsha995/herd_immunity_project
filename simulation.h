

#ifndef SIMULATION_H
#define SIMULATION_H

#include <vector>
#include <string>

class Person {
public:
    enum Status { Susceptible, Infectious, Vaccinated, Recovered };

    // Constructor
    Person(Status initialStatus) : status(initialStatus), infectionDay(-1)  {}

    // Getter for current status
    Status getStatus() const { return status; }

    // Setter for current status
    void setStatus(Status newStatus) { status = newStatus; };

    bool isInfectious() const { return status == Infectious; }
    bool isSusceptible() const { return status == Susceptible; }
    bool isRecovered() const { return status == Recovered; }

    void infect(int day) { status = Infectious; infectionDay = day; }
    
    bool checkRecovery(int currentDay) {
        if (status == Infectious && infectionDay != -1 && (currentDay - infectionDay) >= 4) {
        status = Recovered;
        return true;
        }
        return false;
    }

    private:
    Status status;
    int infectionDay; // Tracks the day of infection for accurate recovery timing
};

class Disease {
private:
    double infectionRate; // Value between 0 and 1 representing the probability of infection

public:
    // Constructor
    Disease(double infectionRate) : infectionRate(infectionRate) {}

    // Getter for infection rate
    double getInfectionRate() const ;

};


class Population {
private:
    int size;
    double vaccinationRate;
    bool patient_0;//Indicates if the population starts with an initial infectious person
    Disease disease;
    std::vector<Person> people;

public:
    // Constructor
    Population(int size, double vaccinationRate, bool patient_0, Disease disease);

    // Method to initialize the population
    void initializePopulation();

    
    void simulateDay(int day);
    bool isInfectiousPresent() const;
    int interactWithPopulation(Population& otherPopulation, int day);

    void displayDailyStatistics(int day, const std::string& populationName) const;
    void getPopulationStats(int& susceptible, int& infectious, int& vaccinated, int& recovered) const;

};

#endif // SIMULATION_H
