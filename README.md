# embedded-system-design

The aim of this final project is to show understanding of various topics learned throughout the COE718 course. The 2 main topics this final project will exploit are Joinable Threads & Priority Inversion. 

To explore the concept of Joinable Threads, I created an embedded system that is able to receive input from a user and schedule 3 intercommunicating threads. By utilizing previous knowledge and experience working with joystick inputs explored in previous labs, it will be used to retrieve a user input and schedule a certain thread. 

Thread 2 or Thread 3 will be scheduled when a user inputs JOYSTICK_UP or JOYSTICK_DOWN and sets a value of a global variable, option, that will inform the action Thread 1 should perform respectively. After completion of Thread 2 or Thread 3, Thread 1 should either light up LEDs from GPIO1 or GPIO2. 

E.g

Case 1: JOYSTICK_UP → Thread 2 → option = 2 → Thread 1 → GPIO1 LEDs 

Case 2: JOYSTICK_DOWN → Thread 3 → option = 3 → Thread 1 → GPIO2 LEDs 

To explore the concept of Priority Inversion, I implemented 3 modified versions of the priority inversion code we received in Lab 4.

 Two versions implemented the CMSIS-RTOS built-in mutex and semaphores respectively, and the last version implemented my own custom logic similar to the principles of mutexes. All 3 versions solve the problem of Priority Inversion. 

 DESIGN 

Joinable Threads
For the joinable thread section of the project, I implemented and scheduled 4 threads; Main thread, Thread 1, Thread 2, and Thread 3. Each thread performs a certain action, and communicates with at least one additional thread. 

For more details on what each thread will implement, please refer to the definitions below;


Main thread — Receive joystick inputs from the user and schedule either Thread 2 or Thread 3 as a result of the input. Afterwards, this thread will schedule Thread 1 to perform a certain action. Note: this thread should repeat periodically so a user may continue to input a different joystick input. 


Thread 1 — This thread will light up a certain sequence of LEDS on either GPIO1 or GPIO2 based on a global variable called option.  Afterwards, this thread will be blocked.  


Thread 2 — This thread will perform a calculation (which theoretically may be useful such as calculating temperature of the system’s environment) and set option = 2. Afterwards, it will signal the Main thread to continue execution and this thread will be blocked.


Thread 3 — This thread will perform a calculation (which theoretically may be useful such as calculating the current time) and set option = 3. Afterwards, it will signal the Main thread to continue execution and this thread will be blocked.


The Threads were scheduled in a non-prementive manner. That is, use of the processor is never taken from the thread. Instead, the thread must voluntarily signal the processor to run other threads instead[1]. 

For further understanding, I have also created a block diagram for graphical purposes which can be found in the Appendix Section. 

Priority Inversion 
Going into more detail, to solve the issue of Priority Inversion, a technique called Priority Inheritance will be used. In Lab 4, three threads of different priorities were scheduled — high, medium, and low. Since there were three threads involved in the Priority Inversion problem, this is a case of Unbounded Priority Inversion. 


In Unbounded Priority Inversion, the highest priority thread has to wait for the medium and low priority before executing[2]. In RTOS this is a huge problem as priorities and deadlines are critical for correctness and performance. By utilizing Priority Inheritance, we allow the lowest priority thread to execute at the same priority as the high priority thread. After completion of the low priority thread, it will get set back to its original priority, and the high priority thread will start executing.


Another issue presented in the Priority Inversion code from Lab 4 is the concept of a shared resource. In the source code, Thread 1(high priority thread) & Thread 3(low priority thread) have a shared resource in common (led[0]). In order for this embedded system to run properly, some sort of synchronization mechanism is required. This is where my custom mutex is implemented. 


Following the definition of how mutexes work. I essentially lock a resource and allow a thread to perform its critical section. This is done through introducing a boolean variable called mutex, which will be used to determine if a shared resource is available. If mutex = true, then that shared resource is available. Else, mutex = false and the shared resource is currently in use by a particular thread. 

Very similar methodology is applied to my built-in versions for mutex and semaphores. That is, for a thread accessing a shared resource, first retrieve a mutex/semaphore. Afterwards, run your critical section of the code that uses the shared resource and release the mutex/semaphore[3].
