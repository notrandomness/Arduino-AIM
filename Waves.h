#ifndef OCEANWAVES_H
#define OCEANWAVES_H
#include "colorutils.h"


class Waves
{
  private:
    double waves[35][3] = {{}}; // speed and position and wavelength: [[s,p,w], [s,p,w],...]
    uint8_t maxWavelength = 40;
    uint8_t maxSpeed = 3;
    float waterArray[360] = {};
  public:

    Waves()
    {
      initializeWaves();
    }

    void initializeWaves()
    {
      for (auto& wave : this->waves)
      {
        wave[0] = (random(100, (this -> maxSpeed )*100)/100) * ( random(2) == 0? 1: -1);//speed
        wave[1] = random(360);//position
        wave[2] = random(20, this->maxWavelength);//wavelength
      }
    }

    void updateWavePositions()
    {
      for (auto& wave : this->waves)
      {
        wave[1] += wave[0];
        if (wave[1] < 0)
        {
          wave[0] = (random(100, this -> maxSpeed *100)/100) * (  random(2) == 0? 1: -1);//speed
          wave[1] = 359;//position
          wave[2] = random(20, this->maxWavelength);//wavelength
        }
        else if (wave[1] > 359) //Generate new random seed when it goes outside of strip-space
        {
          wave[0] = (random(100, this -> maxSpeed *100)/100) * (  random(2) == 0? 1: -1);//speed
          wave[1] = 0;//position
          wave[2] = random(20, this->maxWavelength);//wavelength
        }
      }
    }

    void updateWaves()
    {
      for (int i = 0; i < 360; i++)
      {
        waterArray[i] = 0;
      }
      for (auto& wave : this->waves)
      {

        int position = wave[1];
        int wavelength = wave[2];
        int index = position;
        for (int i = 0; i < wavelength; i++)
        {
          if (index >= 360)
          {
            break;
          }
          waterArray[index] += waveHeight(i, wavelength);
          index++;
        }
      }
      updateWavePositions();
    }

    float waveHeight(float index, float wavelength)
    {
      float ratio = index/wavelength;
      float radians = ratio * 2 * PI;
      //Serial.println(ratio);
      return (sin(radians) * 10);
    }

    CHSV waterColor(float height)
    {
      //height of single wave is -10 to 10
      // -20 should be solid blue, 20 should be white
      float percentWhite = ((height + 20)/40);
      if (percentWhite < .4) // 80% solid blue then 10% white-spectrum
      {
        percentWhite = .4;
      }
      //percentWhite =  (percentWhite - 0.4) ;
      if (percentWhite > 1){
        percentWhite = 1;
      }
      int white = 255* percentWhite;
      if (white > 255)
      {
        white = 255;
      }
      else if (white < 0)
      {
        white = 0;
      }
      //Serial.println(white);
      //128 to 160
      int hue = 160 - (32 * percentWhite);

      return CHSV( hue, 255, white);
    }

    CHSV trippyColor(float height)
    {
      //height of single wave is -10 to 10
      // -20 should be 0, 20 should be 255
      float percentWhite = ((height + 20)/40);
      if (percentWhite < .4) // 80% solid blue then 10% white-spectrum
      {
        percentWhite = .4;
      }
      //percentWhite =  (percentWhite - 0.4) ;
      if (percentWhite > 1){
        percentWhite = 1;
      }
      int white = 255* percentWhite;
      if (white > 255)
      {
        white = 255;
      }
      else if (white < 0)
      {
        white = 0;
      }
      //Serial.println(white);
      //128 to 160
      int hue = ((int)(((height + 20)/40)*255))%255;

      return CHSV( hue, 255, white);
    }

    CHSV getTrippyLEDAtIndex(int i)
    {
      return trippyColor(waterArray[i+30]);
    }

    CHSV getTopLEDAtIndex(int i)
    {
      return waterColor(waterArray[i+30]);
    }

    CHSV getMidLEDAtIndex(int i)
    {
      return waterColor(waterArray[i+30]);
    }

    CHSV getBotLEDAtIndex(int i)
    {
      return waterColor(waterArray[i+30]);
    }
};

#endif