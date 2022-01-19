#!/bin/bash

protoc --cpp_out=pb -I"proto/doc/proto/trunk" `ls proto/doc/proto/trunk/*.proto`
