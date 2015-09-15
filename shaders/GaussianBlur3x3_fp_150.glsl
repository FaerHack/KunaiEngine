#version 150

in vec4 out_colour;			/* Colour generated by the vertex program */
in vec2 out_tex;			/* Texture coordinates generated by the fragment program */
out vec4 fragment_colour;	/* Final fragment colour */

uniform sampler2D tex0;		/* Texture 0 */
uniform sampler2D tex1;		/* Texture 1 */

/* Screen/viewport resolution */
uniform vec2 dimensions;	

vec4 GaussianBlur() 
{
	vec4 Result;
	vec2 TexCoord;

	mat3 GaussianCoef = mat3(
		1.0,	2.0,	1.0,
		2.0,	4.0,	2.0,
		1.0,	2.0,	1.0);

	float stepU = 1.0 / dimensions.x;
	float stepV = 1.0 / dimensions.y;

	for( int i = 0; i < 3; i++ ) 
	{
		for( int j=0; j < 3; j++ ) 
		{
			TexCoord = out_tex + vec2( (i-1)*stepU, (j-1)*stepV );
			Result += GaussianCoef[i][j] * texture2D( tex1, TexCoord );
		}
	}

	Result /= 16.0;
	return Result;
}


void main(void)
{
	/* Perform gaussian blur */
	vec4 texel = GaussianBlur();

    /* Modulate final colour with vertex colour */
	fragment_colour = ( texel + texture2D( tex0, out_tex ) ) * out_colour;
}
