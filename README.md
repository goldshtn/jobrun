### jobrun

Run a process in a Win32 job object and control its resource quotas. Win32 job objects provide resource quotas and other limits that can be applied to Windows processes. This tool assigns these limits to the newly created process.

Usage:

```
jobrun - run a process inside a job and limit its behavior
         copyright (C) 2017 Sasha Goldshtein

USAGE: jobrun [-M MEGABYTES] [-m MEGABYTES] [-w MEGABYTES] [-c SECONDS]
              [-n NUMPROCS] [-b yes|no] [-a AFFINITY] [-p PRIORITY]
              [-s SCHEDCLASS] [-r CPURATE] [-t CPUWEIGHT] [-u UIRESTRS]
              <application>

  -M MEGABYTES   Limit the total committed memory of the job's processes
  -m MEGABYTES   Limit the committed memory of each of the job's processes
  -w MEGABYTES   Limit the process working set of each of the job's processes (soft limit)
  -c SECONDS     Limit the total CPU time of the job's processes
  -n NUMPROCS    Limit the number of processes in the job
  -b yes|no      Allow job processes to break away
  -a AFFINITY    Set the processor affinity of the job's processes
  -p PRIORITY    Set the priority class of the job's processes
  -s SCHEDCLASS  Set the scheduling class (0-9) of the job's processes
  -r CPURATE     Set the portion (%) of the CPU cycles this job's threads can use
  -t CPUWEIGHT   Set the scheduling weight (1-9) of the job object
  -u UIRESTRS    Set the UI restriction class for the job's processes, a bitmask:
                     1 - prevent using USER handles from other processes
                     2 - prevent reading the clipboard
                     4 - prevent writing the clipboard
                     8 - prevent changing system parameters with SystemParametersInfo
                    16 - prevent changing display settings with ChangeDisplaySettings
                    32 - prevent accessing global atoms
                    64 - prevent creating desktops and switching desktops
                   128 - prevent shutting down or restarting with ExitWindows(Ex)
```

Examples:

* Launch Notepad so that it cannot access the clipboard: `jobrun -u 6 notepad`
* Launch a suspicious process but disallow it to create children: `jobrun -n 1 suspicious.exe`
* Run with 1% of CPU time per scheduling interval: `jobrun -r 1 hungry.exe`
* Limit memory usage and CPU affinity: `jobrun -m 100 -a 1 hungry.exe`

To build, open the solution in Visual Studio (tested with 2017) and build.