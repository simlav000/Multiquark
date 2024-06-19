# Running on the Grid
Starting from a **clean shell**, write the commands:
```
$ setupATLAS
$ lsetup panda
```
Now navigate to Multiquark's `build` directory and run:
```
$ asetup  --restore
$ source x86_64*/setup.sh
```
Running on grid will require a new steering macro. This can be done by simply 
copying most of the contents of `ATestRun_eljob.py` all the way up to, and 
including the `driver.submit()` statement into a new file, say `ATestSubmit.py`.



taken from [Atlas Software Tutorial](https://atlassoftwaredocs.web.cern.ch/ABtutorial/grid_running/)
