## Getting set up

The multiquark analysis software is divided up into two parts. One is a private 
repo hosted on GitLab, which requires CERN credentials to access,
and can be found [here](https://gitlab.cern.ch/slavoie/MyAnalysis). The second
part is hosted on my public GitHub account and can be found
[here](https://github.com/simlav000/Multiquark).

## GitLab Repo
This repo is my project which uses the AnalysisBase 25.2.2 release build 
provided by ATLAS software. 

# lxplus
It is intended to be ran on `lxplus`, a linux server hosted by cern
which can be connected to by issuing the command `ssh [username]@lxplus.cern.ch`,
which requires a cern account.


To get it up and running, it is recommended to simply clone the repo in your personal 
`eos` space which can be found at `/eos/user/[first letter of username]/[username]`.
This is likely not your `$HOME` (`~`), but I would recommend making this 
place your home as the eos space not only has much more disk space available 
for data output, but can also be accessed from the web via [cernbox](https://cernbox.cern.ch/).

# Cloning the repo
Then, simply go to the [gitlab page](https://gitlab.cern.ch/slavoie/MyAnalysis),
look for the blue "Code" button, and copy the "Clone with SSH" link and issue 
the command `git clone [link]`.

This project follows the [atlassoftwaredocs project structure](https://atlassoftwaredocs.web.cern.ch/analysis-software/AnalysisSWTutorial/).
There are lots of resources which can be found by following the ATLAS Software Documentation,
and it is possible that following this tutorial will become mandatory as the practices and 
software releases are constantly changing. These instructions work as of August 20th 2024.
Sorry if things are outdated by the time you read this. It's not my fault.

# Project Architecture
Now that you have cloned the `MyAnalysis` directory, let's talk a bit about what is inside, in order of importance.
- **CMakeLists_TopLevel.txt**: This file is the main configuration file for the CMake program, which is used for 
                               building and compiling our project. It will have to be renamed.
- **CMakeLists.txt**: This file is a secondary configuration file used by CMake to manage our custom headers,
                      as well as other third-party (probably ATLAS-owned) packages we wish to include, etc., etc.
- **MultiQuark**: This directory contains all the C++ headers for the project. 
- **Root**: Not to be confused with the ROOT program, this is just the actual C++ source files.
- **share**: This directory holds python scripts - called "steering macros" - which take care of configuring and 
               executing the analysis script. There are two main versions of the steering macro. The first is 
               `ATestRun_eljob.py` which is the file you execute to run on local data. The file contains a line 
               designating a file path containing data to run on. It iterates over all files in the specified 
               directory to pass as input to the job. The second file, `ATestSubmit_eljob.py`, is used to run 
               the job on the GRID. Running on the GRID unlocks vast computing resources and allows access to 
               all ATLAS datasets, which is particularly useful for running on datasets too large to download.
- **data**: This directory contains a `config.yaml` file used to configure CP (Combined Performance) algorithms. 
              This was meant to be used to configure the GoodRunsList algorithm, but I never had the time to make 
              it work, unfortunately. 
- **python**: This is where python scripts used to schedule predefined algorithms to execute should go. I also 
                never made any use of this.
- **src**: This is where Athena sources and private header files go. Given this is not an Athena algorithm, 
             there is nothing interesting going on here either.

# Building the project (Initial Setup)
As of now, all you have are the source files. In order to execute the program we must first 
build the project. It is recommended you navigate to the `MyAnalysis` directory, and execute the following commands:
- `mkdir -p source/MultiQuark && mv * source/MultiQuark`: This will tell you that you can't move source into itself. That's ok.
       The important bit is that you've moved all the contents of `MyAnalysis` two layers deeper into to the `source/MultiQuark` 
       directory.
- `mv source/MultiQuark/CMakeLists_TopLevel.txt source/CMakeLists.txt`: Move the top-level CMakeLists.txt to the 
                                                  source directory and rename it to `CMakeLists.txt`. We have to do it like this
                                                  because each of these files must have exactly this name for CMake 
                                                  to know what to do with them, and we can't have two files with the 
                                                  same name in the same directory. This file now sits at the root of 
                                                  our project, being the first file named `CMakeLists.txt` which 
                                                  will be found when we build the project.
- `mkdir build`: This will create a directory which will hold all the makefiles generated by the CMake program 
                 which manages the building and compiling for us.
- `cd build`: Move to the build directory.
- `setupATLAS`: This command should be executed on every login, and will be needed to setup the tools we are 
                about to use, as well as other tools regarding downloading datasets and running on the GRID.
- `asetup AnalysisBase,25.2.2`: This sets up the correct AnalysisBase release for our project. I believe this
                                initializses some environment variables to allow CMake to find ATLAS's C++ 
                                software headers and such.
- `cmake ../source/`: This executes CMake using the top-level CMakeLists.txt file as instructions for building the 
                      project as a whole, and the secondary CMakeLists.txt file as configuration for setting up and 
                      linking libraries and the like.
- `source x86_64*/setup.sh`: This is the final step of the initial setup. Sourcing this file allows you to execute the job.
- `cd ..`: Now let's run the job.
- `mkdir run`: Create a run directory.
- `ATestRun_eljob.py --submission-dir=submitDir`: This is the command used to run on local files. It executes the previously 
                                                  mentioned steering macro, and creates a directory for the job to output its 
                                                  data. This directory is automatically sybolically linked, so you can always 
                                                  access the contents of the latest submission by navigating to `submitDir`. 
                                                  This bit will only work if you happen to have the permissions to MY eos 
                                                  space. This is because the `ATestRun_eljob.py` is currently configured to 
                                                  look in my personal files for the data to run on. If you do not have
                                                  the correct permissions here, follow the steps outlined in the "Using Rucio" 
                                                  section further down this page.

# Building the project (Subsequent Setup)
Once you've build the project a single time, building the project becomes simpler. The only three commands you must input are:
- `setupATLAS`
- `asetup AnalysisBase,25.2.2`
- `source MyAnalysis/build/x86_64*/setup.sh`
It is recommended to simply put these three commands in a script and make a habit of sourcing this file 
on login. Just make sure your path to the CMake-generated `setup.sh` is correct.

