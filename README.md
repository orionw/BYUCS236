# BYUCS236
A interpreter for the Datalog Language - made for CS236 at BYU

Details on the individual labs are found on the website at:
https://cswiki.cs.byu.edu/cs-236/projects

I have tagged in the releases the final version of each lab.  Each tag/release also contains the zip file for the code that I used to pass of the lab.  

Please note that the most recent commits were an experimental change to the lab and do not reflect the most complete project - see Lab5Done tag for the most complete code.

An example testcase of the Datalog language is:
```
Schemes:
    f(A,B)
    g(C,D)
    r(E,F)

Facts:
    f('1','2').
    f('4','3').
    g('3','2').
    r('1','4').
    r('2','5').
    r('3','5').
    r('4','1').
 
Rules:
    r(A,B) :- f(A,X),g(B,X).
    f(C,D) :- r(D,C).
    g(E,F) :- f(E,X),r(X,F).

Queries:
    f('3',Z)?
    r(Y,'3')?
    f(W,X)?
```
    
    
For more test cases and examples on the individual labs see https://faculty.cs.byu.edu/~barker/cs236/tests/tests.php.

For a sample test case driver, in case you'd like to run multiple tests at once see https://github.com/JustBrenkman/cs236_testbench
