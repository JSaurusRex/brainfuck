BRAINFUCK - JSaurusRex

for linux users, you can use the brainfuck executable by typing ./brainfuck file.txt

for windows users, the .exe is in bin/release, you have to execute it via terminal: "brainfuck file.txt"


currently there are two ways to run programs:

 $ brainfuck program.bf

  this executes the prorgam with an interperter directly, runs all programs I could find perfectly
  
 $ brainfuck -c program.bf

  this first compiles the prorgam to a more compact version of brainfuck and then runs it.
  
  example of compiling: >>>> becomes 14 | 1 (move) 4 (how far to move)
  -- becomes 0-2 | 0(add)-2(amount to add)
  
  (ps, the -c does not always run perfectly, it is still under development. if a program doens't work with it you will have to run programs without the -c option)