# Extensions Names

set extname(PanA,2,1) im1
set extname(PanA,1,1) im2

# Detectors

set mosname KOSMOS
set mossize "\[1:2048,1:4096\]"

set detname(im1) "K1234"

set detsize(im1) "\[1:1024,1:4096\]"
set detsize(im2) "\[1:1024,1:4096\]"

# Geometry.

set detsec(im1) "\[1:1024,1:4096\]"
set detsec(im2) "\[1025:2048,1:4096\]"

set datasec(im1) "\[1:1024,1:4096\]"
set datasec(im2) "\[1:1024,1:4096\]"

set trimsec(im1) "\[1:1024,1:4096\]"
set trimsec(im2) "\[1:1024,1:4096\]"

set biassec(im1) "\[1025:1074,1:4096\]"
set biassec(im2) "\[1025:1074,1:4096\]"
