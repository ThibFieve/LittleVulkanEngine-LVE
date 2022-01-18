#version 450



//layout(location=0) in vec2 position; // See hand notes on this ! In  key word signifies it takes the value from a vertex buffer , layout location set  the storage where this varaible comes from


//For Triangle

vec2 positions[3]= vec2[](
	vec2(0.0,-0.5),
	vec2(0.5,0.5),
	vec2(-0.5,0.5)
);

void main (){


//gl_Position = vec4(positions,0.0,1.0); // now with the vertex buffer the position is automatically take from the reading of the vertex buffer 
//Vertex shader runs once for each vertex , and each time the position varaible will be having a different value specific to that vertex 

//For triangle
gl_Position = vec4(positions[gl_VertexIndex],0.0,1.0);  // positions[gl_Vertexindex] gives the x and y values of the current vertex gl_Vertexindex conatins the index of the current vertex for each time our main function is run

}
