#!/bin/bash

if [ "$#" -ne "0" ]; then
  LAB_NUM="000$1"
  DIR="./${LAB_NUM: -3:3}"
  echo $DIR
else
  echo "No lab number provided..."
fi
