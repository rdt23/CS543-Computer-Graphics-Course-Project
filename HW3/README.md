CS 543, Fall 2013 Homework 3: Hierarchical 3D Modeling of L-System Trees

<table cellspacing="0" width="100%">
  <tbody><tr>
    <th align="left" width="15%" valign="top">Objective:</th>
    <td align="left" width="85%">In this assignment, you will learn how to generate a forest of trees using
      an iterated function system (IFS) called Lindenmayer Systems (a.k.a. L-Systems), and place those trees
      on a "ground plane." You shall also insert PLY models of a car and other PLY objects and place it on the ground plane
      to model a park.
      This assignment consists of two parts: a "Preparation" part
      and a "Submitted Stuff" part.
			<p>
      <b>NOTE: Please submit ONLY the work described below in the section "submitted stuff" by the due date (Oct 29, 2013). 
        Do NOT submit the work you do in the "prep coding" section</b>

  </p></td></tr>
  <tr>
    <td><br></td>
  </tr>
  <tr>
    <th align="left" valign="top">Preparation:</th>
    <td align="left">
The aim of this preparation part is for you to create the IFS for generating the
strings that will define each tree for the forest. In addition, you will create a
PolyCylinder routine to better understand transformations (<i>e.g.</i>, translations, rotations) in OpenGL.

<p>
A drawing pattern is defined by a <i>turtle string</i> made up of command characters that
control how the turtle moves, as well as its state. The commands include:
</p><p>
<table cellpadding="2" border="2">
  <tbody><tr>
    <th>Character</th>
    <th align="left">Meaning</th>
  </tr>

  <tr>
    <td align="center">F</td>
    <td>Move forward a step of length <i>len</i>, drawing a line (or cylinder) to the new point.</td>
  </tr>
  <tr>
    <td align="center">f</td>

    <td>Move forward a step of length <i>len</i> without drawing.</td>
  </tr>
  <tr>
    <td align="center">+</td>
    <td>Apply a positive rotatation about the X-axis of <i>xrot</i> degrees.</td>

  </tr>
  <tr>
    <td align="center">-</td>
    <td>Apply a negative rotatation about the X-axis of <i>xrot</i> degrees.</td>
  </tr>
  <tr>

    <td align="center">&amp;</td>
    <td>Apply a positive rotatation about the Y-axis of <i>yrot</i> degrees.</td>
  </tr>
  <tr>
    <td align="center">^</td>
    <td>Apply a negative rotatation about the Y-axis of <i>yrot</i> degrees.</td>

  </tr>
  <tr>
    <td align="center">\</td>
    <td>Apply a positive rotatation about the Z-axis of <i>zrot</i> degrees.</td>
  </tr>
  <tr>

    <td align="center">/</td>
    <td>Apply a negative rotatation about the Z-axis of <i>zrot</i> degrees.</td>
  </tr>
  <tr>
    <td align="center">|</td>
    <td>Turn around 180 degrees.</td>

  </tr>
  <tr>
    <td align="center">[</td>
    <td>Push the current state of the turtle onto a pushdown stack.</td>
  </tr>
  <tr>
    <td align="center">]</td>

    <td>Pop the state from the top of the turtle stack, and make it the current turtle stack.</td>
  </tr>
</tbody></table>
</p><p>
L-Systems are used to generate a turtle string by iteratively expanding a <i>start token</i>
by applying production (or re-writing) rules. Each L-System consists of a grammar that
defines re-writing rules. Each rule in the grammar consists of a left-hand side (LHS) and a
right-hand side (RHS), separated by a colon.
</p><p>
A sample grammar looks like this:
</p><p>
<font size="+2">
</font></p><pre><font size="+2">start: F-F-F-F
F: F-F+F+FF-F-F+F

</font></pre><font size="+2">
</font>
<p>
In addition to specifying a grammar, we also need to specify the values for
<i>len</i>, <i>xrot</i>, <i>yrot</i>, and <i>zrot</i>, as well as a value denoting
how many times we want to iterate (<i>i.e.</i>, apply the production rule(s)).
</p><p>
Similar to the way the Koch curve is created by replacing each segment with a predefined
pattern, the turtle string is rewritten by substituting every instance of a LHS by its
corresponding RHS. For any token in the string for which there is no matching LHS, the token
is simply copied into the new string. For example, given the grammar:
</p><p>

<font size="+2">
</font></p><pre><font size="+2">start: F+F
F: F-F+F-F
</font></pre><font size="+2">
</font>
<p>
after one iteration, the resulting turtle string would be:
</p><p>
<font size="+2">
</font></p><pre><font size="+2">F-F+F-F+F-F+F-F
</font></pre><font size="+2">
</font>
and after two iterations, the resulting turtle string would be:
<p>
<font size="+2">
</font></p><pre><font size="+2">
F-F+F-F-F-F+F-F+F-F+F-F-F-F+F-F+F-F+F-F-F-F+F-F+F-F+F-F-F-F+F-F
</font></pre><font size="+2">
</font>
<p>
Your system should be able to handle multiple production rules, each having a unique LHS, for example:
</p><p>
<font size="+2">
</font></p><pre><font size="+2">start: X
X: F-[[X]+X]+F[+FX]-X
F: FF
</font></pre><font size="+2">
</font>
Here is an example (in 2D) of some grammars and their corresponding trees. Can you extend these to 3D for
use in this project? (The value of <i>n</i> is the number of iterations, and the angle specified is the
rotation for the "+" and "-" characters.)
<p>
<img src="LsysExamples_sm.jpg">

</p><p>
  </p></td></tr><tr>
    <td><br></td>
  </tr>
  <tr>
    <th align="left" valign="top">Setup:</th>
    <td align="left">
Using the starter code (same one as you got for homework 2) <a href="http://web.cs.wpi.edu/~emmanuel/courses/cs543/f13/homework/HW3/graphics_starting_code_2.zip"> [ here ] </a> or your project 2 code as a starting point, 
add in the other modules you create in this project to render L-systems  (<i>e.g.</i>, grammar.h grammar.cpp, <i>etc.</i>). Be sure to use
the right type of projection matrix and setup parameters to show proper perspective (foreshortening).
    </td>

  </tr>
  <tr>
    <td><br></td>
  </tr>
  <tr>
    <th align="left" valign="top">Prep Coding:</th>
    <td align="left">
<ol>
  <li>

      <b>L-Systems: </b>Write a program that implements the L-System re-writing scheme described above. Your program should
      be able to read several grammars from grammar files, store them in appropriate data structures, and
      then generate the correct turtle strings by applying the rewriting rules as specified.
      <a href="http://web.cs.wpi.edu/~emmanuel/courses/cs543/f13/homework/HW3/lsysSier.txt">Here</a> is a sample L-System file. A description of the file format is contained
      in the file itself, and you may assume that all files have the same format.
      <p>
      Compile and run your program!
  </p></li>
<p>
</p><li>
<b>The PolyCylinder: </b>We define a <i>PolyCylinder</i> as a polyline in three dimensions. Given a sequence of
points in 3-space { (x1, y1, z1), (x2, y2, z2), (x3, y3, z3) }, a polycylinder draws a cylinder from
(x1, y1, z1) to (x2, y2, z2), and another one from (x2, y2, z2) to (x3, y3, z3). To avoid "gaps" in the
joints of the polycylinder, a sphere of the same diameter as the cylinders is drawn at each point.
You can use the following cylinder and sphere ply files (<a href="http://web.cs.wpi.edu/~emmanuel/courses/cs543/f13/homework/HW3/sphere_and_cylinder.zip"> [ Here ] </a>)  for drawing your PolyCylinder.

<p>
You need to write some code that:
</p><ol>
  <li>Moves (translates) to the first point,</li>
  <li>Draws a sphere,</li>
  <li>Points (rotates) towards the next point,</li>
  <li>Draws a cylinder to the next point,</li>
  <li>Moves (translates) to the next point,</li>

  <li>Draws a sphere,</li>
  <li>Points (rotates) towards the next point,</li>
  <li>Draws a cylinder to the next point,</li>
  <li>Moves (translates) to the next point,</li>
  <li>And so on, for all the points in the list.</li>
</ol>
<p>

<b>HINT 1</b>: This part is an exercise in translating and rotating your coordinate system before drawing
your geometry. After you accomplish this, you will understand how transformations are accumulated in OpenGL.
</p><p>
<b>HINT 2</b>: You can simplify your life by always drawing your cylinder along the Z axis. To do this,
apply your rotations such that you are always "moving" along the Z axis by first rotating yourself to point
the current Z direction towards the next point.
</p><p>
Set up a scene that you will use to draw a park with a forest of trees.
Set up a view of your scene,
draw a ground plane, and draw some PolyCylinders in the scene. You can just come up with some
(x, y, z) points on your own for this. Be creative! Write your name, or something.
</p><p>
Compile and run your program!
</p></li>
</ol>
    </td>
  </tr>
  <tr>

    <td><br></td>
  </tr>
  <tr>
    <th align="left" valign="top">Submitted Stuff:</th>
    <td align="left">
Now we need to put it all together.
<ol>
  <li>
Using the code you wrote in the preparation section, your program should read in five L-System files,
"<a href="http://web.cs.wpi.edu/~emmanuel/courses/cs543/f13/homework/HW3/lsys1.txt">lsys1.txt</a>,"
"<a href="http://web.cs.wpi.edu/~emmanuel/courses/cs543/f13/homework/HW3/lsys2.txt">lsys2.txt</a>,"
"<a href="http://web.cs.wpi.edu/~emmanuel/courses/cs543/f13/homework/HW3/lsys3.txt">lsys3.txt</a>," 
"<a href="http://web.cs.wpi.edu/~emmanuel/courses/cs543/f13/homework/HW3/lsys4.txt">lsys4.txt</a>," and ONE OF YOUR OWN, and store them in instances of your <i>grammar</i>

class. You should then apply the re-writing rules for each grammar according to the values specified for this
in each grammar file. Your five L-System files should be drawn on the (a,b,c,d,e) keystrokes respectively. For
instance, hitting keystroke a draws <a href="http://web.cs.wpi.edu/~emmanuel/courses/cs543/f13/homework/HW3/lsys1.txt">lsys1.txt</a>, etc.
  </li>
  <li>
Choose a random location on the ground plane to start drawing one of the randomly selected trees, using a
random color, and draw it.<br>
<b>HINT</b>: You should apply a translation and a rotation to move to the correct start location.
  </li>
  <li>
Repeat Step 2 (at least) 5 times in order to draw your forest. For inspiration, you can look at the 
gallery of forests created by students in a previous CS 4731 class <a href="http://web.cs.wpi.edu/~gogo/courses/cs4731_2006b/lsys/">HERE</a>
Some of the students in that class used a checkerboard floor. You can just make a plain floor.
  </li>

<li>
Make your park look more realistic by adding in more PLY mesh models
such as a car and any other objects you think would make your park look
realistic. Use some of the PLY files from homework 2. You can get the PLY files <a href="http://web.cs.wpi.edu/~emmanuel/courses/cs543/f13/homework/HW3/ply_files.zip"> [ here ] </a> Be creative. 
You don't have to add 3D effects like texturing yet since we 
haven't covered them in class, although people who decide to add any effects won't have points 
taken off.</li>

<li> Keystroke f draws park including your forest of trees, ground plane and car.
</li></ol>
<p>
(If you <b>REALLY</b> want to test your program, try <a href="http://web.cs.wpi.edu/~emmanuel/courses/cs543/f13/homework/HW3/lsys3DHilbert.txt">this</a> input file. This is
from p. 20 of the reference book listed at the bottom of this page and is PURELY OPTIONAL!)
    </p></td>

  </tr>
  <tr>
    <td><br></td>
  </tr>
  <tr>





    <th align="left" valign="top">Attacking the Problem:</th>
    <td align="left">

For the L-System part, start out by creating several classes that will help you manage the different things
you will need to keep straight. For example, you might want to have a <i>turtle</i> class consisting of a
<i>position</i>, <i>orientation</i>, <i>length</i> when drawing, and a <i>string</i> representing the turtle
string.
<p>
You might want to have a <i>rule</i> class that has strings for the <i>lhs</i> and <i>rhs</i>.

</p><p>
A <i>grammar</i> class would consist of a list of <i>rules</i>, along with a method (<i>addRule</i>) to add
a new rule to the grammar. The main method for the grammar class might be something like a <i>rewrite</i> method
that takes in a <i>turtle</i> and a number of <i>iterations</i>, and returns a new string after applying the
rules to the turtle string for the desired number of iterations.

</p><p>
The functionality for implementing '[' and ']' should be achieved by implementing a 
matrix stack and <b>PushMatrix</b> and <b>PopMatrix</b> functions. This will save and return you to the proper
state.
</p><p>
Another thing that will help you greatly is to use the Standard Template Library (STL) that is available
with C++. There are a number of classes that you will find very useful, such as "string," "hash_map,"
"multimap," and/or "list." If you have never used these before, this is a good opportunity for you to
"get out of your comfort zone," as these tools will serve you well in most future endeavors, both in this
class and well beyond.
</p><p>
Some good links to C++ help include <a href="http://www.sgi.com/tech/stl/stl_index_cat.html">the SGI site</a>
and also <a href="http://www.cppreference.com/">CPP Reference</a>.
    </p></td>

  </tr>
  <tr>
    <td><br></td>
  </tr>
  <tr>
    <td><br></td>
  </tr>
  <tr>
    <th align="left" valign="top">Submitting your Work:</th>
    <td align="left">

Make sure to double-check that everything works before submitting.
Submit all your executable and source files. 
Put all your work files (Visual Studio solution, OpenGL program, shaders,
executable and input files into a folder and zip it. Essentially,
after your project is complete, just zip the project directory created
by Visual Studio. Submit your project using web-based turnin. <br><br>


Create documentation for your program and submit it along with the project inside the zip file. 
Your documentation can be either a pure ASCII text or Microsoft Word file.
The documentation does not have to be long. Briefly describe the structure of your program, 
what each file turned in contains. Explain briefly what each module does and 
tie in your filenames. Most importantly, give clear instructions on how
to compile and run your program. <b>MAKE SURE IT RUNS</b> before submission. 
Name your zip file according to the convention <i>FirstName_lastName_hw3.zip</i> <br> <br>



     </td>
  </tr>
  <tr>
    <td><br></td>
  </tr>
  <tr>

    <th align="left" valign="top">Academic<br>Honesty:</th>
    <td align="left">Remember the policy on Academic Honesty: You may discuss the
       assignment with others, but you are to do your own work. The official WPI statement
       for Academic Honesty can be accessed
       <a href="http://www.wpi.edu/offices/policies/honesty/policy.html">HERE</a>.
    </td>
  </tr>
  <tr>
    <td><br></td>
  </tr>

  <tr>
    <th align="left" valign="top">References:</th>
    <td align="left">
    <table cellspacing="4">
      <tbody><tr>
        <td><a href="AlgoBeautyOfPlants.jpg"><img width="141" height="188" src="http://web.cs.wpi.edu/~emmanuel/courses/cs543/f13/homework/HW3/AlgoBeautyOfPlants.jpg"></a></td>
        <td>
          Most of the motivation for my interest in this work comes from the book
          "The Algorithmic Beauty of Plants," by Prezmyslaw Prusinkiewicz and Arstid Lindenmayer,
          Springer Verlag, New York, 1990, ISBN 0-387-97297-8, ISBN 3-540-97297-8.
          <p>

          The whole book has been put <a href="http://algorithmicbotany.org/papers/#abop">online</a> by
          the author. In addition, there are links to other interesting works on this page.
          </p><p>
          You can also find it on <a href="http://www.amazon.com/gp/product/0387946764/002-0022112-1584858?v=glance&amp;n=283155&amp;n=507846&amp;s=books&amp;v=glance">Amazon</a>.
        </p></td>
      </tr>
    
  

</tbody></table>