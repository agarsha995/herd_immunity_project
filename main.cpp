
#include <vector>
#include <fstream> // For file handling
#include <iomanip> // For formatting
#include <string>
#include <utility>
#include <random>
#include "simulation.h"
#include "include/INIReader.h"
#include <iostream>
#include <cmath>

// Struct to store final values of population statistics and number of timesteps
struct SimulationResults {
    int totalSteps = 0;
    int totalSusceptible = 0;
    int totalInfectious = 0;
    int totalRecovered = 0;
    int totalVaccinated = 0;

    std::vector<int> stepsPerRun;
    std::vector<int> susceptiblePerRun;
    std::vector<int> infectiousPerRun;
    std::vector<int> recoveredPerRun;
    std::vector<int> vaccinatedPerRun;

    void update(int steps, int susceptible, int infectious, int recovered, int vaccinated) {
        totalSteps += steps;
        totalSusceptible += susceptible;
        totalInfectious += infectious;
        totalRecovered += recovered;
        totalVaccinated += vaccinated;

        stepsPerRun.push_back(steps);
        susceptiblePerRun.push_back(susceptible);
        infectiousPerRun.push_back(infectious);
        recoveredPerRun.push_back(recovered);
        vaccinatedPerRun.push_back(vaccinated);
    }
};


// Function to compute mean and standard deviation
void computeMeanAndStdDev(
    const std::vector<int>& data, 
    double& mean, 
    double& stdDev) 
{
    if (data.empty()) {
        mean = 0.0;
        stdDev = 0.0;
        return;
    }

    double sum = 0.0;
    for (int value : data) {
        sum += value;
    }
    mean = sum / data.size();

    double variance = 0.0;
    for (int value : data) {
        variance += (value - mean) * (value - mean);
    }
    stdDev = std::sqrt(variance / data.size());
}

// Function to calculate and print statistics for all runs
void calculateAndPrintStatistics(const SimulationResults& results, int numRuns) {
    // Calculate means
    double meanSteps = static_cast<double>(results.totalSteps) / numRuns;
    double meanSusceptible = static_cast<double>(results.totalSusceptible) / numRuns;
    double meanInfectious = static_cast<double>(results.totalInfectious) / numRuns;
    double meanRecovered = static_cast<double>(results.totalRecovered) / numRuns;
    double meanVaccinated = static_cast<double>(results.totalVaccinated) / numRuns;

    // Calculate standard deviations
    double stdDevSteps, stdDevSusceptible, stdDevInfectious, stdDevRecovered, stdDevVaccinated;

    computeMeanAndStdDev(results.stepsPerRun, meanSteps, stdDevSteps);
    computeMeanAndStdDev(results.susceptiblePerRun, meanSusceptible, stdDevSusceptible);
    computeMeanAndStdDev(results.infectiousPerRun, meanInfectious, stdDevInfectious);
    computeMeanAndStdDev(results.recoveredPerRun, meanRecovered, stdDevRecovered);
    computeMeanAndStdDev(results.vaccinatedPerRun, meanVaccinated, stdDevVaccinated);

    // Print statistics
    // Print statistics
std::cout << "\n--- Simulation Statistics ---\n";

// Averages
std::cout << "\n--- Averages ---\n";
std::cout << "Average Steps:          " << meanSteps << "\n";
std::cout << "Average Susceptible:    " << meanSusceptible << "\n";
std::cout << "Average Infectious:     " << meanInfectious << "\n";
std::cout << "Average Recovered:      " << meanRecovered << "\n";
std::cout << "Average Vaccinated:     " << meanVaccinated << "\n";

// Standard Deviations
std::cout << "\n--- Standard Deviations ---\n";
std::cout << "StdDev Steps:           " << stdDevSteps << "\n";
std::cout << "StdDev Susceptible:     " << stdDevSusceptible << "\n";
std::cout << "StdDev Infectious:      " << stdDevInfectious << "\n";
std::cout << "StdDev Recovered:       " << stdDevRecovered << "\n";
std::cout << "StdDev Vaccinated:      " << stdDevVaccinated << "\n";


}


int main() {
    // Load INI file
    INIReader reader("disease_in.ini");

    if (reader.ParseError() < 0) {
        std::cerr << "Can't load 'disease_in.ini'\n";
        return 1;
    }

    // Read disease infection rate
    double infectionRate = reader.GetReal("disease", "transmissability", -1);
    if (infectionRate == -1) {
        std::cerr << "Error reading infection rate.\n";
        return 1;
    }

    Disease disease(infectionRate);  // Initialize the Disease object

    // Number of simulation runs
    int numRuns = reader.GetInteger("global", "simulation_runs", 1);
    if (numRuns <= 0) {
        std::cerr << "Invalid number of runs specified in INI file.\n";
        return 1;
    }

    // Open files for writing
    std::ofstream statsFile("disease_stats.csv");
    std::ofstream detailsFile("disease_details.csv");

    // Write headers for CSV files
    statsFile << "key,value\n";
    detailsFile << "name,infectious,recovered,susceptible,vaccinated\n";

    // Results aggregation
    std::vector<int> totalStepsPerRun;
    std::vector<int> susceptiblePerRun;
    std::vector<int> infectiousPerRun;
    std::vector<int> recoveredPerRun;
    std::vector<int> vaccinatedPerRun;

      SimulationResults simulationResults; // Struct to store cumulative results

    for (int run = 1; run <= numRuns; ++run) {
        std::cout << "Starting Simulation Run " << run << "...\n";

    // Define populations based on INI file
    std::vector<Population> populations;
    std::vector<std::string> populationnames;

    // Read population data from INI file
         int num_Populations = reader.GetInteger("global", "num_Populations", -1);
    for (int i = 1; i <=num_Populations; ++i) {
        std::string section = "population_" + std::to_string(i);
        int size = reader.GetInteger(section, "size", -1);
        double vaccinationRate = reader.GetReal(section, "vaccination_rate", -1);
        bool patient_0 = reader.GetBoolean(section, "patient_0", false);
        std::string name = reader.Get(section, "name", "UNKNOWN");
       // std::cout << "  patient_0  " <<  patient_0  ;
        if (size == -1 || vaccinationRate == -1) {
            std::cerr << "Error reading data for " << section << ".\n";
            return 1;
        }

        populations.emplace_back(size, vaccinationRate,  patient_0, disease);
        populationnames.emplace_back(name);
    }      
    // Open files for writing
   std::ofstream statsFile("disease_stats.csv");
   std::ofstream detailsFile("disease_details.csv");

    // Write headers for CSV files
    statsFile << "key,value\n";
    detailsFile << "name,infectious,recovered,susceptible,vaccinated\n";

    // Step 3: Initialize the community population
    int day = 1;
     bool continueSimulation = true;

     // Simulation loop
    while (continueSimulation) {
       // std::cout << "\nDay " << day << " Simulation:\n";

        
        continueSimulation = false;  // Assume simulation will end unless an infectious person is found
     if (num_Populations > 1) { // Cross-contamination logic only if there are multiple populations  

        // Randomly select two different populations for interaction
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> popDist(0, populations.size() - 1);

    int firstPopIndex, secondPopIndex;
    do {
        firstPopIndex = popDist(gen);
        secondPopIndex = popDist(gen);
    } while (firstPopIndex == secondPopIndex);

    int interactions = populations[firstPopIndex].interactWithPopulation(populations[secondPopIndex], day);
     }
        //process each population
        for (size_t i = 0; i < populations.size(); ++i) {
           

        // Write statistics to the stats file
            int susceptible, infectious, vaccinated, recovered;
            populations[i].getPopulationStats(susceptible, infectious, vaccinated, recovered);
            detailsFile << populationnames[i] << ","
                      << infectious <<  "," << recovered << "," << susceptible << ","<< vaccinated <<"\n";

            populations[i].simulateDay(day);
               
        
            if (populations[i].isInfectiousPresent()) {
                continueSimulation = true;  // Continue simulation if any population has infectious persons
            }
            

        
        }
        
        ++day;      
    }

    for (size_t i = 0; i < populations.size(); ++i) {
    std::string populationName = "Population " + std::to_string(i + 1);
    std::cout << populationnames[i] << " Population Final Status"  << "\n";
    populations[i].displayDailyStatistics(day, populationName);

// Write statistics to the stats file
    int susceptible, infectious, vaccinated, recovered;
    populations[i].getPopulationStats(susceptible, infectious, vaccinated, recovered);
    detailsFile << populationnames[i] << ","
              << infectious <<  "," << recovered << "," << susceptible << ","<< vaccinated <<"\n";
  }


    // Final summary for disease_stats.csv
    int totalSteps = day - 1; // Number of days simulated
    int totalSusceptible = 0, totalInfectious = 0, totalRecovered = 0, totalVaccinated = 0;

    for (const auto& population : populations) {
        int susceptible, infectious, vaccinated, recovered;
        population.getPopulationStats(susceptible, infectious, vaccinated, recovered);

        totalSusceptible += susceptible;
        totalInfectious += infectious;
        totalRecovered += recovered;
        totalVaccinated += vaccinated;

    }

    statsFile << "total_steps," << totalSteps << "\n";
    statsFile << "susceptible_persons," << totalSusceptible << "\n";
    statsFile << "recovered_persons," << totalRecovered << "\n";
    statsFile << "vaccinated_persons," << totalVaccinated << "\n";

    // Store results for this run
        totalStepsPerRun.push_back(totalSteps);
        susceptiblePerRun.push_back(totalSusceptible);
        infectiousPerRun.push_back(totalInfectious);
        recoveredPerRun.push_back(totalRecovered);
        vaccinatedPerRun.push_back(totalVaccinated);

        //simulationResults.update(totalSteps, totalSusceptible, totalInfectious, totalRecovered, totalVaccinated);
        simulationResults.update(totalSteps, totalSusceptible, totalInfectious, totalRecovered, totalVaccinated);

        std::cout << "Simulation Run " << run << " completed after " << totalSteps << " days.\n";
    }

    // Compute averages and standard deviations
    calculateAndPrintStatistics(simulationResults, numRuns);


    // Close files
    statsFile.close();
    detailsFile.close();

    std::cout << "\nSimulation ended after " << numRuns << " days.\n";
    return 0;
}









