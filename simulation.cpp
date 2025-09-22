#include "simulation.h"
#include <fstream>
#include <iostream>
#include <random>
#include <unordered_set>//for unique selection random contacts
#include <cstdlib>
#include <vector>
#include <ctime>
#include <algorithm>


// Definition of getInfectionRate
double Disease::getInfectionRate() const {
    return infectionRate;
}


// Constructor that sets the population size and vaccination rate
    Population::Population(int size, double vaccinationRate, bool patient_0,  Disease disease)
    : size(size), vaccinationRate(vaccinationRate), patient_0(patient_0), disease(disease) {
    initializePopulation();
}


int Population::interactWithPopulation(Population& otherPopulation, int day) {
    // Find the smaller population size
    int smallerSize = std::min(this->size, otherPopulation.size);

    // Calculate 5% of the smaller population, rounded to the nearest integer
    int interactionCount = std::max(1, static_cast<int>(std::round(smallerSize * 0.05)));
 
    // Random devices
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> thisDist(0, this->size - 1);
    std::uniform_int_distribution<> otherDist(0, otherPopulation.size - 1);

    // Simulate contacts
   
    for (int i = 0; i < interactionCount; ++i) {
        int thisPersonIndex = thisDist(gen);
        int otherPersonIndex = otherDist(gen);

        Person& thisPerson = this->people[thisPersonIndex];
        Person& otherPerson = otherPopulation.people[otherPersonIndex];

        

        // If one person is infectious and the other is susceptible, attempt transmission
        if (thisPerson.isInfectious() && otherPerson.isSusceptible()) {
            std::uniform_real_distribution<> infectionChance(0.0, 1.0);
            if (infectionChance(gen) < this->disease.getInfectionRate()) {
                otherPerson.infect(day);
            }
        } else if (otherPerson.isInfectious() && thisPerson.isSusceptible()) {
            std::uniform_real_distribution<> infectionChance(0.0, 1.0);
            if (infectionChance(gen) < otherPopulation.disease.getInfectionRate()) {
                thisPerson.infect(day);
            }
        }
    }
    return interactionCount; // Return the total number of interactions
}


// Method to initialize the population with vaccinated and susceptible people
void Population::initializePopulation() {
    people.clear(); // Ensure the vector is empty before adding people
    people.reserve(size); // Reserve space for the entire population

    // Step 2: Populate remaining people as Vaccinated or Susceptible based on vaccination rate
    int numVaccinated = static_cast<int>((size) * vaccinationRate);
    int numSusceptible = (size) - numVaccinated;
   
    // Populate the vector with vaccinated people
    for (int i = 0; i < numVaccinated; i++) {
        people.emplace_back(Person::Vaccinated);
       

    }
     // Populate the remaining people as susceptible
    for (int i = 0; i < numSusceptible; i++) {
        people.emplace_back(Person::Susceptible);  
    
    }
    if (patient_0) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dist(0, size - 1);

        int randomInfectiousIndex;
        do {
            randomInfectiousIndex = dist(gen);
        } while (!people[randomInfectiousIndex].isSusceptible());
        people[randomInfectiousIndex].infect(0); // Infect one individual on day 0
    }
}
 
   
    
void Population::simulateDay(int day) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> contactDist(0, size - 1);
    std::uniform_real_distribution<> infectionChance(0.0, 1.0);

//Infectious persons spread disease  and check recovery
    for (Person& person : people) {
        if (person.isInfectious()) {
            // Each infectious person contacts 5 random people
            for (int i = 0; i < 5; ++i) {
                int contactIndex = contactDist(gen);
                if (people[contactIndex].isSusceptible() &&
                    infectionChance(gen) < disease.getInfectionRate()) {
                    people[contactIndex].infect(day);// Infects with the current day
                }

            }
            // Check if this person recovers after 5 days
            person.checkRecovery(day);
 
        }

    }
   
   
}

bool Population::isInfectiousPresent() const {
    for (const Person& person : people) {
        if (person.isInfectious()) return true;
    }
    return false;
}
    



void Population::displayDailyStatistics(int day, const std::string& populationName) const {
    int susceptibleCount = 0, infectiousCount = 0, vaccinatedCount = 0, recoveredCount = 0;

    for (const Person& person : people) {
        switch (person.getStatus()) {
            case Person::Susceptible:
             ++susceptibleCount;
              break;
            case Person::Infectious:
             ++infectiousCount;
              break;
            case Person::Vaccinated:
             ++vaccinatedCount;
              break;
            case Person::Recovered:
             ++recoveredCount; 
             break;
        }
    }
     
     std::cout << "Final Susceptible: " << susceptibleCount << std::endl;
    std::cout << "Final Vaccinated: " << vaccinatedCount << std::endl;
    std::cout << "Final Recovered: "  << recoveredCount <<"\n"<< std::endl;
}

void Population::getPopulationStats(int& susceptible, int& infectious, int& vaccinated, int& recovered) const {
    susceptible = 0;
    infectious = 0;
    vaccinated = 0;
    recovered = 0;

    for (const Person& person : people) {
        switch (person.getStatus()) {
            case Person::Susceptible:
                ++susceptible;
                break;
            case Person::Infectious:
                ++infectious;
                break;
            case Person::Vaccinated:
                ++vaccinated;
                break;
            case Person::Recovered:
                ++recovered;
                break;
        }
    }       
       
}
