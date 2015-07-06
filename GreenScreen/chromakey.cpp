/*********************************************************************
 File:     chromakey.cpp

 Author:   Kevin Jiang

 Email address: kevinjia@usc.edu

 Usage: program_name in.bmp background.bmp dist_threshold out1.bmp out2.bmp

 Notes:
 This program performs the chroma key operation on an input 
 using two different methods.

 Method 1 Utilize a user-defined distance threshold from the
          chromakey value as a discriminator

 Method 2 Devise a method that to determine the chromakey mask
          that doesn't require a user-input threshold

********************************************************************/

#include <cstdio>
#include <iostream>
#include <cstdlib>
#include <cmath>
#include "bmplib.h"

using namespace std;

// Use user-provided threshold for chroma-key distance
// The "output" of this method is to produce a valid mask array
//  where entries in the mask array are 1 for foreground image
//  and 0 for 'background'

void method1(unsigned char inImage[][SIZE][RGB], 
	     bool mask[][SIZE],
	     double threshold)
{

   //Creating chromakey value
   double chromakey_R = 0; //Initialize all chromakey values
   double chromakey_G = 0;
   double chromakey_B = 0;
   double counter = 0; //To count how many pixels in chromakey sample.
   for (int column = 0; column < 10; column++) //Loops through columns
   {
	for (int row = 0; row < 10; row++) //Loops through rows
	{
          chromakey_R += inImage[column][row][0]; //Summing Red value
          chromakey_G += inImage[column][row][1]; //Summing Green value
          chromakey_B += inImage[column][row][2]; //Summing Blue value
          counter = counter + 1; //increases pixel count by 1
	}
   }

   chromakey_R = chromakey_R/counter; //finds average chromakey Red-value
   chromakey_G = chromakey_G/counter; //finds average chromakey Green-value
   chromakey_B = chromakey_B/counter; //finds average chromakey Blue-value

   //scanning image pixels
   for (int column = 0; column < SIZE; column++)
   {
      for (int row = 0; row < SIZE; row++)
      {
         int Red =(int) inImage[column][row][0]; //find pixel Red value
         int Green = (int) inImage[column][row][1]; //find pixel Green value
         int Blue = (int) inImage[column][row][2]; //find pixel Blue value
         int R_dist = chromakey_R - Red;
         int G_dist = chromakey_G - Green;
         int B_dist = chromakey_B - Blue;         
   double d_chromadist = sqrt(pow(R_dist, 2)+pow(G_dist, 2)+pow(B_dist, 2));
         if (d_chromadist >= threshold) //looking to set foreground
         {
             mask[column][row] = 1; //it's a foreground if greater chromadist
         }
         else
         {
             mask[column][row] = 0; //it's a background (just in case)
         }
      }
   }
}

// Devise a method to automatically come up with a threshold
//  for the chroma key determination
// The "output" of this method is to produce a valid mask array
//  where entries in the mask array are 1 for foreground image
//  and 0 for 'background'

void method2(unsigned char inImage[][SIZE][RGB], 
	     bool mask[][SIZE])  
{
   //Creating chromakey value
   double chromakey_R = 0; //Initialize all chromakey values
   double chromakey_G = 0;
   double chromakey_B = 0;
   double counter = 0;
   for (int column = 0; column < 10; column++)
   {
	for (int row = 0; row < 10; row++)
        {
           chromakey_R += inImage[column][row][0];
           chromakey_G += inImage[column][row][1];
           chromakey_B += inImage[column][row][2];
           counter = counter + 1;
        }
   }

   chromakey_R = chromakey_R/counter;
   chromakey_G = chromakey_G/counter;
   chromakey_B = chromakey_B/counter;

//Calculating Foreground Sample Color Distance

double max_dist = 0;
for (int column = 0; column < SIZE; column++) //loop through columns
   {
      for (int row = 0; row < SIZE; row++) //loop through rows
      {
         double Red = (double) inImage[column][row][0];
         double Green = (double) inImage[column][row][1];
         double Blue = (double) inImage[column][row][2];
         double R_dist = chromakey_R - Red;
         double G_dist = chromakey_G - Green;
         double B_dist = chromakey_B - Blue;         
   double d_chromadist = sqrt(pow(R_dist, 2)+pow(G_dist, 2)+pow(B_dist, 2));
         if (d_chromadist > max_dist) //if chromadist > last documented max
         {                            //chromadistance
             max_dist = d_chromadist; //then set that as the new max_dist
         }
      }
   }

const double scale = 4.1; //scaling factor
double threshold = max_dist/scale; //somewhere between 0 and max_dist is
for (int column = 0; column < SIZE; column++) //the ideal threshold value.
   {
      for (int row = 0; row < SIZE; row++)
      {
         double Red = (double) inImage[column][row][0];
         double Green = (double) inImage[column][row][1];
         double Blue = (double) inImage[column][row][2];
         double R_dist = chromakey_R - Red;
         double G_dist = chromakey_G - Green;
         double B_dist = chromakey_B - Blue;         
   double d_chromadist = sqrt(pow(R_dist, 2)+pow(G_dist, 2)+pow(B_dist, 2));
	 if (d_chromadist > threshold)
         {
             mask[column][row] = 1;
         }
	 else
         {
             mask[column][row] = 0;
         }
       }
   }
}

// If mask[i][j] = 1 use the input image pixel for the output image
// Else if mask[i][j] = 0 use the background image pixel
void replace(bool mask[SIZE][SIZE],
	     unsigned char inImage[SIZE][SIZE][RGB],
	     unsigned char bgImage[SIZE][SIZE][RGB],
	     unsigned char outImage[SIZE][SIZE][RGB])
{
   for (int column = 0; column < SIZE; column++) //loop through columns
   {
      for (int row = 0; row < SIZE; row++) // loop through rows
      {
         if (mask[column][row] == 1) //if foreground
         {
            outImage[column][row][0] = inImage[column][row][0]; //use input
            outImage[column][row][1] = inImage[column][row][1];
            outImage[column][row][2] = inImage[column][row][2];
         }
         else //if background
         {
            outImage[column][row][0] = bgImage[column][row][0]; //use bground
            outImage[column][row][1] = bgImage[column][row][1];
            outImage[column][row][2] = bgImage[column][row][2];
         }
      }
   }
}

int main(int argc, char *argv[])
{
  // Image data array
  // Note:  DON'T use the static keyword except where we expressly say so.
  //        It puts the large array in a separate, fixed, area of memory. 
  //        It is bad practice. But useful until we have dynamic allocation.
  static unsigned char inputImage[SIZE][SIZE][RGB];
  static unsigned char bgrndImage[SIZE][SIZE][RGB];
  static unsigned char outputImage[SIZE][SIZE][RGB];
  static bool chromaMask[SIZE][SIZE];

  double threshold;

  if (argc < 6) {
    cerr << "usage: program_name in.bmp background.bmp dist_threshold " 
         << "out1.bmp out2.bmp" << endl;
    return 0;
  }
	
  if (readRGBBMP(argv[1], inputImage)) {
    cerr << "Error reading file: " << argv[1] << endl;
    return 1;
  }

  if (readRGBBMP(argv[2], bgrndImage)) {
    cout << "Error reading file: " << argv[2] << endl;
    return 1;
  }
  
  // Write code to convert the threshold (argv[3])
  //  from string format to a double and assign the 'threshold'
   threshold = atof(argv[3]);

  // Call Method 1 Function
   method1(inputImage, chromaMask, threshold);

  // Produce the output by calling replace()
   replace(chromaMask, inputImage, bgrndImage, outputImage);

  // Write the output image to a file using the filename argv[4]
  if (writeRGBBMP(argv[4], outputImage)) {
    cout << "Error writing file: " << argv[4] << endl;
    exit(1);
  }	

  // Call Method 2 Function
   method2(inputImage, chromaMask);

  // Produce the output by calling replace()
   replace(chromaMask, inputImage, bgrndImage, outputImage);

  // Write the output image to a file using the filename argv[5]
  if (writeRGBBMP(argv[5], outputImage)) {
    cout << "Error writing file: " << argv[5] << endl;
    exit(1);
  }	

  return 0;
}

