Jonathan Lee
Niharika Nagar
Sehaj Singh


Assignment 7 Readme


In our implementation of mathfs, we utilize a struct called pathInfo that contains all the path information (pathname, document description, number of arguments per operation, and a function pointer to the particular operation). For example, the operation add would be represented in a table of paths as {“add”, “Adds 2 numbers…”, 3, add}. We use the first parameter to compare with the input of the user. The second parameter is what goes in the document that is created. The third parameter is 3 because it goes under add/2/2 where you add 2 and 2. The fourth is a reference to the function pointer add.


As specified in the documentation for this assignment, we followed closely to the hello example as provided by the lovely people at FUSE. Because this assignment requires more than just one path, we parse through the path and split it using strtok. We also have to loop through the different folders to make sure everything is in order. We will describe the minute changes to these functions:


* We also utilize timestamp in mathfs_getattr so that the user knows the access time when doing ls or shell operation. 
* In mathfs_readdir we use the filler function like in the FUSE hello example. We have one for the current directory “.”, the parent directory “..”, and depending on the input, a directory for each input if “/” and “doc” if the input is within a specific function.


As far as the math functions add, sub, mul, div, exp, fib, factor, we create a global variable answer so each function can return the answer. For factor, we utilize an extra function prime which is a basic homework assignment for students in CS111. Because the input is a string, we convert it using atof. We store it in answer via sprintf and utilize a nice shade of cyan. Any kind of error is in red and is capitalized so that the user knows they done goofed.