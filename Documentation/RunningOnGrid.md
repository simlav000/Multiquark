# Running on the Grid
Starting from a **clean shell**, write the commands:
```bash
$ setupATLAS
$ lsetup panda
```
Now navigate to Multiquark's `build` directory and run:
```bash
$ asetup  --restore
$ source x86_64*/setup.sh
```
Running on grid will require a new steering macro. If one is already present,
you may ignore the rest of this document. If you must set one up, here is 
how you do it. This can be done by simply copying most of the contents of 
`ATestRun_eljob.py` all the way up to, and including the `driver.submit()` 
statement into a new file which we shall name `ATestSubmit_eljob.py`. 
Then, we make the following edits:

1. Change the SampleHandler
    ```python
    # Before
    sample = ROOT.SH.SampleLocal("dataset")
    sample.add(file)
    sh.add(sample)
    ```

    ```python
    # After
    ROOT.SH.scanRucio(sh, "nameOfDataSet")
    ```
2. Replace the driver

    ```python
    #Before
    driver = ROOT.EL.DirectDriver()
    ```
    ```python
    # After
    driver = ROOT.EL.PrunDriver()
    ```
3. Specify structure of output name

    Since it is likely that the input sample has a really long name, we 
    have to provide a shorter one or else Grid will have a hard time handling 
    it. This is done by adding the line:
    ```python
    driver.options().setString("nc_outputSampleName", "user.%nickname%.grid_test_run")
    ```

4. Change submission method

    Simply change the final line `driver.submit()` to `driver.submitOnly()`. 
    **This should be the last line of the file.**

    Finally, ensure that in the project's `CMakeLists.txt`,the following line
    is present:
    ```cmake
    atlas_install_scripts(share/*_eljob.py)
    ```
    and that you call `make` from within the build directory to make the new 
    macro recognized. Once that is done, you can run a grid job by executing
    ```bash
    $ ATestSubmit_eljob.py --submission-dir=submitDir
    ```

Taken from [Atlas Software Tutorial](https://atlassoftwaredocs.web.cern.ch/ABtutorial/grid_running/)
