CS 543, Fall 2013 Homework 5, Due by class time (6PM) on Tuesday, December 3, 2013     

<h3> Homework 5 Overview</h3>


In this project you shall implement some of the image manipulation
operations we discussed in class. 
The following <a href="http://web.cs.wpi.edu/~emmanuel/courses/cs543/f13/homework/HW5/cs543_proj5_starter.zip">  [ Visual Studio solution ] </a>
contains a working  program that reads in and displays a bmp file usain_bolt.bmp.
Your task will be to program various GLSL fragment shaders
that implement various operations that we learned in class
and control the effects interactively using keystrokes.
To start off, download the Visual Studio Solution, compile it
and run it. I have tested it and it works in the Zoolab. <br><br>

To read in the usain_bolt.bmp file, the program uses use the 
<a href="https://github.com/chazomaticus/libbmpread"> [ libbmpread library ] </a> which is a tiny, fast 
bitmap (.bmp) image file loader. Specifically, libbmpread is
implemented as two files bmpread.c and bmpread.h which have
been included in the Visual Studio starter code. bmp file 
loading functions and data structures are then used in
the starter program to load the bmp file. <br><br>


<h3> The Assignment </h3>

You should implement some of the image-manipulation
operations previously discussed in class. Your program
behavior should be as follows. <br><br>

User hits:

<ul>
<li><b>Key 'O':</b> Display original picture (usain_bolt.bmp)
</li><li><b>Key 'L':</b> Display picture with Luminance effect (gray scale)
</li><li><b>Key 'N':</b> Display image negative
</li><li><b>Key 'D':</b> Display image with edge detection effect
</li><li><b>Key 'E':</b> Display image with embossing effect
</li><li><b>Key 'T':</b> Display image with Toon rendering
</li><li><b>Key 'W':</b> Display image with Twirl Transformation
</li><li><b>Key 'P':</b> Display image with Ripple Transformation
</li><li><b>Key 'S':</b> Display image with Spherical Transformation
</li></ul>


<h3>Submitting Your Work</h3>

<font size="2" ,="" face="Helvetica,Arial,sans-serif">

Make sure to double-check that everything works before submitting.
Submit all your executable and source files. 
Put all your work files (Visual Studio solution, OpenGL program, shaders,
executable and input files into a folder and zip it. Essentially,
after your project is complete, just zip the project directory created
by Visual Studio. Submit your project using web-based turnin. <br><br>


Create documentation for your program and submit it along with the
project inside the zip file. Your documentation can be either a pure
ASCII text or Microsoft Word file. The documentation does not have to
be long. Briefly describe the structure of your program, what each file
turned in contains. Explain briefly what each module does and tie in
your filenames. Most importantly, give clear instructions on how to
compile and run your program. MAKE SURE IT RUNS before submission.
Name your zip file according to the convention <i>FirstName_lastName_hw5.zip</i><br><br>
