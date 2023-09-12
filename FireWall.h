#ifndef FIREWALL_H
#define FIREWALL_H
#include "colorutils.h"


class FireWall
{
  private:
    double fireSeeds[30][3] = {{}}; // speed and position and temp: [[s,p,t], [s,p,t],...]
    uint8_t maxSeedTemp = 80;
    uint8_t maxSeedSpeed = 5;
    int tempArray[300] = {};
    uint8_t radiationSize = 30;
  public:
    FireWall()
    {
      initializeFireSeeds();
    }
    void initializeFireSeeds()
    {
      for (auto& fireSeed : this->fireSeeds)
      {
        fireSeed[0] = (random(1, (this -> maxSeedSpeed )*100)/100) * ( random(2) == 0? 1: -1);//speed
        fireSeed[1] = random(300);//position
        fireSeed[2] = random(10, this->maxSeedTemp);//temperature
      }
    }
    void updateSeedPositions()
    {
      for (auto& fireSeed : this->fireSeeds)
      {
        fireSeed[1] += fireSeed[0];
        fireSeed[2] += random(-2, 3); //random temp fluctuation
        if (fireSeed[2] < 0)
        {
          fireSeed[2] = 0;
        }
        if (fireSeed[1] < 0 || fireSeed[1] > 299) //Generate new random seed when it goes outside of strip-space
        {
          fireSeed[0] = (random(100, this -> maxSeedSpeed *100)/100) * (  random(2) == 0? 1: -1);//speed
          fireSeed[1] = random(300);//position
          fireSeed[2] = random(100, this->maxSeedTemp);//temperature
        }
        if (random(20) == 0) // 1 in 200 chance of changing speed
        {
          fireSeed[0] = (random(100, this -> maxSeedSpeed *100)/100) * ( random(2) == 0? 1: -1);//speed
        }
      }
    }
    void updateTemperatures()
    {
      for (int i = 0; i < 300; i++)
      {
        tempArray[i] = 0;
      }
      for (auto& fireSeed : this->fireSeeds)
      {

        int position = fireSeed[1];
        this->tempArray[position]+= fireSeed[2];
        if (this->tempArray[position] > 255)
        {
          this->tempArray[position] = 255;
        }
        for (int i = 1; i < this->radiationSize; i++)
        {
          int temp = fireSeed[2] - (i * 5); //decreases by 5 degrees for each LED away from seed
          if (temp > 0)
          {
            int backPos = position - i;
            if (backPos >= 0)
            {
              this->tempArray[backPos]+= temp;
              if (this->tempArray[backPos] > 255)
              {
                this->tempArray[backPos] = 255;
              }
            }
            int forPos = position + i;
            if (forPos < 300)
            {
              this->tempArray[forPos]+= temp;
              if (this->tempArray[forPos] > 255)
              {
                this->tempArray[forPos] = 255;
              }
            }
          }

        }
      }
      updateSeedPositions();
    }
    CRGB getTopLEDAtIndex(int i)
    {
      int heat = tempArray[i] - 100;
      heat = heat < 0? 0 : heat;
      return HeatColor(heat);
    }

    CRGB getMidLEDAtIndex(int i)
    {
      int heat = tempArray[i] - 50;
      heat = heat < 0? 0 : heat;
      return HeatColor(heat);
    }

    CRGB getBotLEDAtIndex(int i)
    {
      return HeatColor(tempArray[i]);
    }
};

#endif