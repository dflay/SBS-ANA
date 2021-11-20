# SBS-ANA
Data analysis code for the JLab SBS Run Group of experiments 

# Setting Up the Environment 

In order to use the analysis scripts on the ifarm, the user needs to 
modify the `~/setup/setup.csh` script and source it before running 
analysis code.  This script defines paths that the SBS analysis framework 
needs to identify the offline/nearline analyer source code, replay scripts, 
and output directory locations. 

The `~/setup/rootrc` file gives an example of a ROOT environment setup script. 
The user should modify this as needed to point at their own installations and 
output directory locations.  This file should be saved as `.rootrc` in their 
working analysis directory; this is loaded automatically when calling the 
`analyzer' or `root'.

# Analysis Code 

Analyzer/ROOT scripts for the beam analysis are located in the `beam` directory. 
These range from simple checks of the various beam-related systems, like the 
raster, BPMs, BCMs, and the Unser.  There are also more applied scripts that 
extract the beam charge for a given run, and extracting the BCM calibration 
coefficients from a dedicated set of BCM calibration data runs.  These 
scripts will be available soon.   
