#version 310 es 
#include "constants.h"
layout(location = 0) flat in highp uint in_nodeid;

layout(early_fragment_tests) in;

layout (location = 0) out highp uint out_node_id;

void main()
{
    out_node_id = in_nodeid;
}