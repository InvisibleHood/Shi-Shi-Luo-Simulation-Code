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
        double lambda = GetTotalLeavingRate();
        double tau = -log(1 - (double)rand() / RAND_MAX) / lambda;  //The time between each independet event which follows poisson distribution has exponential distribution
        // int LeavingUrnIdx = ChooseUrnTo(lambda);
        // double totalGi = GetTotalArrivingRate(LeavingUrnIdx);
        // int ArrivingUrnIdx = ChooseUrnTo2(totalGi, LeavingUrnIdx);
        //Get the urn from which a ball leaves
        int LeavingUrnIdx = ChooseUrnTo(lambda, [this](int i) {
            return RateLeavingUrn(i); // Compute leaving rates
        });

        // Compute the total arriving rate for the chosen urn
        double totalGi = GetTotalArrivingRate(LeavingUrnIdx);

        // Get the urn to which a ball arrives
        int ArrivingUrnIdx = ChooseUrnTo(totalGi, [this, LeavingUrnIdx](int i) {
            return RateArrivingUrn(i, LeavingUrnIdx); // Compute arriving rates
        });


        // Select the urn based on the random number and the rates
        urns[LeavingUrnIdx]--;
        urns[ArrivingUrnIdx]++;

        time += tau;
    }

    return (urns[0] == m) ? 0 : 1;
}

int Simbox::ChooseUrnTo(double totalRate, std::function<double(int)> rateFunc) {
    std::random_device rd;       // Provide a random seed
    std::mt19937 gen(rd());      // Random number generator

    std::vector<double> prob_vec(urns.size(), 0.0);
    double sum = 0;

    for (int i = 0; i <= n; i++) {
        double rate = rateFunc(i); // Use the provided function to compute the rate
        prob_vec[i] = rate / totalRate;
        sum += rate;
    }

    if (sum != totalRate) {
        std::cout << "Error: sum of rates is not equal to total rate" << std::endl;
    }

    std::discrete_distribution<int> dd(prob_vec.begin(), prob_vec.end());
    return dd(gen);
}



// int Simbox::ChooseUrnTo(double totalRate) {
//     std::random_device rd; //give ramdom seed
//     std::mt19937 gen(rd()); //random number generator

//     std::vector<double> prob_vec(urns.size(), 0.0);
//     double sum = 0;
//     for (int i = 0; i <= n; i++) {
//         double rate = RateLeavingUrn(i);
//         prob_vec[i] = rate / totalRate;
//         sum += rate;
//     }
//     if (sum != totalRate) {
//         std::cout << "Error: sum of rates is not equal to total rate" << std::endl;
//     }
//     std::discrete_distribution<int> dd(prob_vec.begin(), prob_vec.end());
//     return dd(gen);
// }


// int Simbox::ChooseUrnTo2(double totalRate, int LeavingUrnIdx) {
//     std::random_device rd; //give ramdom seed
//     std::mt19937 gen(rd()); //random number generator

//     std::vector<double> prob_vec(urns.size(), 0.0);
//     double sum = 0;
//     for (int i = 0; i <= n; i++) {
//         double rate = RateArrivingUrn(i, LeavingUrnIdx);
//         prob_vec[i] = rate / totalRate;
//         sum += rate;
//     }
//     if (sum != totalRate) {
//         std::cout << "Error: sum of arriving rates is not equal to total rate" << std::endl;
//     }
//     std::discrete_distribution<int> dd(prob_vec.begin(), prob_vec.end());
//     return dd(gen);
// }


double Simbox::GetTotalArrivingRate(int LeavingUrnIdx) {
    double totalGi = 0.0;
    for (int i = 0; i <= n; i++) {
        totalGi += RateArrivingUrn(i, LeavingUrnIdx);
    }
    return totalGi;
}

double Simbox::GetTotalLeavingRate() {
    double lambda = 0;
    for (int i = 0; i <= n; i++) {
        lambda += RateLeavingUrn(i);
    }
    return lambda;
}


double Simbox::RateArrivingUrn(int arrivingUrn, int leavingUrn) {
    if (arrivingUrn == leavingUrn) {
        return 0;
    }
    if (arrivingUrn == leavingUrn - 1 && leavingUrn < n) {
        return (n - leavingUrn) * (1 + s) * ((double)leavingUrn / n) 
        + Miu(leavingUrn - 1) * (1 + (double)r * (leavingUrn - 1) / n) * w;
    } 
    
    if (arrivingUrn == leavingUrn + 1 && leavingUrn > 0) {
        return leavingUrn * (1 - (double)leavingUrn / n) 
        + Miu(leavingUrn + 1) * (1 + (double)r * (leavingUrn + 1) / n) * w;
    } 
    //group level events
    return Miu(arrivingUrn) * (1 + (double)r * arrivingUrn / n) * w;
}

double Simbox::RateLeavingUrn(int coopNum){
    double sum = 0;
    for (int i = 0; i <= n; i++) {
        if (i != coopNum) {
            sum += Miu(i) * w * (1 + (double)r * i / n);
        }
    }
    double leavingRate = 0.0;
    if (coopNum > 0 && coopNum < n) {
        leavingRate = m * (Miu(coopNum)) * (n - coopNum) * ((double)coopNum / n) * (2 + s) 
        + m * Miu(coopNum) * sum;
        return leavingRate;
    }
    leavingRate = m * Miu(coopNum) * sum;
    return leavingRate;
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