#include "simbox.h"
#include <iostream>
#include <random>
#include <cstdlib>  // For rand() and srand()
#include <ctime>    // For time()


Simbox::Simbox(void) {     // constructor
    //define the urns' size here based on the number of individuals
    std::vector<int> u(n + 1, 0);
    urns = u;

    std::random_device rd; //give ramdom seed
    std::mt19937 gen(rd()); //random number generator
    std::uniform_int_distribution<> distr(0, n);
    for (int i = 0; i < m; i++) {
        int randomCoopNum = distr(gen); //generate random number between 0 and n
        urns[randomCoopNum]++;
    }
}

int Simbox::RunSimulation() {
    while (urns[0] != m && urns[n] != m) { //not at absorbing state
        std::vector<double> leavingRate_urns = GetTotalLeavingRates();
        double lambda = std::accumulate(leavingRate_urns.begin(), leavingRate_urns.end(), 0.0);
        double tau = -log(1 - (double)rand() / RAND_MAX) / lambda;  //The time between each independet event which follows poisson distribution has exponential distribution

        //Get the urn from which a ball leaves
        int leavingRateIdx = draw_random_urn(leavingRate_urns, lambda);

        // Compute the total arriving rate for the chosen urn
        std::vector<double> destinationRate_urns = GetTotalDestinationRates(leavingRateIdx);
        double totalGi = std::accumulate(destinationRate_urns.begin(), destinationRate_urns.end(), 0.0);

        // Get the urn to which a ball arrives
        int ArrivingUrnIdx = draw_random_urn(destinationRate_urns, totalGi);

        // Select the urn based on the random number and the rates
        urns[leavingRateIdx]--;
        urns[ArrivingUrnIdx]++;

        time += tau;
    }

    return (urns[0] == m) ? 0 : 1;
}

int Simbox::draw_random_urn(const std::vector<double>& urns, double totalRate) {
    std::random_device rd; // give random seed
    std::mt19937 gen(rd()); // random number generator
    std::vector<double> prob_vec(urns.size(), 0.0);
    for (int i = 0; i <= n; i++) {
        prob_vec[i] = urns[i] / totalRate; // normalize the rates
    }
    std::discrete_distribution<int> distr(prob_vec.begin(), prob_vec.end());
    return distr(gen); // generate random number between 0 and n
}

std::vector<double> Simbox::GetTotalLeavingRates(){
    double sum = 0;
    std::vector<double> group_level_urns(n + 1, 0.0);
    for (int i = 0; i <= n; i++) {
        group_level_urns[i] = Miu(i) * w * (1 + (double)r * i / n);
        sum += group_level_urns[i];
    } 
    // Calculate leaving rates for each urn
    std::vector<double> leavingRate_urns(n + 1, 0.0);
    leavingRate_urns[0] = (sum - group_level_urns[0]) * m * Miu(0);
    for (int i = 1; i < n; ++i) {
        leavingRate_urns[i] = m * Miu(i) * (n - i) * ((double)i / n) * (2 + s) 
        + m * Miu(i) * (sum - group_level_urns[i]);
    }
    leavingRate_urns[n] = m * Miu(n) * (sum - group_level_urns[n]);
    return leavingRate_urns;
}

std::vector<double> Simbox::GetTotalDestinationRates(int LeavingUrnIdx) {
    std::vector<double> destinationRate_urns(n + 1, 0.0);
    for (int i = 0; i <= n; i++) {
        if (i == LeavingUrnIdx) {
            destinationRate_urns[i] = 0.0; // No rate for the urn from which a ball leaves
        } else if (i == LeavingUrnIdx - 1 && LeavingUrnIdx < n) {
            destinationRate_urns[i] = (n - LeavingUrnIdx) * (1 + s) * ((double)LeavingUrnIdx / n) 
            + Miu(LeavingUrnIdx - 1) * (1 + (double)r * (LeavingUrnIdx - 1) / n) * w;
        } else if (i == LeavingUrnIdx + 1 && LeavingUrnIdx > 0) {
            destinationRate_urns[i] = LeavingUrnIdx * (1 - (double)LeavingUrnIdx / n) 
            + Miu(LeavingUrnIdx + 1) * (1 + (double)r * (LeavingUrnIdx + 1) / n) * w;
        } else {
            destinationRate_urns[i] = Miu(i) * (1 + (double)r * i / n) * w; // Group level events
        }
    }
    
    return destinationRate_urns;
}

double Simbox::Miu(int coop_num) { //u(i/n)
    return (double)urns[coop_num] / m;   
}


void Simbox::Report() {
    int ball_total = 0;   
    for (int i = 0; i <= n; i++) {
        std::cout << "Check Urn " << i << " and it has " << urns[i] << " balls" << std::endl;
        ball_total += urns[i];
    }
    std::cout << "Total number of balls: " << ball_total << std::endl;
}

Simbox::~Simbox() {

}