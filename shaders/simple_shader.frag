#version 450


layout (location = 0) out vec4 outColor; //we output to kocation 0 , type and name, its an output we declare what will be the output of the fragment shader

layout (location = 0) in vec3 inColor;


void main (){
	outColor = vec4(inColor, 1.0) ;// Red Green Blue  Alpha , alpha is for opacity 
}

