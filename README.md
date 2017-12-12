### jobrun

Run a process in a Win32 job object and control its resource quotas. Win32 job objects provide resource quotas and other limits that can be applied to Windows processes. This tool assigns these limits to the newly created process.

Usage:

```
jobrun - run a process inside a job and limit its behavior
         copyright (C) 2017 Sasha Goldshtein

USAGE: jobrun [-m MEGABYTES] [-w MEGABYTES] [-c SECONDS] [-n NUMPROCS]
              [-b yes|no] [-a AFFINITY] [-p PRIORITY] [-s SCHEDCLASS]
              <application>

  -m MEGABYTES   Limit total committed memory of the job's processes
  -w MEGABYTES   Limit the process working set of the job's processes (soft limit)
  -c SECONDS     Limit the total CPU time of the job's processes
  -n NUMPROCS    Limit the number of processes in the job
  -b yes|no      Allow job processes to break away
  -a AFFINITY    Set the processor affinity of the job's processes
  -p PRIORITY    Set the priority class of the job's processes
  -s SCHEDCLASS  Set the scheduling class (0-9) of the job's processes
```

To build, open the solution in Visual Studio (tested with 2017) and build.