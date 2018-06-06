/*Megan Chipman (m3700960)
 *CAP 4453 - Robot Vision
 *Programming Assignment 2: Canny Edge Detector
 *March 21, 2018
 */

/* Canny.c */
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#define PICSIZE 256
#define MAXMASK 100

//Declare and Initialize Global Variables:
int pic [PICSIZE][PICSIZE]; 
int flags[PICSIZE][PICSIZE];
double histogram[PICSIZE]; 
double outpicx[PICSIZE][PICSIZE]; 
double outpicy[PICSIZE][PICSIZE]; 
double maskx[MAXMASK][MAXMASK];
double masky[MAXMASK][MAXMASK]; 
double ival[PICSIZE][PICSIZE],maxival;
double convx[PICSIZE][PICSIZE];
double convy[PICSIZE][PICSIZE]; 
double peaks[PICSIZE][PICSIZE]; 

//Begin Main Function: 
int main(int argc,char **argv)
{
    //Declare and Initialize Variables: 
    int i, j, p, q, s, t, mr, centx, centy, moretodo, end;  
    double maskvalx, maskvaly, sum, sig, maxival, slope, HI, LO, percent, cutoff, areaOfTops; 
    FILE *original, *magnitude, *peakMap, *doubleThreshold; 

    sig = 1.0;   
    percent = 0.053; 
    mr = (int)(sig * 3); 
    centx = (MAXMASK / 2);
    centy = (MAXMASK / 2);  

    original = fopen("garb34.pgm", "rb"); 
    magnitude = fopen("CannyMagnitude.pgm", "wb");
    peakMap = fopen("CannyPeaks.pgm", "wb");  
    doubleThreshold = fopen("CannyThresholds.pgm", "wb"); 

    //Add Headers to Output Files: 
    fprintf(magnitude, "P5\n");
    fprintf(magnitude, "%d %d\n", 256, 256);
    fprintf(magnitude, "255\n");
    
    fprintf(peakMap, "P5\n");
    fprintf(peakMap, "%d %d\n", 256, 256);
    fprintf(peakMap, "255\n");

    fprintf(doubleThreshold, "P5\n");
    fprintf(doubleThreshold, "%d %d\n", 256, 256);
    fprintf(doubleThreshold, "255\n");

    //Read in Original Image: 
    for (i = 0; i < PICSIZE; i++)
    {
        for (j = 0; j < PICSIZE; j++)
        {
            pic[i][j] = getc(original); 
            pic[i][j] &= 0377;
        }
    }
    

    //Utilize First Gaussian Derivatives for x and y:
    for (p = -mr; p <= mr; p++)
    {
        for (q = -mr; q <= mr; q++)
        {
            maskvalx = (p * exp(-((p*p)+(q*q))/(2*(sig*sig)))); 
            (maskx[p+centy][q+centx]) = maskvalx; 
        }
    } 

    for (p = -mr; p <= mr; p++)
    {
        for (q = -mr; q <= mr; q++)
        {
            maskvaly = (q * exp(-((p*p)+(q*q))/(2*(sig*sig)))); 
            (masky[p+centy][q+centx]) = maskvaly; 
        }
    } 

    
    //Perform Convolution with x and y Masks: 
    for (i = mr; i < PICSIZE - mr; i++)
    {
        for (j = mr; j < PICSIZE - mr; j++)
        {
            sum = 0; 
            for (p = -mr; p <= mr; p++)
            {
                for (q = -mr; q <= mr; q++)
                {
                    sum += pic[i+p][j+q] * maskx[p+centy][q+centx]; 
                }
            }

            outpicx[i][j] = sum; 
            convx[i][j] = sum; 
        }
    }

    for (i = mr; i < PICSIZE - mr; i++)
    {
        for (j = mr; j < PICSIZE - mr; j++)
        {
            sum = 0; 
            for (p = -mr; p <=mr; p++)
            {
                for (q = -mr; q <= mr; q++)
                {
                    sum += pic[i+p][j+q] * masky[p+centy][q+centx]; 
                }
            }

            outpicy[i][j] = sum;
            convy[i][j] = sum; 
              
        }
    }
    
    //Compute the Magnitude using Maskx and Masky: 
    maxival = 0; 

    for (i=mr;i<PICSIZE-mr;i++)
    { 
        for (j=mr;j<PICSIZE-mr;j++)
        {
            ival[i][j]=sqrt((double)((outpicx[i][j]*outpicx[i][j]) +
                           (outpicy[i][j]*outpicy[i][j])));
            
            if (ival[i][j] > maxival)
            {
                maxival = ival[i][j];                        
            }
                
        }
    }

    //Apply Scale and Wrap-Around for Magnitude Image: 
    for (i=0;i<PICSIZE;i++)
    { 
        for (j=0;j<PICSIZE;j++)
        {
            ival[i][j] = (ival[i][j] / maxival) * 255; 
        }
    }
    

    //Distinguish Peaks to Create Peak Image: 
    for (i = 0; i < PICSIZE-mr; i++)
    {
        for (j = 0; j < PICSIZE-mr; j++)
        {
            if (convx[i][j] == 0.0)
            {
                convx[i][j] = 0.00001; 
            }

            slope = (convy[i][j] / convx[i][j]); 

            if ((slope <= tan(22.5) + 1.0) && (slope > tan(-22.5)+ 1.0))
            {
                if ((ival[i][j] > ival[i][j-1]) && (ival[i][j] > ival[i][j+1]))
                {
                    peaks[i][j] = 255; 
                }
            }

            else if ((slope <= tan(67.5)+ 1.0) && (slope > tan(22.5)+ 1.0))
            {
                if ((ival[i][j] > ival[i-1][j-1]) && (ival[i][j] > ival[i+1][j+1]))
                {
                    peaks[i][j] = 255; 
                }
            }

            else if ((slope <= tan(-22.5)+ 1.0) && (slope > tan(-67.5)+ 1.0))
            {
                if ((ival[i][j] > ival[i+1][j-1]) && (ival[i][j] > ival[i-1][j+1]))
                {
                    peaks[i][j] = 255; 
                }
            }

            else
            {
                if ((ival[i][j] > ival[i-1][j]) && (ival[i][j] > ival[i+1][j]))
                {
                    peaks[i][j] = 255; 
                }
            }
        }
    }

    //Output Original Peak Map: 
    for (i=0;i<PICSIZE;i++)
    { 
        for (j=0;j<PICSIZE;j++)
        {
            //The original calculation of ival will be placed into the magnitude image: 
            fprintf(peakMap, "%c", (char)((int)(peaks[i][j]))); 
        }
    }

    //Compute Histogram: 
    for (i = 0; i < PICSIZE; i++)
    {
        for (j = 0; j < PICSIZE; j++)
        {
            histogram[(int)ival[i][j]]++; 
        }
    }

    end = 0; 
    areaOfTops = 0; 
    cutoff = percent * PICSIZE * PICSIZE; 

    //Use Histogram and Percent to Compute HI and LO: 
    for (i = PICSIZE; i > 0 && end != 1; i--)
    {
        areaOfTops += histogram[i]; 
        if (areaOfTops > cutoff)
        {
            end = 1;  
        }
    }

    HI = i; 
    LO = 0.35 * HI; 


    //Apply Double Threshold of HI and LO: 
    for (i = 0; i < 256; i++)
    {
        for (j = 0; j < 256; j++)
        {
            if(peaks[i][j] == 255)
            {
                if (ival[i][j] > HI)
                { 
                    peaks[i][j] = 0; 
                    flags[i][j] = 255; 
                }

                else if (ival[i][j] < LO)
                {
                    peaks[i][j] = 0;
                    flags[i][j] = 0;  
                }
            }
        }
    }
 
    moretodo = 1; 
    while (moretodo == 1)
    {
        moretodo = 0; 
        for (i = 0; i < PICSIZE-mr; i++)
        {
            for (j = 0; j < PICSIZE-mr; j++)
            {
                if (peaks[i][j] == 255)
                {
                    for (p = -1; p <= 1; p++)
                    {
                        for (q = -1; q <= 1; q++)
                        {
                            if (flags[i+p][j+q] == 255)
                            {
                                peaks[i][j] = 0; 
                                flags[i][j] = 255; 
                                moretodo = 1; 
                            }
                        }
                    }
                }
            }
        }
    }

    //Check for proper output (should be similar to magnitude image): 
    for (i=0;i<PICSIZE;i++)
    { 
        for (j=0;j<PICSIZE;j++)
        {
            fprintf(magnitude, "%c", (char)((int)(ival[i][j])));
            fprintf(doubleThreshold, "%c", (char)((int)(flags[i][j]))); 
        }
    }

    //Close Utilized Files: 
    fclose(original); 
    fclose(magnitude); 
    fclose(peakMap); 
    fclose(doubleThreshold); 

}