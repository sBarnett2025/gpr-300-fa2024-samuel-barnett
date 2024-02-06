#version 450
out vec4 FragColor; //The color of this fragment
in vec2 UV;

uniform sampler2D _ColorBuffer;

uniform float _Sharpness;  

void main()
{

    vec2 offsets[9] = vec2[](
        vec2(-_Sharpness,  _Sharpness), // top-left
        vec2( 0.0f, _Sharpness),        // top-center
        vec2( _Sharpness, _Sharpness),  // top-right
        vec2(-_Sharpness, 0.0f),        // center-left
        vec2( 0.0f, 0.0f),              // center-center
        vec2( _Sharpness, 0.0f),        // center-right
        vec2(-_Sharpness, -_Sharpness), // bottom-left
        vec2( 0.0f, -_Sharpness),       // bottom-center
        vec2( _Sharpness, -_Sharpness)  // bottom-right    
    );

    float kernel[9] = float[](
        -1, -1, -1,
        -1,  9, -1,
        -1, -1, -1
    );
    
    vec3 sampleTex[9];
    for(int i = 0; i < 9; i++)
    {
        sampleTex[i] = vec3(texture(_ColorBuffer, UV + offsets[i]));
    }
    vec3 col = vec3(0.0);
    for(int i = 0; i < 9; i++)
        col += sampleTex[i] * kernel[i];
    
    FragColor = vec4(col, 1.0);


    //vec3 color = 1.0-texture(_ColorBuffer,UV).rgb;
	//FragColor = vec4(color,1.0);

}  