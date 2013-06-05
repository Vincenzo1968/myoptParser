myoptParser
===========

C library for parsing command line arguments

1 - Introduction
2 - Roadmap
3 - Building myoptParser 
4 - Installing myoptParser 


1 - Introduction
-----------------
myoptParser is a library to parsing command line arguments.

2 - Roadmap
------------

3 - Building myoptParser
--------------------
To build myoptParser use the following steps:

   1) Unzip and untar the distribution myoptparser-1.0.tar.gz source tarball:
   
      $ tar -jxvf myoptparser-1.0.tar.gz
      $ cd myoptParser-x.y
 OR

   1) Use a git client to retrieve the source code anonymously from
      the project site. Then run the autoreconf utility at the root of
      the myoptParser directory to generate a configure script.

      $ git clone https://github.com/Vincenzo1968/myoptParser.git
      $ cd myoptParser
          
   2) Run the configure script to create myoptParser Makefiles. If the configure
      script fails... now you have a problem.
     
      $ ./configure --help          # (for available configuration options)
      $ ./configure [--with-option1][--with-the-other-option]

      NOTE: You may also run configure from a different directory in order to
      keep your distribution clean, e.g.:

      $ mkdir build
      $ chdir build
      $ ../configure [--with-option1][--with-the-other-option]
          
   3) Run the top level Makefile:
    
      $ make
          
4 - Installing myoptParser
----------------------
To install myoptParser build the install target in the top level Makefile
    
      $ sudo make install
    
