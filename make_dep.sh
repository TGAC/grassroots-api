#!/bin/bash


MakeDependency()
{
 cd $1
 ./configure --prefix=$2
 make
}


MakeDependency extras/jansson-2.6 /usr

