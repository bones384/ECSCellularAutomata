//
// Created by mkowa on 02.04.2024.
//

#include "coinflip.h"
std::uniform_int_distribution<> roll::RNG::uniformDistribution{0, 1};
std::uniform_int_distribution<> roll::RNG::percentile{1, 100};
bool roll::flipCoin() { roll::RNG r1; return roll::RNG::uniformDistribution(r1.generator);}
bool roll::percentileRoll(int chance)
{
    roll::RNG r1;
    if(chance>=r1.percentile(r1.generator)) return true;
    return false;
}