/*Megan Chipman (m3700960)
 *CAP 4453 - Robot Vision
 *Programming Assignment 1: Elementary Edge Detection
 *February 3, 2018
 */

/* Sobel.c */
#include <stdio.h>                          
#include <math.h>
#include <stdlib.h>

//Declare and Initialize Global Variables
int pic[256][256];
int outpicx[256][256];
int outpicy[256][256];
int maskx[3][3] = {{-1,0,1},{-2,0,2},{-1,0,1}};
int masky[3][3] = {{1,2,1},{0,0,0},{-1,-2,-1}};
double ival[256][256],maxival;

//Begin Main Function
int main(int argc,char **argv)
{
        //Declare and Initialize Variables: 
        int i,j,p,q,mr,sum1,sum2;
        double thresholdLO, thresholdHI;
        FILE *magnitude, *lo, *hi, *original, *fopen();
        
        //Open Original PGM Image to Read and Write: 
        original = fopen("face05.pgm", "rb"); 
        magnitude = fopen("NOThreshold.pgm", "wb");
        lo = fopen("LOThreshold.pgm", "wb");
        hi = fopen("HIThreshold.pgm", "wb"); 
        
        //Initialize Threshold Values: 
        thresholdLO = 24; 
        thresholdHI = 48; 

        //Add Headers to Output Files: 
        fprintf(magnitude, "P5\n");
        fprintf(magnitude, "%d %d\n", 256, 256);
        fprintf(magnitude, "255\n");

        fprintf(lo, "P5\n");
        fprintf(lo, "%d %d\n", 256, 256);
        fprintf(lo, "255\n");

        fprintf(hi, "P5\n");
        fprintf(hi, "%d %d\n", 256, 256);
        fprintf(hi, "255\n");

        //Parse Through Original Image: 
        for (i=0;i<256;i++)
        { 
            for (j=0;j<256;j++)
                {
                    pic[i][j]  =  getc(original);
                    pic[i][j]  &= 0377;
                }
        }

        //Initialize Mask Radius: 
        mr = 1;
        
        //Compute Weighted Sums and Masks: 
        for (i=mr;i<256-mr;i++)
        { 
            for (j=mr;j<256-mr;j++)
            {
                sum1 = 0;
                sum2 = 0;
                
                for (p=-mr;p<=mr;p++)
                {
                    for (q=-mr;q<=mr;q++)
                    {
                        sum1 += pic[i+p][j+q] * maskx[p+mr][q+mr];
                        sum2 += pic[i+p][j+q] * masky[p+mr][q+mr];
                        }
                    }
                    
                    outpicx[i][j] = sum1;
                    outpicy[i][j] = sum2;
                }
            }

            //Initialize maxival: 
            maxival = 0;

            //Compute Magnitude Using Masks Outpicx and Outpicy: 
            for (i=mr;i<256-mr;i++)
            { 
                for (j=mr;j<256-mr;j++)
                {
                    ival[i][j]=sqrt((double)((outpicx[i][j]*outpicx[i][j]) +
                                   (outpicy[i][j]*outpicy[i][j])));
                    
                    if (ival[i][j] > maxival)
                    {
                        maxival = ival[i][j];                        
                    }
                        
                }
            }

            //Utilize Established Thresholds to Determine Whether a Pixel is an Edge: 
            for (i=0;i<256;i++)
            { 
                for (j=0;j<256;j++)
                {
                    //Calculate for Wrap-Around
                    ival[i][j] = (ival[i][j] / maxival) * 255;  

                    //The original calculation of ival will be placed into the magnitude image: 
                    fprintf(magnitude, "%c", (char)((int)(ival[i][j])));

                    //If the ival exceeds the high threshold, then make that pixel white in the high
                    //threshold output image. Otherwise, make that pixel black. 
                    if (ival[i][j] > thresholdHI)
                    { 
                        fprintf(hi, "%c", (char)(255));
                    }

                    else
                    {
                        fprintf(hi, "%c", (char)(0));
                    }

                    //If the ival exceeds the low threshold, but is also lower than the high threshold, then 
                    //make that pixel white in the low threshold output image. Otherwise, make that pixel black. 
                    if (ival[i][j] > thresholdLO)
                    {
                        fprintf(lo, "%c", (char)(255));
                    }

                    else
                    {
                        fprintf(lo, "%c", (char)(0));
                    }
                }
            }

            //Close All Files Used: 
            fclose(original);
            fclose(magnitude);
            fclose(lo); 
            fclose(hi); 
}