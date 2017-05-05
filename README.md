# Quadtree Mapping

## Description

Quadtree mapping is a library that can be used in the context of mobile robotics to obtain a 2D map of the environment the robot is in.
It is not a SLAM (Simultaneour Localization and Mapping) approach.
As input uses:
- the 2D pose of the robot (x, y, orientation)
- A 2D lidar scan (for example an Hokuyo laser was used)
Internally it stores the data in a quadtree data structure.
In the initial commit of main.cpp, you can see out to create as output a cpp file that compiled could create a 2D image of the map.

The original code is pretty primitive and the comments are in Italian.

## Future developments

My idea is to upload the project in gitlab, refactor the code, add meaningful comments in English, add unit tests, split it in a library and some viewer.
As viewer it would be simple for me to create something in QT using QCustomPlot.
To test input data, the data from [RawSeeds](http://www.rawseeds.org/home/) project might be used.

## Credits

Original authors of the projects are:
- [Mauro Brenna](https://www.linkedin.com/in/maurobrenna/)
- [Ivan Reguzzoni](https://www.linkedin.com/in/ivan-reguzzoni-2051438a/)

This project was an [AIRLab](http://airwiki.ws.dei.polimi.it/index.php/2D_Mapping_Using_a_Quadtree_Data_Structure) project.
