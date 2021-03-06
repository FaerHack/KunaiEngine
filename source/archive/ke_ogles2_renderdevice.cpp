//
//  ke_ogles2_renderdevice.cpp
//  rigidbodies
//
//  Created by Brandon Fleming on 10/15/14.
//  Copyright (c) 2014 Shogun3D. All rights reserved.
//

#include "ke_ogles2_renderdevice.h"
#include "ke_debug.h"


/*
 * Globals
 */

/* OpenGL primitive types */
uint32_t primitive_types[] =
{
    GL_POINTS,
    GL_LINES,
    GL_LINE_STRIP,
    GL_LINE_LOOP,
    GL_TRIANGLES,
    GL_TRIANGLE_STRIP
};

/* OpenGL data types */
uint32_t data_types[] =
{
    GL_BYTE,
    GL_UNSIGNED_BYTE,
    GL_SHORT,
    GL_UNSIGNED_SHORT,
    GL_INT,
    GL_UNSIGNED_INT,
    GL_FLOAT,
    0, //GL_DOUBLE
};

/* OpenGL buffer usage types */
uint32_t buffer_usage_types[] =
{
    GL_STATIC_DRAW,
    0, //GL_STATIC_READ,
    0, //GL_STATIC_COPY,
    GL_DYNAMIC_DRAW,
    0, //GL_DYNAMIC_READ,
    0, //GL_DYNAMIC_COPY,
    GL_STREAM_DRAW,
    0, //GL_STREAM_READ,
    0, //GL_STREAM_COPY,
};

/* OpenGL depth/alpha test functions */
uint32_t test_funcs[] =
{
    GL_NEVER,
    GL_LESS,
    GL_EQUAL,
    GL_LEQUAL,
    GL_GREATER,
    GL_NOTEQUAL,
    GL_GEQUAL,
    GL_ALWAYS
};

/* OpenGL texture targets */
uint32_t texture_targets[] =
{
    0, //GL_TEXTURE_1D,
    0, //GL_TEXTURE_1D_ARRAY,
    GL_TEXTURE_2D,
    0, //GL_TEXTURE_2D_ARRAY,
    0, //GL_TEXTURE_3D,
    0, //GL_TEXTURE_RECTANGLE,
};

/* OpenGL polygon modes */
uint32_t polygon_modes[] =
{
    GL_FRONT,
    GL_BACK,
    GL_FRONT_AND_BACK
};

/* OpenGL fill modes */
uint32_t fill_modes[] =
{
    0, //GL_POINT,
    0, //GL_LINE,
    0, //GL_FILL,
};

/* OpenGL texture formats */
uint32_t texture_formats[] =
{
    GL_RGBA,
    GL_BGRA
};

/* OpenGL cull modes */
uint32_t cull_modes[] =
{
    GL_NONE,
    GL_CW,
    GL_CCW
};

/* OpenGL blend modes */
uint32_t blend_modes[] =
{
    GL_ZERO,
    GL_ONE,
    GL_SRC_COLOR,
    GL_ONE_MINUS_SRC_COLOR,
    GL_DST_COLOR,
    GL_ONE_MINUS_DST_COLOR,
    GL_SRC_ALPHA,
    GL_ONE_MINUS_SRC_ALPHA,
    GL_DST_ALPHA,
    GL_ONE_MINUS_DST_ALPHA,
    GL_CONSTANT_COLOR,
    GL_ONE_MINUS_CONSTANT_COLOR,
    GL_CONSTANT_ALPHA,
    GL_ONE_MINUS_CONSTANT_ALPHA,
    GL_SRC_ALPHA_SATURATE,
    0, //GL_SRC1_COLOR,
    0, //GL_ONE_MINUS_SRC1_COLOR,
    0, //GL_SRC1_ALPHA,
    0, //GL_ONE_MINUS_SRC1_ALPHA
};


/*
 * Name: ke_ogl_renderdevice::ke_ogl_renderdevice
 * Desc: Default constructor
 */
ke_ogles2_renderdevice_t::ke_ogles2_renderdevice_t()
{
    /* TODO: Disable by making private? */
    assert(No);
}


/*
 * Name: ke_ogl_renderdevice::
 * Desc:
 */

/*
 * Name: ke_ogl_renderdevice::ke_ogl_renderdevice
 * Desc: Appropriate constructor used for initialization of OpenGL via SDL.
 */
ke_ogles2_renderdevice_t::ke_ogles2_renderdevice_t( ke_renderdevice_desc_t* renderdevice_desc )
{
    /* Until we are finished initializing, mark this flag as false */
    initialized = false;
    
    /* Sanity checks */
    if( !renderdevice_desc )
        return;
    
    /* Save a copy of the render device description */
    device_desc = new ke_renderdevice_desc_t;
    memmove( device_desc, renderdevice_desc, sizeof( ke_renderdevice_desc_t ) );
    
    /* Verify device type */
    if( device_desc->device_type != KE_RENDERDEVICE_OGLES2 )
        return;
    
    /* Initialize SDL video */
    if( SDL_InitSubSystem( SDL_INIT_VIDEO ) != 0 )
        return;
    
    SDL_LogSetAllPriority( SDL_LOG_PRIORITY_WARN );
    
    /* Setup OpenGL properties */
    SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, Yes );
    SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, device_desc->depth_bpp );
    SDL_GL_SetAttribute( SDL_GL_STENCIL_SIZE, device_desc->stencil_bpp );
    
    /* Set the appropriate OpenGL version and profile */
    major_version = 2;
    minor_version = 0;
    SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, major_version );
    SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, minor_version );
    SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES );
    
    /* TEST */
    int landscape = 1;
    int modes = SDL_GetNumDisplayModes(0);
    int sx = 0, sy = 0;
    for (int i = 0; i < modes; i++)
    {
        SDL_DisplayMode mode;
        SDL_GetDisplayMode(0, i, &mode);
        if (landscape ? mode.w > sx : mode.h > sy)
        {
            sx = mode.w;
            sy = mode.h;
        }
    }
    
    /* Initialize the SDL window */
    window = SDL_CreateWindow( "Kunai Engine 0.1a",  0, 0,
                              device_desc->width, device_desc->height, SDL_WINDOW_OPENGL | SDL_WINDOW_BORDERLESS );
    if( !window )
        return;
    
    /* Create our OpenGL context. */
    context = SDL_GL_CreateContext( window );
    
    /* Verify that we have a valid context */
    if( !context )
        DISPDBG( KE_ERROR, "Error creating OpenGL context!" );

    /* Set default OpenGL render states */
    glClearDepthf( 1.0f );
    glEnable( GL_DEPTH_TEST );
    glDepthFunc( GL_LEQUAL );
    
    glDisable( GL_BLEND );
    glDisable( GL_CULL_FACE );
    glDisable( GL_TEXTURE_2D );
    
    /* Set vertex attributes to their defaults */
    current_vertexattribute[0].index = 0;
    current_vertexattribute[0].size = 3;
    current_vertexattribute[0].type = KE_FLOAT;
    current_vertexattribute[0].normalize = No;
    current_vertexattribute[0].stride = 0;
    current_vertexattribute[0].offset = 0;
    current_vertexattribute[1].index = -1;
    
    /* Nullify current geometry buffer */
    current_geometrybuffer = NULL;
    
    /* Mark as initialized */
    initialized = Yes;
    
    /* Print OpenGL driver/implementation details */
    DISPDBG( 1, "\n\tOpenGL Vendor: " << glGetString( GL_VENDOR ) <<
            "\n\tOpenGL Version: " << glGetString( GL_VERSION ) <<
            "\n\tOpenGL Renderer: " << glGetString( GL_RENDERER ) << "\n" );
}


/*
 * Name: ke_ogl_renderdevice::~ke_ogl_renderdevice
 * Desc: Default deconstructor
 */
ke_ogles2_renderdevice_t::~ke_ogles2_renderdevice_t()
{
    delete device_desc;
    
    /* Kill the default vertex and fragment program */
    ke_uninitialize_default_shaders();
    
    /* Uninitialize and close OpenGL and SDL */
    SDL_GL_DeleteContext( context );
    SDL_DestroyWindow( window );
    SDL_QuitSubSystem( SDL_INIT_VIDEO );
}


/*
 * Name: ke_ogl_renderdevice::confirm_device
 * Desc: Gives confirmation that this device was successfully initialized.
 */
bool ke_ogles2_renderdevice_t::confirm_device()
{
    return initialized;
}

/*
 * Name: ke_ogles2_renderdevice_t::get_device_desc
 * Desc: Returns a copy of the device description structure
 */
void ke_ogles2_renderdevice_t::get_device_desc( ke_renderdevice_desc_t* device_desc )
{
    memmove( device_desc, this->device_desc, sizeof( ke_renderdevice_desc_t ) );
}

/*
 * Name: ke_ogl_renderdevice::set_clear_colour_fv
 * Desc: Sets the clear colour
 */
void ke_ogles2_renderdevice_t::set_clear_colour_fv( float* colour )
{
    memcpy( this->clear_colour, clear_colour, sizeof(float)*4 );
    
    glClearColor( colour[0], colour[1], colour[2], colour[3] );
}


/*
 * Name: ke_ogl_renderdevice::set_clear_colour_ubv
 * Desc: Same as above.
 */
void ke_ogles2_renderdevice_t::set_clear_colour_ubv( uint8_t* colour )
{
    this->clear_colour[0] = float(colour[0]/255);
    this->clear_colour[1] = float(colour[1]/255);
    this->clear_colour[2] = float(colour[2]/255);
    this->clear_colour[3] = float(colour[3]/255);
    
    glClearColor( this->clear_colour[0], this->clear_colour[1], this->clear_colour[2], this->clear_colour[3] );
}


/*
 * Name: ke_ogl_renderdevice::set_clear_depth
 * Desc:
 */
void ke_ogles2_renderdevice_t::set_clear_depth( float depth )
{
    glClearDepthf( depth );
}


/*
 * Name: ke_ogl_renderdevice::clear_render_buffer
 * Desc: Clears only the current render buffer
 */
void ke_ogles2_renderdevice_t::clear_colour_buffer()
{
    glClear( GL_COLOR_BUFFER_BIT );
}


/*
 * Name: ke_ogl_renderdevice::clear_depth_buffer
 * Desc: Clears only the current depth buffer
 */
void ke_ogles2_renderdevice_t::clear_depth_buffer()
{
    glClear( GL_DEPTH_BUFFER_BIT );
}


/*
 * Name: ke_ogl_renderdevice::clear_stencil_buffer
 * Desc: Clears only the current stencil buffer
 */
void ke_ogles2_renderdevice_t::clear_stencil_buffer()
{
    glClear( GL_STENCIL_BUFFER_BIT );
}


/*
 * Name: ke_ogl_renderdevice::swap
 * Desc: Swaps the double buffer.
 */
void ke_ogles2_renderdevice_t::swap()
{
    SDL_GL_SwapWindow( window );
}


/*
 * Name: ke_ogles2_renderdevice_t::create_geometry_buffer
 * Desc: Creates a geometry buffer based on the vertex and index data given.  Vertex and index
 *       buffers are encapsulated into one interface for easy management, however, index data
 *       input is completely optional.  Interleaved vertex data is also supported.
 */
bool ke_ogles2_renderdevice_t::create_geometry_buffer( void* vertex_data, uint32_t vertex_data_size, void* index_data, uint32_t index_data_size, uint32_t index_data_type, uint32_t flags, ke_vertexattribute_t* va, ke_geometrybuffer_t** geometry_buffer )
{
    GLenum error = glGetError();
    
    /* Sanity check(s) */
    if( !geometry_buffer )
        return false;
    //if( !vertex_attributes )
    //  return false;
    if( !vertex_data_size )
        return false;   /* Temporary? */
    
    *geometry_buffer = new ke_ogl_geometrybuffer_t;
    ke_ogl_geometrybuffer_t* gb = static_cast<ke_ogl_geometrybuffer_t*>( *geometry_buffer );
    
    /* Enumerate buffer usage flags */
    
    /* Create a vertex array object */
    glGenVertexArraysOES( 1, &gb->vao );
    error = glGetError();
    
    /* Bind this vertex array object */
    glBindVertexArrayOES( gb->vao );
    
    /* Create the vertex buffer object */
    glGenBuffers( 1, &gb->vbo[0] );
    error = glGetError();
    
    /* Set the vertex buffer data */
    glBindBuffer( GL_ARRAY_BUFFER, gb->vbo[0] );
    glBufferData( GL_ARRAY_BUFFER, vertex_data_size, vertex_data, buffer_usage_types[flags] );
    error = glGetError();
    
    /* Set the vertex attributes for this geometry buffer */
    for( int i = 0; va[i].index != -1; i++ )
    {
        glVertexAttribPointer( va[i].index,
                              va[i].size,
                              data_types[va[i].type],
                              va[i].normalize,
                              va[i].stride,
                              BUFFER_OFFSET(va[i].offset) );
        glEnableVertexAttribArray(va[i].index);
    }
    error = glGetError();
    
    /* Create an index buffer if desired */
    if( index_data_size )
    {
        glGenBuffers( 1, &gb->vbo[1] );
        glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, gb->vbo[1] );
        error = glGetError();
        
        /* Set the index buffer data */
        glBufferData( GL_ELEMENT_ARRAY_BUFFER, index_data_size, index_data, buffer_usage_types[flags] );
        gb->index_type = index_data_type;
    }
    
    /* Unbind this vertex array object */
    glBindVertexArrayOES(0);
    
    return true;
}


/*
 * Name: ke_ogles2_renderdevice_t::delete_geometry_buffer
 * Desc:
 */
void ke_ogles2_renderdevice_t::delete_geometry_buffer( ke_geometrybuffer_t* geometry_buffer )
{
    ke_ogl_geometrybuffer_t* gb = static_cast<ke_ogl_geometrybuffer_t*>( geometry_buffer );
    
    /* Delete the VBO and VAO */
    glDeleteBuffers( 2, gb->vbo );
    glDeleteVertexArraysOES( 1, &gb->vao );
    
    delete geometry_buffer;
}

/*
 * Name: ke_ogl_renderdevice::set_vertex_buffer
 * Desc: Sets the current geometry buffer to be used when rendering. Internally, binds the
 *       vertex array object. If NULL, then sets the current vertex array object to 0.
 */
void ke_ogles2_renderdevice_t::set_geometry_buffer( ke_geometrybuffer_t* geometry_buffer )
{
    current_geometrybuffer = geometry_buffer;
    
    if( geometry_buffer )
        glBindVertexArrayOES( static_cast<ke_ogl_geometrybuffer_t*>( geometry_buffer )->vao );
    else
        glBindVertexArrayOES(0);
}

/*
 * Name: ke_ogles2_renderdevice_t::create_program
 * Desc: Creates a complete OpenGL program out of shaders in text form. The minimum requirements
 *       are one valid vertex and fragment shader, while geometry and tesselation shaders are
 *       optional.  Obviously, tesselation shaders require OpenGL 4.1+, and cannot be used with
 *       OpenGL 3.2.  This function will automatically search for specific attribute locations
 *       before linking it and search for pre-determined uniform names for textures and matrices
 *       (see code below).
 *       TODO: Allow user defined constants.
 */
bool ke_ogles2_renderdevice_t::create_program( const char* vertex_shader, const char* fragment_shader, const char* geometry_shader, const char* tesselation_shader, ke_vertexattribute_t* vertex_attributes, ke_gpu_program_t** gpu_program )
{
    GLuint p, f, v, t = 0, g;
    *gpu_program = new ke_ogl_gpu_program_t;
    ke_ogl_gpu_program_t* gp = static_cast<ke_ogl_gpu_program_t*>( *gpu_program );
    GLenum error = glGetError();
    
    v = glCreateShader( GL_VERTEX_SHADER );
    f = glCreateShader( GL_FRAGMENT_SHADER );
    
    const char* vv = vertex_shader;
    const char* ff = fragment_shader;
    const char* gg = geometry_shader;
    const char* tt = tesselation_shader;
    
    glShaderSource( v, 1, &vv, NULL );
    glShaderSource( f, 1, &ff, NULL );
    
    GLint compiled;
    
    glCompileShader(v);
    glGetShaderiv( v, GL_COMPILE_STATUS, &compiled );
    if( !compiled )
    {
        char str[2048];
        int len = 0;
        
        glGetShaderInfoLog( v, 2048, &len, str );
        printf("Vertex shader not compiled.\n%s\n", str);
    }
    
    glCompileShader(f);
    glGetShaderiv( f, GL_COMPILE_STATUS, &compiled );
    if( !compiled )
    {
        char str[2048];
        int len = 0;
        
        glGetShaderInfoLog( f, 2048, &len, str );
        printf("Fragment shader not compiled.\n%s\n", str);
    }
    
    p = glCreateProgram();
    
    glBindAttribLocation( p, 0, "in_pos" );
    glBindAttribLocation( p, 1, "in_normal" );
    glBindAttribLocation( p, 2, "in_tangent" );
    glBindAttribLocation( p, 3, "in_bitangent" );
    glBindAttribLocation( p, 4, "in_colour" );
    glBindAttribLocation( p, 5, "in_tex0" );
    glBindAttribLocation( p, 6, "in_tex1" );
    glBindAttribLocation( p, 7, "in_tex2" );
    glBindAttribLocation( p, 8, "in_tex3" );
    glBindAttribLocation( p, 9, "in_tex4" );
    glBindAttribLocation( p, 10, "in_tex5" );
    glBindAttribLocation( p, 11, "in_tex6" );
    glBindAttribLocation( p, 12, "in_tex7" );
    
    glAttachShader( p, v );
    glAttachShader( p, f );
    
    glLinkProgram(p);
    glUseProgram(p);
    
    glDeleteShader(v);
    glDeleteShader(f);
    glDeleteShader(g);
    
    GLuint uniform_tex0 = glGetUniformLocation( p, "tex0" );
    GLuint uniform_tex1 = glGetUniformLocation( p, "tex1" );
    GLuint uniform_tex2 = glGetUniformLocation( p, "tex2" );
    GLuint uniform_tex3 = glGetUniformLocation( p, "tex3" );
    GLuint uniform_tex4 = glGetUniformLocation( p, "tex4" );
    GLuint uniform_tex5 = glGetUniformLocation( p, "tex5" );
    GLuint uniform_tex6 = glGetUniformLocation( p, "tex6" );
    GLuint uniform_tex7 = glGetUniformLocation( p, "tex7" );
    
    gp->matrices[0] = glGetUniformLocation( p, "world" );
    error = glGetError();
    gp->matrices[1] = glGetUniformLocation( p, "view" );
    error = glGetError();
    gp->matrices[2] = glGetUniformLocation( p, "proj" );
    error = glGetError();
    
    glUniform1i( uniform_tex0, 0 );
    glUniform1i( uniform_tex1, 1 );
    glUniform1i( uniform_tex2, 2 );
    glUniform1i( uniform_tex3, 3 );
    glUniform1i( uniform_tex4, 4 );
    glUniform1i( uniform_tex5, 5 );
    glUniform1i( uniform_tex6, 6 );
    glUniform1i( uniform_tex7, 7 );
    
    glUseProgram(0);
    
    /* Save the handle to this newly created program */
    gp->program = p;
    
#if 0
    /* Copy vertex attributes */
    int va_size = 0;
    while( vertex_attributes[va_size].index != -1 )
        va_size++;
    
    gp->va = new ke_vertexattribute_t[va_size+1];
    memmove( gp->va, vertex_attributes, sizeof( ke_vertexattribute_t ) * (va_size+1) );
#endif
    
    return true;
}

/*
 * Name: ke_ogles2_renderdevice_t::delete_program
 * Desc: Deletes the GPU program.
 */
void ke_ogles2_renderdevice_t::delete_program( ke_gpu_program_t* gpu_program )
{
    /* Deletes the GPU program */
    if( gpu_program )
    {
        glDeleteProgram( static_cast<ke_ogl_gpu_program_t*>(gpu_program)->program );
        //delete[] static_cast<ke_ogl_gpu_program_t*>(gpu_program)->va;
        delete gpu_program;
    }
}

/*
 * Name: ke_ogles2_renderdevice_t::set_program
 * Desc: Sets the GPU program.  If NULL, the GPU program is set to 0.
 */
void ke_ogles2_renderdevice_t::set_program( ke_gpu_program_t* gpu_program )
{
    GLenum error = glGetError();
    
    /* Check for a valid pointer. If NULL, then we set the current program to 0. */
    if( gpu_program )
    {
        ke_ogl_gpu_program_t* gp = static_cast<ke_ogl_gpu_program_t*>(gpu_program);
        
        /* Save a copy of this program */
        current_gpu_program = gpu_program;
        
        glUseProgram( gp->program );
    }
    else
        glUseProgram(0);
}

/*
 * Name: ke_ogles2_renderdevice_t::set_program_constant_1fv
 * Desc: Sets program constants (do your research on GLSL uniforms)
 */
void ke_ogles2_renderdevice_t::set_program_constant_1fv( const char* location, int count, float* value )
{
    ke_ogl_gpu_program_t* p = static_cast<ke_ogl_gpu_program_t*>( current_gpu_program );
    
    int loc = glGetUniformLocation( p->program, location );
    glUniform1fv( loc, count, value );
}

/*
 * Name: ke_ogles2_renderdevice_t::set_program_constant_2fv
 * Desc: Sets program constants (do your research on GLSL uniforms)
 */
void ke_ogles2_renderdevice_t::set_program_constant_2fv( const char* location, int count, float* value )
{
    ke_ogl_gpu_program_t* p = static_cast<ke_ogl_gpu_program_t*>( current_gpu_program );
    
    int loc = glGetUniformLocation( p->program, location );
    glUniform2fv( loc, count, value );
}

/*
 * Name: ke_ogles2_renderdevice_t::set_program_constant_3fv
 * Desc: Sets program constants (do your research on GLSL uniforms)
 */
void ke_ogles2_renderdevice_t::set_program_constant_3fv( const char* location, int count, float* value )
{
    ke_ogl_gpu_program_t* p = static_cast<ke_ogl_gpu_program_t*>( current_gpu_program );
    
    int loc = glGetUniformLocation( p->program, location );
    glUniform3fv( loc, count, value );
}

/*
 * Name: ke_ogles2_renderdevice_t::set_program_constant_4fv
 * Desc: Sets program constants (do your research on GLSL uniforms)
 */
void ke_ogles2_renderdevice_t::set_program_constant_4fv( const char* location, int count, float* value )
{
    ke_ogl_gpu_program_t* p = static_cast<ke_ogl_gpu_program_t*>( current_gpu_program );
    
    int loc = glGetUniformLocation( p->program, location );
    glUniform4fv( loc, count, value );
}

/*
 * Name: ke_ogles2_renderdevice_t::set_program_constant
 * Desc: Sets program constants (do your research on GLSL uniforms)
 */
void ke_ogles2_renderdevice_t::set_program_constant_1iv( const char* location, int count, int* value )
{
    ke_ogl_gpu_program_t* p = static_cast<ke_ogl_gpu_program_t*>( current_gpu_program );
    
    int loc = glGetUniformLocation( p->program, location );
    glUniform1iv( loc, count, value );
}

/*
 * Name: ke_ogles2_renderdevice_t::set_program_constant
 * Desc: Sets program constants (do your research on GLSL uniforms)
 */
void ke_ogles2_renderdevice_t::set_program_constant_2iv( const char* location, int count, int* value )
{
    ke_ogl_gpu_program_t* p = static_cast<ke_ogl_gpu_program_t*>( current_gpu_program );
    
    int loc = glGetUniformLocation( p->program, location );
    glUniform2iv( loc, count, value );
}

/*
 * Name: ke_ogles2_renderdevice_t::set_program_constant
 * Desc: Sets program constants (do your research on GLSL uniforms)
 */
void ke_ogles2_renderdevice_t::set_program_constant_3iv( const char* location, int count, int* value )
{
    ke_ogl_gpu_program_t* p = static_cast<ke_ogl_gpu_program_t*>( current_gpu_program );
    
    int loc = glGetUniformLocation( p->program, location );
    glUniform3iv( loc, count, value );
}

/*
 * Name: ke_ogles2_renderdevice_t::set_program_constant
 * Desc: Sets program constants (do your research on GLSL uniforms)
 */
void ke_ogles2_renderdevice_t::set_program_constant_4iv( const char* location, int count, int* value )
{
    ke_ogl_gpu_program_t* p = static_cast<ke_ogl_gpu_program_t*>( current_gpu_program );
    
    int loc = glGetUniformLocation( p->program, location );
    glUniform4iv( loc, count, value );
}

/*
 * Name: ke_ogles2_renderdevice_t::get_program_constant_fv
 * Desc: Gets program constants (do your research on GLSL uniforms)
 */
void ke_ogles2_renderdevice_t::get_program_constant_fv( const char* location, float* value )
{
    ke_ogl_gpu_program_t* p = static_cast<ke_ogl_gpu_program_t*>( current_gpu_program );
    
    int loc = glGetUniformLocation( p->program, location );
    glGetUniformfv( p->program, loc, value );
}

/*
 * Name: ke_ogles2_renderdevice_t::get_program_constant_iv
 * Desc: Gets program constants (do your research on GLSL uniforms)
 */
void ke_ogles2_renderdevice_t::get_program_constant_iv( const char* location, int* value )
{
    ke_ogl_gpu_program_t* p = static_cast<ke_ogl_gpu_program_t*>( current_gpu_program );
    
    int loc = glGetUniformLocation( p->program, location );
    glGetUniformiv( p->program, loc, value );
}

/*
 * Name: ke_ogles2_renderdevice_t::create_constant_buffer
 * Desc: Creates a constant buffer.
 * TODO: Support for OpenGL.
 */
bool ke_ogles2_renderdevice_t::create_constant_buffer( uint32_t buffer_size, ke_constantbuffer_t** constant_buffer )
{
    return false;
}

/*
 * Name: ke_ogles2_renderdevice_t::delete_constant_buffer
 * Desc:
 */
void ke_ogles2_renderdevice_t::delete_constant_buffer( ke_constantbuffer_t* constant_buffer )
{
    
}

/*
 * Name: ke_ogles2_renderdevice_t::set_constant_buffer_data
 * Desc:
 */
bool ke_ogles2_renderdevice_t::set_constant_buffer_data( void* data, ke_constantbuffer_t* constant_buffer )
{
    return true;
}

/*
 * Name: ke_ogles2_renderdevice_t::set_vertex_shader_constant_buffer
 * Desc:
 */
void ke_ogles2_renderdevice_t::set_vertex_shader_constant_buffer( int slot, ke_constantbuffer_t* constant_buffer )
{
    
}

/*
 * Name: ke_ogles2_renderdevice_t::set_pixel_shader_constant_buffer
 * Desc:
 */
void ke_ogles2_renderdevice_t::set_pixel_shader_constant_buffer( int slot, ke_constantbuffer_t* constant_buffer )
{
    
}

/*
 * Name: ke_ogles2_renderdevice_t::set_geometry_shader_constant_buffer
 * Desc:
 */
void ke_ogles2_renderdevice_t::set_geometry_shader_constant_buffer( int slot, ke_constantbuffer_t* constant_buffer )
{
    
}

/*
 * Name: ke_ogles2_renderdevice_t::set_tesselation_shader_constant_buffer
 * Desc:
 */
void ke_ogles2_renderdevice_t::set_tesselation_shader_constant_buffer( int slot, ke_constantbuffer_t* constant_buffer )
{
    
}

/*
 * Name: ke_ogl_renderdevice::create_texture_1d
 * Desc: Creates a 1D texture.
 */
bool ke_ogles2_renderdevice_t::create_texture_1d( uint32_t target, int width, int mipmaps, uint32_t format, uint32_t data_type, ke_texture_t** texture )
{
#if 0
    GLenum error = glGetError();
    
    /* Allocate a new texture */
    *texture = new ke_ogl_texture_t;
    ke_ogl_texture_t* t = static_cast<ke_ogl_texture_t*>( *texture );
    
    /* Set texture attributes */
    t->width = width;
    t->target = target;
    t->data_type = data_types[data_type];
    t->depth_format = texture_formats[format];
    t->internal_format = texture_formats[format];
    t->target = target;
    
    /* Use OpenGL to create a new 1D texture */
    glGenTextures( 1, &t->handle );
    glBindTexture( t->target, t->handle );
    error = glGetError();
    
    /* Set the initial texture attributes */
    glTexImage1D( t->target, 0, texture_formats[format], width, 0, texture_formats[format], data_types[data_type], NULL );
    error = glGetError();
    
    /* Set texture parameters */
    glTexParameteri( target, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    error = glGetError();
    glTexParameteri( target, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    error = glGetError();
    
    return true;
#endif
    
    return false;
}

/*
 * Name: ke_ogl_renderdevice::create_texture_2d
 * Desc: Creates a blank 2D texture.
 */
bool ke_ogles2_renderdevice_t::create_texture_2d( uint32_t target, int width, int height, int mipmaps, uint32_t format, uint32_t data_type, ke_texture_t** texture )
{
    GLenum error = glGetError();
    
    /* Allocate a new texture */
    (*texture) = new ke_ogl_texture_t;
    ke_ogl_texture_t* t = static_cast<ke_ogl_texture_t*>( *texture );
    
    /* Set texture attributes */
    t->width = width;
    t->height = height;
    t->target = target;
    t->data_type = data_types[data_type];
    t->depth_format = texture_formats[format];
    t->internal_format = texture_formats[format];
    t->target = target;
    
    /* Use OpenGL to create a new 2D texture */
    glGenTextures( 1, &t->handle );
    glBindTexture( t->target, t->handle );
    error = glGetError();
    
    /* Set the initial texture attributes */
    glTexImage2D( t->target, 0, texture_formats[format], width, height, 0, texture_formats[format], data_types[data_type], NULL );
    error = glGetError();
    
    /* Set texture parameters */
    glTexParameteri( target, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    error = glGetError();
    glTexParameteri( target, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    error = glGetError();
    
    return true;
}

/*
 * Name: ke_ogl_renderdevice::create_texture_3d
 * Desc: Creates a blank 3D texture.
 */
bool ke_ogles2_renderdevice_t::create_texture_3d( uint32_t target, int width, int height, int depth, int mipmaps, uint32_t format, uint32_t data_type, ke_texture_t** texture )
{
#if 0 /* TODO */
    GLenum error = glGetError();
    
    /* Allocate a new texture */
    (*texture) = new ke_ogl_texture_t;
    ke_ogl_texture_t* t = static_cast<ke_ogl_texture_t*>( *texture );
    
    /* Set texture attributes */
    t->width = width;
    t->height = height;
    t->depth = depth;
    t->target = target;
    t->data_type = data_types[data_type];
    t->depth_format = texture_formats[format];
    t->internal_format = texture_formats[format];
    t->target = target;
    
    /* Use OpenGL to create a new 3D texture */
    glGenTextures( 1, &t->handle );
    glBindTexture( t->target, t->handle );
    error = glGetError();
    
    /* Set the initial texture attributes */
    glTexImage3D( t->target, 0, texture_formats[format], width, height, depth, 0, texture_formats[format], data_types[data_type], NULL );
    error = glGetError();
    
    /* Set texture parameters */
    glTexParameteri( target, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    error = glGetError();
    glTexParameteri( target, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    error = glGetError();
    
    return true;
#endif
    
    return false;
}

/*
 * Name: ke_ogles2_renderdevice_t::delete_texture
 * Desc: Deletes a texture from memory.
 */
void ke_ogles2_renderdevice_t::delete_texture( ke_texture_t* texture )
{
    ke_ogl_texture_t* t = static_cast<ke_ogl_texture_t*>( texture );
    
    if( texture )
    {
        glDeleteTextures( 1, &t->handle );
        delete texture;
    }
}


/*
 * Name: ke_ogles2_renderdevice_t::set_texture_data_1d
 * Desc: Sets pixel data for a 1D texture.
 */
void ke_ogles2_renderdevice_t::set_texture_data_1d( int offsetx, int width, int miplevel, void* pixels, ke_texture_t* texture )
{
#if 0
    ke_ogl_texture_t* t = static_cast<ke_ogl_texture_t*>( texture );
    
    glTexSubImage1D( t->target, miplevel, offsetx, width, t->internal_format, t->data_type, pixels );
#endif
}

/*
 * Name: ke_ogles2_renderdevice_t::set_texture_data_2d
 * Desc: Sets pixel data for a 1D texture.
 */
void ke_ogles2_renderdevice_t::set_texture_data_2d( int offsetx, int offsety, int width, int height, int miplevel, void* pixels, ke_texture_t* texture )
{
    ke_ogl_texture_t* t = static_cast<ke_ogl_texture_t*>( texture );
    
    glTexSubImage2D( t->target, miplevel, offsetx, offsety, width, height, t->internal_format, t->data_type, pixels );
    GLenum error = glGetError();
}

/*
 * Name: ke_ogles2_renderdevice_t::set_texture_data_3d
 * Desc: Sets pixel data for a 1D texture.
 */
void ke_ogles2_renderdevice_t::set_texture_data_3d( int offsetx, int offsety, int offsetz, int width, int height, int depth, int miplevel, void* pixels, ke_texture_t* texture )
{
#if 0 /* TODO */
    ke_ogl_texture_t* t = static_cast<ke_ogl_texture_t*>( texture );
    
    glTexSubImage3D( t->target, miplevel, offsetx, offsety, offsetz, width, height, depth, t->internal_format, t->data_type, pixels );
#endif
}

/*
 * Name: ke_ogles2_renderdevice_t::create_render_target
 * Desc: Creates a seperate render target (FBO), typically used for rendering to a texture.
 *       Creates a colour, depth and stencil buffer (if desired) and can be set as a texture.
 */
bool ke_ogles2_renderdevice_t::create_render_target( int width, int height, int depth, uint32_t flags, ke_rendertarget_t** rendertarget )
{
#if 0 /* TODO */
    GLenum error = glGetError();
    ke_ogl_rendertarget_t* rt = static_cast<ke_ogl_rendertarget_t*>( *rendertarget );
    
    /* Generate frame buffer object */
    glGenFramebuffers( 1, &rt->frame_buffer_object );
    error = glGetError();
    if( error != GL_NO_ERROR )
        DISPDBG( 1, "ke_ogl_renderdevice::create_render_target(): Error creating FBO!\n" );
    
    /* Bind the FBO */
    glBindFramebuffer( GL_FRAMEBUFFER, rt->frame_buffer_object );
    
    /* Create a texture to render this FBO to */
    this->create_texture_2d( KE_TEXTURE_2D, width, height, 0, KE_TEXTUREFORMAT_RGBA, KE_UNSIGNED_BYTE, &rt->texture );
    
    /* Use nearest point filtering */
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    
    /* Create the depth buffer */
    glGenRenderbuffers( 1, &rt->depth_render_buffer );
    error = glGetError();
    if( error != GL_NO_ERROR )
        DISPDBG( 1, "ke_ogl_renderdevice::create_render_target(): Error creating depth buffer!\n" );
    
    /* Set the depth buffer attributes */
    ke_ogl_texture_t* tex = static_cast<ke_ogl_texture_t*>( rt->texture );
    
    glBindRenderbuffer( GL_RENDERBUFFER, rt->depth_render_buffer );
    glRenderbufferStorage( GL_RENDERBUFFER, GL_DEPTH_COMPONENT, tex->width, tex->height );
    glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rt->depth_render_buffer );
    
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, tex->height, 0 );
    
    GLenum draw_buffers[] = { GL_COLOR_ATTACHMENT0 };
    glDrawBuffers( 1, draw_buffers );
    
    /* Check the framebuffer status */
    if( glCheckFramebufferStatus( GL_FRAMEBUFFER ) != GL_FRAMEBUFFER_COMPLETE )
    {
        error = glGetError();
        DISPDBG( 1, "ke_ogles2_renderdevice_t::create_render_target(): Error during rendertarget creation! (error=0x" << error << ")\n" );
    }
    
    return true;
#endif
    
    return false;
}

/*
 * Name: ke_ogles2_renderdevice_t::delete_render_target
 * Desc: Deletes the render target resources used.
 */
void ke_ogles2_renderdevice_t::delete_render_target( ke_rendertarget_t* rendertarget )
{
    ke_ogl_rendertarget_t* rt = static_cast<ke_ogl_rendertarget_t*>( rendertarget );
    
    /* Delete the texture */
    this->delete_texture( static_cast<ke_ogl_texture_t*>( rt->texture ) );
    
    /* Delete the render target */
    glDeleteRenderbuffers( 1, &rt->depth_render_buffer );
    glDeleteFramebuffers( 1, &rt->frame_buffer_object );
    
    delete rendertarget;
}

/*
 * Name: ke_ogles2_renderdevice_t::bind_render_target
 * Desc: Binds the render target to OpenGL.  You set the texture to the appropriate  texture
 *       stage yourself using ::set_texture().
 */
void ke_ogles2_renderdevice_t::bind_render_target( ke_rendertarget_t* rendertarget )
{
    GLenum error = glGetError();
    ke_ogl_rendertarget_t* rt = static_cast<ke_ogl_rendertarget_t*>( rendertarget );
    
    /* Bind the FBO */
    glBindFramebuffer( GL_FRAMEBUFFER, rt->frame_buffer_object );
    error = glGetError();
    if( error != GL_NO_ERROR )
        DISPDBG( 1, "ke_ogles2_renderdevice_t::bind_render_target(): Error binding rendertarget! (error=0x" << error << ")\n" );
}

/*
 * Name: ke_ogles2_renderdevice_t::set_texture
 * Desc: Sets a texture to the desired texture stage.  If NULL, then texturing is disabled on
 *       the selected texture stage.
 */
void ke_ogles2_renderdevice_t::set_texture( int stage, ke_texture_t* texture )
{
    ke_ogl_texture_t* tex = static_cast<ke_ogl_texture_t*>(texture);
    
    /* Select the currently active texture stage */
    glActiveTexture( GL_TEXTURE0 + stage );
    
    /* If this is a valid texture, set it. */
    if( texture )
    {
        glEnable( tex->target );
        glBindTexture( tex->target, tex->target );
    }
    else
    {
//        glDisable( GL_TEXTURE_1D );
        glDisable( GL_TEXTURE_2D );
//        glDisable( GL_TEXTURE_3D );
//        glDisable( GL_TEXTURE_1D_ARRAY );
//        glDisable( GL_TEXTURE_2D_ARRAY );
//        glDisable( GL_TEXTURE_RECTANGLE );
    }
}

/*
 * Name: ke_ogles2_renderdevice_t::set_render_states
 * Desc: Applies a list of user defined render states.
 * TODO: Allow explicit deferring of render states?
 */
void ke_ogles2_renderdevice_t::set_render_states( ke_state_t* states )
{
    int i = 0;
    
    /* Apply each render state in the list */
    while( states[i].state != -1 )
    {
        switch( states[i].state )
        {
            case KE_RS_DEPTHTEST:
                if( states[i].param1 )
                    glEnable( GL_DEPTH_TEST );
                else
                    glDisable( GL_DEPTH_TEST );
                break;
                
            case KE_RS_DEPTHFUNC:
                glDepthFunc( test_funcs[states[i].param1] );
                break;
                
            case KE_RS_DEPTHMASK:
                if( states[i].param1 )
                    glEnable( GL_DEPTH_WRITEMASK );
                else
                    glDisable( GL_DEPTH_WRITEMASK );
                break;
                
            case KE_RS_CLEARDEPTH:
                glClearDepthf( states[i].fparam );
                break;
                
            case KE_RS_ALPHABLEND:
                if( states[i].param1 )
                    glEnable( GL_BLEND );
                else
                    glDisable( GL_BLEND );
                break;
                
            case KE_RS_FRONTFACE:
                /* TODO */
                break;
                
            /*case KE_RS_POLYGONMODE:
                glPolygonMode( polygon_modes[states[i].param1], fill_modes[states[i].param2] );
                break;*/
                
            case KE_RS_BLENDFUNC:
                glBlendFunc( blend_modes[states[i].param1], blend_modes[states[i].param2] );
                break;
                
            case KE_RS_CULLMODE:
                if( states[i].param1 )
                    glEnable( GL_CULL_FACE );
                else
                    glDisable( GL_CULL_FACE );
                glCullFace( cull_modes[states[i].param2] );
                break;
                
            default:
                DISPDBG( KE_WARNING, "Bad render state!\nstate: " << states[i].state << "\n"
                        "param1: " << states[i].param1 << "\n"
                        "param2: " << states[i].param2 << "\n"
                        "param3: " << states[i].param3 << "\n"
                        "fparam: " << states[i].fparam << "\n"
                        "dparam: " << states[i].dparam << "\n" );
                break;
        }
        
        i++;
    }
}

/*
 * Name: ke_ogles2_renderdevice_t::set_sampler_states
 * Desc: Applies a list of user defined sampler states.
 * TODO: Allow explicit deferring of sampler states?
 */
void ke_ogles2_renderdevice_t::set_sampler_states( ke_state_t* states )
{
    
}

/*void ke_ogles2_renderdevice_t::draw_vertices_im()
 {
 
 }*/


/*
 * Name: ke_ogl_renderdevice::draw_vertices
 * Desc: Draws vertices from the current vertex buffer
 */
void ke_ogles2_renderdevice_t::draw_vertices( uint32_t primtype, uint32_t stride, int first, int count )
{
    ke_ogl_geometrybuffer_t* gb = static_cast<ke_ogl_geometrybuffer_t*>( current_geometrybuffer );
    ke_ogl_gpu_program_t* gp = static_cast<ke_ogl_gpu_program_t*>( current_gpu_program );
    GLenum error = glGetError();
    
    /* Assuming there is already a GPU program bound, attempt to set the current matrices */
    glUniformMatrix4fv( gp->matrices[0], 1, No, &world_matrix.col0.x );
    error = glGetError();
    glUniformMatrix4fv( gp->matrices[1], 1, No, &view_matrix.col0.x );
    error = glGetError();
    glUniformMatrix4fv( gp->matrices[2], 1, No, &projection_matrix.col0.x );
    error = glGetError();
    
    /* Bind the vertex buffer object, but not the index buffer object */
    glBindBuffer( GL_ARRAY_BUFFER, gb->vbo[0] );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
    error = glGetError();
    
    /* Draw the vertices */
    glDrawArrays( primitive_types[primtype], first, count );
    error = glGetError();
}

/*
 * Name: ke_ogl_renderdevice::draw_indexed_vertices
 * Desc: Draws vertices from the current vertex and index buffer.
 */
void ke_ogles2_renderdevice_t::draw_indexed_vertices( uint32_t primtype, uint32_t stride, int count )
{
    ke_ogl_geometrybuffer_t* gb = static_cast<ke_ogl_geometrybuffer_t*>( current_geometrybuffer );
    ke_ogl_gpu_program_t* gp = static_cast<ke_ogl_gpu_program_t*>( current_gpu_program );
    GLenum error = glGetError();
    
    /* Assuming there is already a GPU program bound, attempt to set the current matrices */
    glUniformMatrix4fv( gp->matrices[0], 1, No, &world_matrix.col0.x );
    glUniformMatrix4fv( gp->matrices[1], 1, No, &view_matrix.col0.x );
    glUniformMatrix4fv( gp->matrices[2], 1, No, &projection_matrix.col0.x );
    
    /* Bind the vertex and index buffer objects */
    glBindBuffer( GL_ARRAY_BUFFER, gb->vbo[0] );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, gb->vbo[1] );
    error = glGetError();
    if( error != GL_NO_ERROR )
        DISPDBG( 1, "draw_indexed_vertices(): error binding buffers\n" );
    
    /* Draw the vertices */
    glDrawElements( primitive_types[primtype], count, data_types[gb->index_type], NULL );
    error = glGetError();
    if( error != GL_NO_ERROR )
        DISPDBG( 1, "draw_indexed_vertices(): error drawing vertices\n" );
}

/*
 * Name: ke_ogles2_renderdevice_t::draw_indexed_vertices_range
 * Desc: Same as above, but allows the user to specify the start/end vertex.
 */
void ke_ogles2_renderdevice_t::draw_indexed_vertices_range( uint32_t primtype, uint32_t stride, int start, int end, int count )
{
#if 0
    ke_ogl_geometrybuffer_t* gb = static_cast<ke_ogl_geometrybuffer_t*>( current_geometrybuffer );
    ke_ogl_gpu_program_t* gp = static_cast<ke_ogl_gpu_program_t*>( current_gpu_program );
    GLenum error = glGetError();
    
    /* Assuming there is already a GPU program bound, attempt to set the current matrices */
    glUniformMatrix4fv( gp->matrices[0], 1, No, &world_matrix.col0.x );
    glUniformMatrix4fv( gp->matrices[1], 1, No, &view_matrix.col0.x );
    glUniformMatrix4fv( gp->matrices[2], 1, No, &projection_matrix.col0.x );
    
    /* Bind the vertex buffer object, but not the index buffer object */
    glBindBuffer( GL_ARRAY_BUFFER, gb->vbo[0] );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, gb->vbo[1] );
    error = glGetError();
    if( error != GL_NO_ERROR )
        DISPDBG( 1, "draw_indexed_vertices_range(): error binding buffers\n" );
    
    /* Draw the vertices */
    glDrawRangeElements( primitive_types[primtype], start, end, count, data_types[gb->index_type], NULL );
    error = glGetError();
    if( error != GL_NO_ERROR )
        DISPDBG( 1, "draw_indexed_vertices_range(): error drawing vertices\n" );
#endif
}

/*
 * Name: ke_ogles2_renderdevice_t::get_framebuffer_region
 * Desc: Returns a pointer filled with pixels of the given region of the current framebuffer.
 * TODO: Determine bit depth, allow reading from depth buffers, etc.
 */
bool ke_ogles2_renderdevice_t::get_framebuffer_region( int x, int y, int width, int height, uint32_t flags, int* bpp, void** pixels )
{
    int buffer_bpp = device_desc->colour_bpp;
    
    /* Return the bit depth of this framebuffer */
    *bpp = buffer_bpp;
    
    /* Allocate pointer to hold the pixel data */
    (*pixels) = new uint8_t[(width-x)*(height-y)*(buffer_bpp/8)];
    if( !(*pixels) )
        return false;
    
    /* Read from the current framebuffer */
    glReadPixels( x, y, width, height, GL_RGBA, GL_UNSIGNED_BYTE, *pixels );
    
    return true;
}

/*
 * Name: ke_ogl_renderdevice::set_viewport
 * Desc: Sets the viewport.
 */
void ke_ogles2_renderdevice_t::set_viewport( int x, int y, int width, int height )
{
    /* Set the viewport */
    glViewport( x, y, width, height );
    
    viewport[0] = x;
    viewport[1] = y;
    viewport[2] = width;
    viewport[3] = height;
}


/*
 * Name: ke_ogl_renderdevice::set_perspective_matrix
 * Desc: Sets the projection matrix by creating a perspective matrix.
 */
void ke_ogles2_renderdevice_t::set_perspective_matrix( float fov, float aspect, float near_z, float far_z )
{
    /* Set up projection matrix using the perspective method */
    projection_matrix = M4MakePerspective( fov, aspect, near_z, far_z );
}


/*
 * Name: ke_ogl_renderdevice::set_view_matrix
 * Desc:
 */
void ke_ogles2_renderdevice_t::set_view_matrix( const Matrix4* view )
{
    /* Copy over the incoming view matrix */
    memmove( &view_matrix, view, sizeof( Matrix4 ) );
}


/*
 * Name: ke_ogl_renderdevice::set_world_matrix
 * Desc:
 */
void ke_ogles2_renderdevice_t::set_world_matrix( const Matrix4* world )
{
    /* Copy over the incoming world matrix */
    memmove( &world_matrix, world, sizeof( Matrix4 ) );
}


/*
 * Name: ke_ogl_renderdevice::set_modelview_matrix
 * Desc:
 */
void ke_ogles2_renderdevice_t::set_modelview_matrix( const Matrix4* modelview )
{
    /* Copy over the incoming modelview matrix */
    memmove( &modelview_matrix, modelview, sizeof( Matrix4 ) );
}


/*
 * Name: ke_ogl_renderdevice::set_projection_matrix
 * Desc:
 */
void ke_ogles2_renderdevice_t::set_projection_matrix( const Matrix4* projection )
{
    /* Copy over the incoming projection matrix */
    memmove( &projection_matrix, projection, sizeof( Matrix4 ) );
}


/*
 * Name: ke_ogles2_renderdevice_t::block_until_vertical_blank
 * Desc: Stalls the current thread for an interval equivalent to one
 *       vertical blank. This function does not sync to the actual vertical blank
 *       as I have not found a way to do this on any platform besides Windows.
 *       This is also thread safe.
 */
void ke_ogles2_renderdevice_t::block_until_vertical_blank()
{
    SDL_DisplayMode display_mode;
    
    /* Get the current display mode */
    /* TODO: Get display mode based on windowed or fullscreen mode. */
    SDL_GetWindowDisplayMode( window, &display_mode );
    
    /* Stall this thread for 1000/refresh_rate milliseconds */
    SDL_Delay( 1000 / display_mode.refresh_rate );
}


/*
 * Name: ke_ogles2_renderdevice_t::set_swap_interval
 * Desc: Sets the swap interval (enables/disable vertical sync). See SDL documentation on
 *       SDL_GL_SetSwapInterval for a more detailed description.
 */
void ke_ogles2_renderdevice_t::set_swap_interval( int swap_interval )
{
    SDL_GL_SetSwapInterval( swap_interval );
}


/*
 * Name: ke_ogles2_renderdevice_t::get_swap_interval
 * Desc: Returns the vertical sync value set above.
 */
int ke_ogles2_renderdevice_t::get_swap_interval()
{
    return SDL_GL_GetSwapInterval();
}
