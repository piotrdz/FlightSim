cmake_minimum_required(VERSION 2.8)

add_custom_target( translations ALL
 ./make_pot.sh &&
 ./make_po.sh &&
 ./make_mo.sh &&
 ./install_mo.sh )