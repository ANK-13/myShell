# myShell
This is the basic shell developed using C language and process invokations in Linux. This shell is tested in ubuntu 18 LTS.
This shell developed has various features as below:

              1. One can run commands as in bash shell. The output is show just after it.
              2. `printenv` is custom commands that gives all the environment variables.			
              3. `Ctrl-C` does not terminate the programme. It gives warning stating to use `Ctrl-D` instead. Ctrl-D properly shuts down the shell.
              4. In order to run commands at background add additional arguments `&` at the end.This allows to run command in the background. On running command you can get process ID of background command.
              5. All the processes running in background are added in a Linked List. You can view the 
              list by custom command `showjobs`. This command shows the number of process in linked list. 
              There process ID and there status. This status is mentioned using following letter. 
              There description is as follows:
               D    uninterruptible sleep (usually IO)
               R    running or runnable (on run queue)
               S    interruptible sleep (waiting for an event to complete)
               T    stopped by job control signal
               t    stopped by debugger during the tracing
               W    paging (not valid since the 2.6.xx kernel)
               X    dead (should never be seen)
               Z    defunct ("zombie") process, terminated but not reaped by its parent
