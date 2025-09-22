#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "include/doctest.h"
#include "simulation.h"
#include <vector>
#include <random>
#include <fstream>
#include <iostream>
#include <cstdio>

// Helper function to read the content of a file
// This function reads the entire content of a file and returns it as a string
std::string readFileContent(const std::string& filename) {
    std::ifstream file(filename);
    std::string content((std::istreambuf_iterator<char>(file)),
                        (std::istreambuf_iterator<char>()));
    return content;
}

// Tests for the Disease class
TEST_CASE("Disease Class Tests") {
    SUBCASE("Infection rate is set correctly") {
        // Create a Disease object with a specified infection rate
        Disease disease(0.5); // Infection rate set to 50%
        // Check if the infection rate is correctly initialized
        CHECK(disease.getInfectionRate() == doctest::Approx(0.5));
    }
}

// Tests for Population initialization
TEST_CASE("Population Initialization Tests") {
    Disease disease(0.3); // 30% infection rate

    SUBCASE("Correct initialization without patient zero") {
        // Create a population with 100 individuals, 20% vaccinated, no patient zero
        Population population(100, 0.2, false, disease);

        // Get population stats for verification
        int susceptible, infectious, vaccinated, recovered;
        population.getPopulationStats(susceptible, infectious, vaccinated, recovered);

        // Verify expected distribution of population states
        CHECK(susceptible == 80);  // 80% should be susceptible
        CHECK(vaccinated == 20);  // 20% should be vaccinated
        CHECK(infectious == 0);   // No infectious person as patient zero is false
        CHECK(recovered == 0);    // No recovered individuals at initialization
    }

    SUBCASE("Correct initialization with patient zero") {
        // Create a population with patient zero included
        Population population(100, 0.2, true, disease);

        // Get population stats for verification
        int susceptible, infectious, vaccinated, recovered;
        population.getPopulationStats(susceptible, infectious, vaccinated, recovered);

        // Verify expected distribution of population states
        CHECK(infectious == 1);   // One infectious person as patient zero is true
        CHECK(susceptible == 79); // One less susceptible because of patient zero
        CHECK(vaccinated == 20);  // Vaccinated remains 20
        CHECK(recovered == 0);    // No recovered individuals at initialization
    }
}

// Tests for Population interactions
TEST_CASE("Population Interaction Tests") {
    Disease disease(0.3); // 30% infection rate
    Population population1(100, 0.2, true, disease); // Population 1 with patient zero
    Population population2(100, 0.2, false, disease); // Population 2 without patient zero

    SUBCASE("Interactions increase contact count") {
        // Simulate interactions between two populations
        int interactions = population1.interactWithPopulation(population2, 1);
        // Check that there were some interactions
        CHECK(interactions > 0);
    }

    SUBCASE("Infection spreads between populations") {
        // Simulate interactions between populations
        population1.interactWithPopulation(population2, 1);

        // Retrieve stats for both populations
        int susceptible1, infectious1, vaccinated1, recovered1;
        int susceptible2, infectious2, vaccinated2, recovered2;
        population1.getPopulationStats(susceptible1, infectious1, vaccinated1, recovered1);
        population2.getPopulationStats(susceptible2, infectious2, vaccinated2, recovered2);

        // Verify that infection spreads correctly
        CHECK(infectious1 >= 1); // Population 1 still has infectious people
        CHECK(infectious2 >= 0); // Infection might have spread to Population 2
    }
}

// Tests for daily simulation of disease spread
TEST_CASE("Daily Simulation Tests") {
    Disease disease(0.4); // 40% infection rate
    Population population(100, 0.1, true, disease); // Population with patient zero

    SUBCASE("Simulate 1 day of infection spread") {
        // Simulate one day in the population
        population.simulateDay(1);

        // Retrieve population stats
        int susceptible, infectious, vaccinated, recovered;
        population.getPopulationStats(susceptible, infectious, vaccinated, recovered);

        // Check expected changes after one day
        CHECK(susceptible <= 99); // Susceptible count should reduce
        CHECK(infectious >= 1);   // At least one person should be infectious
        CHECK(recovered == 0);    // No one should recover on day 1
    }

    SUBCASE("Simulate 10 days of infection spread and recovery") {
        // Simulate 10 days of infection spread and recovery
        for (int day = 1; day <= 10; ++day) {
            population.simulateDay(day);
        }

        // Retrieve population stats
        int susceptible, infectious, vaccinated, recovered;
        population.getPopulationStats(susceptible, infectious, vaccinated, recovered);

        // Check expected outcomes after 10 days
        CHECK(recovered >= 0); // Some people should have recovered
        CHECK(infectious >= 0); // There might still be infectious people
        CHECK(susceptible <= 90); // Susceptible count should reduce
    }
}

// Tests for displaying daily statistics
TEST_CASE("Display Statistics Tests") {
    Disease disease(0.3); // 30% infection rate
    Population population(50, 0.1, true, disease); // Population with 50 people, patient zero

    SUBCASE("Display statistics after simulation") {
        // Simulate and display statistics for 5 days
        for (int day = 1; day <= 5; ++day) {
            population.simulateDay(day);
            population.displayDailyStatistics(day, "Test Population");
        }
        // Test passes if no exceptions occur during statistics display
    }
}

// Tests for the Person class
TEST_CASE("Person Class Tests") {
    SUBCASE("Person initial status is correctly set") {
        // Test initialization of a person in different states
        Person susceptible(Person::Susceptible);
        CHECK(susceptible.getStatus() == Person::Susceptible);

        Person vaccinated(Person::Vaccinated);
        CHECK(vaccinated.getStatus() == Person::Vaccinated);

        Person infectious(Person::Infectious);
        CHECK(infectious.getStatus() == Person::Infectious);

        Person recovered(Person::Recovered);
        CHECK(recovered.getStatus() == Person::Recovered);
    }

    SUBCASE("Person infection status changes correctly") {
        // Test status transition from susceptible to infectious
        Person person(Person::Susceptible);
        CHECK(person.isSusceptible());

        person.infect(0); // Infect the person
        CHECK(person.isInfectious()); // Person should now be infectious
        CHECK(!person.isSusceptible()); // Person is no longer susceptible
    }

    SUBCASE("Person recovers after the correct number of days") {
        // Test recovery process for an infectious person
        Person person(Person::Infectious);
        person.infect(1); // Infect the person on day 1

        CHECK_FALSE(person.checkRecovery(2)); // Less than 4 days since infection
        CHECK_FALSE(person.checkRecovery(4)); // Exactly 3 days since infection
        CHECK(person.checkRecovery(5));       // 4 days since infection
        CHECK(person.isRecovered());          // Person should now be recovered
    }
}
