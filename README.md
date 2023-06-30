# script_prototype

A work-in-progress scripting language that will have similar syntactical rules to C

## Code Example

```cpp

char[][] ok = { {'a', 'a'+ 1c }, {'c', ok[1][0] + 1c} };
char ok2 = -(-(++(ok[0] = ok[1])[0]));

```

- These two statements should look familiar to any C programmers as this language is heavily inspired by the C language
- The language is statically typed, so the user needs to define the datatypes he's working with in order to maintain type safety.



### Let's break it down

### 1. statement
```cpp
char[][] ok = { {'a', 'a'+ 1c }, {'c', ok[1][0] + 1c} };
``` 
- Here we declare a 2D array called "ok", which will be initialized with the character values (a,b) and (c,d) 
- Character literals can either be defined by putting the character within the ' marks (like in C), or by putting a "c" suffix after the literal. 
I have implemented the suffix to ensure that you cannot accidentally lose data while assigning to smaller types (implicit narrowing conversions).
 - This language does NOT allow the following expression due to the integer literal being implicitly cast to a char:
```cpp

//problem
char a = 97; //this will fail due to 97 being implicitly cast to a char which would lead to data loss

//solution
char a = 97c; //now the literal is being treated as a char
``` 

### 2. statement

```cpp
char ok2 = -(-(++(ok[0] = ok[1])[0]));
```
- In the language arrays are not treated like pointers (unlike C) because the arrays are dynamic, so expressions like int[] = int[] are allowed
- it should be noticed that array assignments create copies rather than references 
- If you know the operator precedence rules in C, you should be able to figure out that the result of that expression is 'd' :)




## Thank you for reading this little readme about my scripting language :)
-  Note: the language does not have support for anything not shown in this post

### Here's the parser output for the two statements if you are interested:

```
  adding [] type modifier!
  adding [] type modifier!
  declaring a variable of type "char[][]" with the name of "ok"
  EvaluateEntireExpression('a' )
  assigning from ­ -> a
  EvaluateEntireExpression('a' + 1c )
  a + ☺ = b
  assigning from á -> b
  EvaluateEntireExpression('c' )
  assigning from  -> c
  EvaluateEntireExpression(ok [ 1 ] [ 0 ] + 1c )
  EvaluateEntireExpression(1 )
  array lvalue updated
  EvaluateEntireExpression(0 )
  array lvalue updated
  c + ☺ = d
  assigning from ░ -> d
  declaring a variable of type "char" with the name of "ok2"
  EvaluateEntireExpression(- ( - ( ++ ( ok [ 0 ] = ok [ 1 ] ) [ 0 ] ) ) )
  EvaluateEntireExpression(- ( ++ ( ok [ 0 ] = ok [ 1 ] ) [ 0 ] ) )
  EvaluateEntireExpression(++ ( ok [ 0 ] = ok [ 1 ] ) [ 0 ] )
  EvaluateEntireExpression(ok [ 0 ] = ok [ 1 ] )
  EvaluateEntireExpression(0 )
  array lvalue updated
  EvaluateEntireExpression(1 )
  array lvalue updated
  EvaluateEntireExpression(0 )
  array lvalue updated
  incrementing lvalue from c to d
  negating from d to £
  negating from £ to d
  assigning from └ -> d

----------------------------------------------------
script stack:

ok2: <char> (d)
ok:
  [0]: <char>
      [0]: <char> (d)
      [1]: <char> (d)
  [1]: <char>
      [0]: <char> (d)
      [1]: <char> (d)

```

