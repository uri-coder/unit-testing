# unit-test

brief: a system for geting report on stm card peripherals and hardwer performants and generating file showing peripheral performants. 
the application asking from card, is name id and a list of periferal tests to run.
after that the application run the tests, sending data to stm, stm perform the test, fills error report and send the tested data back to the app.
app compare the data sent to the data received, also take the time elapsed of the test, 
efter runing all the list of test, application generating rsults file named with card name and uniq id.

the test list and tests setting is written by the hardware specific test devloper.
in the system ther is: 1.pc apllication.   2.stm unit testing src file.    3.stm hardwer specific testing sorce file.
the stm hardwer specific testing sorce file, is ware developer of test, can esley write the test code, for the specific card.

this give great modularity. the system is not designed for a specific stm card. you can easily write any test for any card in the specific test src files and flash the system code to stm card,
connect to pc vie Ethernet run the application, the test will run and a result file will be generated.

the system perform multiple test on the card simultaneously. with freertos on stm and multi threading on pc

the system also allows to test multiple diffrent cards simultaneously.
