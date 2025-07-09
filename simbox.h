#pragma once

#include <vector>
#include <functional>



class Simbox {
    public:
        Simbox();
        ~Simbox();
        int RunSimulation();
        double GetTotalLeavingRate();
        double RateLeavingUrn(int coop_num);
        int ChooseUrnTo(double totalRate, std::function<double(int)> rateFunc);
        //int ChooseUrnTo(double totalRate);
        double GetTotalArrivingRate(int LeavingUrnIdx);
        double RateArrivingUrn(int arrivingUrn, int leavingUrn);
        //int ChooseUrnTo2(double totalRate, int LeavingUrnIdx);
        double Miu(int coop_num);
        void Report();

        std::vector<double> GetTotalLeavingRates();
        int draw_random_urn(const std::vector<double>& urns, double totalRate);
        std::vector<double> GetTotalDestinationRates(int LeavingUrnIdx);

        int m = 50;  //total numbers of balls
        int n = 50;  //individuals' number in ball is n = 20

        std::vector<int> urns;   //the index represents the cooperator numbers in the ball

        float s = 0.5; //replication rate difference between cooperators and defectors
        float r = 0.5; //selection coefficient at the group level 
        float w = 10; //ratio of the rate of group-level events to the rate of individual-level events.
        double time = 0;
};
