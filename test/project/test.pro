CONFIG -= qt
CONFIG += debug


INCLUDEPATH += ../../quadtree_mapping/inc

HEADERS = ../../quadtree_mapping/inc/node.h \
          ../../quadtree_mapping/inc/quadtree.h \
          ../../quadtree_mapping/inc/virtualortoscan.h \
          ../../quadtree_mapping/inc/virtualscan.h


SOURCES = ../../quadtree_mapping/src/node.cpp \
          ../../quadtree_mapping/src/quadtree.cpp \
          ../../quadtree_mapping/src/virtualortoscan.cpp \
          ../../quadtree_mapping/src/virtualscan.cpp \
		  ../src/main.cpp

TARGET = quadtree_mapping_test
