![Spatial Collision Test Image](http://i.imgur.com/VqIZUMS.png)  

This project is a benchmark tool used to assess performance of different collision detection data structures.

Most of these data structures were taken Christer Ericson's Real-Time collision detection (Morgan Kaufmann Publishers 2004) book.

The Kd-Tree SAH idea was taken from Maxim Shevtsov, Alexei Soupikov, Alexander  Kapustin,
Intel  Corporation, 2007, "Highly Parallel Fast KD‐tree Construction for Interactive Ray Tracing of Dynamic Scenes" paper.

The implemented data structures are:  
* Bruteforce (for reference)  
* Sort and Sweep (Sweep and Prune)  
* Uniform Grid  
* Hierarchical Grid  
* Octree  
* Loose Octree  
* Kd-Tree (using SAH)  

A small demo application was written (using GLUT and FF OpenGL) to help visualize and compare the tested data structures.


Please see LICENSE file for License information.  
All code is (c) Mykola Konyk, 2008.
