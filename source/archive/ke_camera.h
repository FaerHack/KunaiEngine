//
//  ke_camera.h
//
//  Created by Shogun3D on 8/4/14.
//  Copyright (c) 2014 Shogun3D. All rights reserved.
//

#ifndef __ke_camera__
#define __ke_camera__

#include "ke_platform.h"
#include <vectormath.h>


/*
 * Camera base class
 */
class ke_camera_t
{
public:
    ke_camera_t() : position(vmathV3MakeFromElems_V( 0, 0, 0 )), rotation(vmathV3MakeFromElems_V( 0, 0, 0 )) {};
    
    virtual void get_world_matrix( Matrix4* world_matrix )
    {
        /* Negate camera position */
        Vector3 neg_position = vmathV3Neg_V( position );
        
        /* Create a new world matrix based on the position and rotation of this camera */
#if 0
        vmathM4MakeIdentity( world_matrix );
        vmathM4MakeRotationX( world_matrix, rotation.x );
        vmathM4MakeRotationY( world_matrix, rotation.y );
        vmathM4MakeTranslation( world_matrix, &neg_position );
#else
        Matrix4 rotx, roty, trans;
        rotx = roty = trans = vmathM4MakeIdentity_V();
        
        rotx = vmathM4MakeRotationX_V( rotation.x );
        roty = vmathM4MakeRotationY_V( rotation.y );
        trans = vmathM4MakeTranslation_V( neg_position );
        vmathM4Mul( world_matrix, &roty, &rotx );
        vmathM4Mul( world_matrix, world_matrix, &trans );
#endif
    }
    
    virtual void set_position( Vector3 position )
    {
        this->position = position;
    }
    
    virtual void get_position( Vector3& position )
    {
        position = this->position;
    }
    
    virtual void set_rotation( Vector3 rotation )
    {
        this->rotation = rotation;
    }
    
    virtual void get_rotation( Vector3& rotation )
    {
        rotation = this->rotation;
    }
    
    virtual void set_radius( float radius )
    {
        this->radius = radius;
    }
    
    virtual void get_radius( float& radius )
    {
        radius = this->radius;
    }
    
    virtual void set_keyboard_callback( void (*callback)(ke_camera_t*, void*) )
    {
        keyboard_callback = callback;
    }
    
    virtual void set_gamepad_callback( void (*callback)(ke_camera_t*, void*) )
    {
        gamepad_callback = callback;
    }
    
    virtual void on_mouse_down( float x, float y, int button, int down )
    {
        
    }
    
    virtual void on_mouse_move( float x, float y )
    {
        float diffx = x - lastx;
        float diffy = y - lasty;
        
        lastx = x;
        lasty = y;
        
        rotation.x += (float) diffy;
        rotation.y += (float) diffx;
        
        /* TODO: Customized X Axis limits */
        /*if( camera.vecRot.x < -30.0f )
            camera.vecRot.x = -30.0f;
        if( camera.vecRot.x > 90.0f )
            camera.vecRot.x = 90.0f;*/
    }
    
    virtual void on_key_press( void* context )
    {
        if( keyboard_callback )
            keyboard_callback( this, context );
    }
    
    virtual void on_gamepad_press( void* context )
    {
        if( gamepad_callback )
            gamepad_callback( this, context );
    }
    
public:
    Vector3 position, rotation;     /* Camera orientation */
    float   lastx, lasty;           /* Last mouse positions (for PC/Mac controls) */
    float   radius;                 /* Distance between the camera and the entity (3rd person) */
    
protected:
    void (*keyboard_callback)(ke_camera_t* camera, void* context);     /* Keyboard input callback */
    void (*gamepad_callback)(ke_camera_t* camera, void* context);   /* Gamepad input callback */
};

/* New camera creation function */
ke_camera_t* ke_new_camera();

#endif /* defined(__ke_camera__) */
