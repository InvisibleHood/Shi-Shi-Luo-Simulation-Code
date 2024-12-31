#pragma once

#include <vector>


class Simbox {
    public:
        Simbox();
        ~Simbox();
        int RunSimulation();
        double GetTotalLeavingRate();
        double RateLeavingUrn(int coop_num);
        int ChooseUrnTo(double totalRate);
        double GetTotalArrivingRate(int LeavingUrnIdx);
        double RateArrivingUrn(int arrivingUrn, int leavingUrn);
        int ChooseUrnTo2(double totalRate, int LeavingUrnIdx);
        double Miu(int coop_num);
        void Report();

        int m = 25;  //total numbers of balls
        int n = 49;  //individuals' number in ball is n = 20

        std::vector<int> urns;   //the index represents the cooperator numbers in the ball

        float s = 0.2; //replication rate difference between cooperators and defectors
        float r = 0.2; //selection coefficient at the group level 
        float w = 1; //ratio of the rate of group-level events to the rate of individual-level events.
        double time = 0;
};
