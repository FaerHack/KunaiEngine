//
//  ke_d3d11_renderdevice.h
//
//  Created by Shogun3D on 8/24/14.
//  Copyright (c) 2014 Shogun3D. All rights reserved.
//

#ifndef __ke_d3d11_renderdevice__
#define __ke_d3d11_renderdevice__

#pragma warning(disable:4838)

#include "KeRenderDevice.h"
#ifndef _WIN32
#error "Direct3D is not supported on non-Microsoft platforms!"
#else
#include <SDL.h>
#include <d3d11.h>
#include <d3d11_1.h>
#include <d3d11_2.h>
#include <d3d11_3.h>
#include <d3d11_4.h>
#include <dxgi1_5.h>
#include <dxgi1_6.h>
#include <dxgidebug.h>
#include <D3Dcompiler.h>
#include <DirectXMath.h>
#include <comip.h>
#include <comdef.h>
#include <ddraw.h>
#endif


/*
 * Non-ATL based smart COM pointer types
 */
#define _ComPtr(_interface)	_com_ptr_t<_com_IIID<_interface, &__uuidof(_interface)>>

#ifndef _UWP	/* For Desktop Windows only */
//typedef _ComPtr(IDirectDraw7)				CDirectDraw7;
typedef _com_ptr_t<_com_IIID<IDirectDraw7, &IID_IDirectDraw7>>	CDirectDraw7;
#endif

typedef _ComPtr(ID3D11Device)				CD3D11Device;
typedef _ComPtr(ID3D11DeviceContext)		CD3D11DeviceContext;
typedef _ComPtr(IDXGISwapChain)				CDXGISwapChain;
typedef _ComPtr(IDXGIOutput)				CDXGIOutput;
typedef _ComPtr(IDXGIDevice)				CDXGIDevice;
typedef _ComPtr(IDXGIDevice1)				CDXGIDevice1;
typedef _ComPtr(IDXGIAdapter)				CDXGIAdapter;
typedef _ComPtr(IDXGIFactory)				CDXGIFactory;
typedef _ComPtr(IDXGIFactory1)				CDXGIFactory1;
typedef _ComPtr(ID3D11RenderTargetView)		CD3D11RenderTargetView;
typedef _ComPtr(ID3D11DepthStencilView)		CD3D11DepthStencilView;
typedef _ComPtr(ID3D11Buffer)				CD3D11Buffer;
typedef _ComPtr(ID3D11VertexShader)			CD3D11VertexShader;
typedef _ComPtr(ID3D11PixelShader)			CD3D11PixelShader;
typedef _ComPtr(ID3D11GeometryShader)		CD3D11GeometryShader;
typedef _ComPtr(ID3D11HullShader)			CD3D11HullShader;
typedef _ComPtr(ID3D11DomainShader)			CD3D11DomainShader;
typedef _ComPtr(ID3D11ComputeShader)		CD3D11ComputeShader;
typedef _ComPtr(ID3D11InputLayout)			CD3D11InputLayout;
typedef _ComPtr(ID3D11Texture1D)			CD3D11Texture1D;
typedef _ComPtr(ID3D11Texture2D)			CD3D11Texture2D;
typedef _ComPtr(ID3D11Texture3D)			CD3D11Texture3D;
typedef _ComPtr(ID3D11BlendState)			CD3D11BlendState;
typedef _ComPtr(ID3D11RasterizerState)		CD3D11RasterizerState;
typedef _ComPtr(ID3D11DepthStencilState)	CD3D11DepthStencilState;
typedef _ComPtr(ID3D11SamplerState)			CD3D11SamplerState;
typedef _ComPtr(ID3D11Query)				CD3D11Query;
typedef _ComPtr(ID3D11CommandList)			CD3D11CommandList;
typedef _ComPtr(ID3D11ShaderResourceView)	CD3D11ShaderResourceView;
typedef _ComPtr(ID3D10Blob)					CD3D10Blob;
typedef _ComPtr(ID3D11Debug)				CD3D11Debug;
typedef _ComPtr(ID3D11InfoQueue)			CD3D11InfoQueue;

#ifdef __d3d11_1_h__
typedef _ComPtr(ID3D11Device1)				CD3D11Device1;
typedef _ComPtr(ID3D11DeviceContext1)		CD3D11DeviceContext1;
typedef _ComPtr(IDXGISwapChain1)			CDXGISwapChain1;
typedef _ComPtr(IDXGIOutput1)				CDXGIOutput1;
typedef _ComPtr(IDXGIDevice2)				CDXGIDevice2;
typedef _ComPtr(IDXGIFactory2)				CDXGIFactory2;
typedef _ComPtr(ID3D11BlendState1)			CD3D11BlendState1;
typedef _ComPtr(ID3D11RasterizerState1)		CD3D11RasterizerState1;
#endif

#ifdef __d3d11_2_h__
typedef _ComPtr(ID3D11Device2)				CD3D11Device2;
typedef _ComPtr(ID3D11DeviceContext2)		CD3D11DeviceContext2;
typedef _ComPtr(IDXGISwapChain2)			CDXGISwapChain2;
typedef _ComPtr(IDXGISwapChain3)			CDXGISwapChain3;
//typedef _ComPtr(IDXGISwapChainMedia)		CDXGISwapChainMedia;
typedef _ComPtr(IDXGIDevice3)				CDXGIDevice3;
#endif

#ifdef __dxgidebug_h__
typedef _ComPtr(IDXGIInfoQueue)				CDXGIInfoQueue;
typedef _ComPtr(IDXGIDebug)					CDXGIDebug;
typedef _ComPtr(IDXGIDebug1)				CDXGIDebug1;
#endif

#ifdef __dxgi1_6_h__
typedef _ComPtr(IDXGIOutput6)				CDXGIOutput6;
#endif


/*
 * Constant buffer structure
 */
struct IKeDirect3D11ConstantBuffer : public IKeConstantBuffer
{
	KEMETHOD            Destroy();
    
    _KEMETHOD(void*)    MapData( uint32_t flags );
    KEMETHOD            UnmapData( void* );
    
    _KEMETHOD(bool)     SetConstantData( uint32_t offset, uint32_t size, void* ptr );
    KEMETHOD            GetDesc( KeConstantBufferDesc* desc );
    
	CD3D11Buffer	cb;				/* Constant buffer */
	uint32_t		data_size;      /* Size of the data buffer */
    uint32_t		flags;          /* Buffer creation flags */
    uint32_t		block_index;    /* Block index representing the block name we are attempting to access */
    char			block_name[64]; /* Name of the block/struct within the GPU program we are writing data to */
};

/*
 * Geometry buffer structure
 */
struct IKeDirect3D11GeometryBuffer : public IKeGeometryBuffer
{
	KEMETHOD Destroy();
    
    _KEMETHOD(void*) MapData( uint32_t flags );
	_KEMETHOD(void*) MapDataAsync( uint32_t flags );
    KEMETHOD UnmapData( void* );
	KEMETHOD UnmapDataAsync( void* );

	_KEMETHOD(bool) SetVertexData( uint32_t offset, uint32_t size, void* ptr );
    _KEMETHOD(bool) SetIndexData( uint32_t offset, uint32_t size, void* ptr );
	KEMETHOD GetDesc( KeGeometryBufferDesc* desc );

	CD3D11Buffer	vb;		/* Vertex buffer */
	CD3D11Buffer	ib;		/* Index buffer */
	uint32_t stride;		/* Length of vertex data (in bytes) */
	uint32_t index_type;	/* Data type for index data */
};

/*
 * Command list structure
 */
struct IKeDirect3D11CommandList : public IKeCommandList
{
	KEMETHOD Destroy();

	CD3D11CommandList cl;
	CD3D11DeviceContext deferred_ctxt;
};

/*
 * GPU Program structure
 */
struct IKeDirect3D11GpuProgram : public IKeGpuProgram
{
	KEMETHOD Destroy();
	KEMETHOD GetVertexAttributes( KeVertexAttribute* vertex_attributes );
    
	CD3D11VertexShader		vs;		/* Vertex shader */
	CD3D11PixelShader		ps;		/* Pixel shader */
	CD3D11GeometryShader	gs;		/* Geometry shader */
	CD3D11HullShader		hs;		/* Hull shader */
	CD3D11DomainShader		ds;		/* Domain shader */
	CD3D11ComputeShader		cs;		/* Compute shader */
	CD3D11InputLayout		il;		/* Vertex input layout */
	KeVertexAttribute*		va;
};

/*
 * Texture base structure
 */
struct IKeDirect3D11Texture : public IKeTexture
{
	KEMETHOD Destroy();

    _KEMETHOD(void*) MapData( uint32_t flags );
    KEMETHOD UnmapData( void* );

	_KEMETHOD(bool) SetTextureData( KeTextureDesc* texture_data, void* pixels );
	_KEMETHOD(bool) GetTextureDesc( KeTextureDesc* texture_desc );

	CD3D11Texture1D		tex1d;
	CD3D11Texture2D		tex2d;
	CD3D11Texture3D		tex3d;
	uint32_t			flags;

    uint32_t width, height;     /* Texture width/height */
    uint32_t depth;             /* Texture depth (for 3D and array textures) */
    uint32_t depth_format;      /* See glTexImageXD */
    uint32_t internal_format;   /* See glTexImageXD */
    uint32_t data_type;         /* Internal data type */
    uint32_t target;            /* Direct3D11 texture type */
};

/*
 * Rendertarget base structure
 */
struct IKeDirect3D11RenderTarget : public IKeRenderTarget
{
	KEMETHOD Destroy();
    
    _KEMETHOD(void*) MapData( uint32_t flags );
    KEMETHOD UnmapData( void* );

	_KEMETHOD(bool) GetTexture( IKeTexture** texture );
	virtual IKeTexture* GetTexture2();

	uint32_t    frame_buffer_object;    /* Frame buffer object handle */
	uint32_t    depth_render_buffer;    /* Depth render buffer */
	/* TODO: Stencil? */

	IKeDirect3D11Texture* texture;
};

/*
 * Palette base structure
 */
struct IKeDirect3D11Palette : public IKePalette
{

};

/*
 * GPU fence structure
 */
struct IKeDirect3D11Fence : public IKeFence
{
	KEMETHOD Destroy();
    
	_KEMETHOD(bool) Insert();
    _KEMETHOD(bool) Test();
    KEMETHOD Block();
    _KEMETHOD(bool) Valid();

	CD3D11Query			query;
	CD3D11DeviceContext context;	/* Copy of the render device's context */
};

/*
 * Render/Texture state structure
 */
struct IKeDirect3D11RenderStateBuffer : public IKeRenderStateBuffer
{
    KEMETHOD Destroy();
	bool PVT_CreateWithDefaults( CD3D11Device device );
	bool PVT_CreateWithDefaults( CD3D11Device1 device );
	bool PVT_CreateWithDefaults( CD3D11Device2 device );

	CD3D11BlendState			bs;
	CD3D11RasterizerState		rs;
	CD3D11DepthStencilState		dss;
};

struct IKeDirect3D11TextureSamplerBuffer : public IKeTextureSamplerBuffer
{
	KEMETHOD Destroy();

	CD3D11SamplerState			ss;
};


/*
 * Render device base class
 */
class IKeDirect3D11RenderDevice : public IKeRenderDevice
{
public:
	IKeDirect3D11RenderDevice();
	IKeDirect3D11RenderDevice( KeRenderDeviceDesc* renderdevice_desc );
	virtual ~IKeDirect3D11RenderDevice();

public:
	/* Misc */
    _KEMETHOD(bool) ConfirmDevice();
    KEMETHOD GetDeviceDesc( KeRenderDeviceDesc* device_desc );
	KEMETHOD GetDeviceCaps( KeRenderDeviceCaps* device_caps );
    
    /* General rendering stuff */
    KEMETHOD SetClearColourFV( float* colour );
    KEMETHOD SetClearColourUBV( uint8_t* colour );
    KEMETHOD SetClearDepth( float depth );
	KEMETHOD SetClearStencil( uint32_t stencil );
    KEMETHOD ClearColourBuffer();
    KEMETHOD ClearDepthBuffer();
    KEMETHOD ClearStencilBuffer();
	KEMETHOD Clear( uint32_t buffers );
	KEMETHOD ClearState();
    KEMETHOD Swap();
	_KEMETHOD(bool) ResizeRenderTargetAndDepthStencil( int width, int height );
    
	KEMETHOD SetIMCacheSize( uint32_t cache_size );
    _KEMETHOD(bool) CreateGeometryBuffer( void* vertex_data, uint32_t vertex_data_size, void* index_data, uint32_t index_data_size, uint32_t index_data_type, uint32_t flags, KeVertexAttribute* va, IKeGeometryBuffer** geometry_buffer );
    KEMETHOD DeleteGeometryBuffer( IKeGeometryBuffer* geometry_buffer );
    KEMETHOD SetGeometryBuffer( IKeGeometryBuffer* geometry_buffer );
	_KEMETHOD(bool) CreateCommandList( IKeCommandList** command_list );
	_KEMETHOD(bool) BeginCommandList( IKeCommandList* command_list );
	_KEMETHOD(bool) EndCommandList( IKeCommandList** command_list, int restore_state );
	_KEMETHOD(bool) ExecuteCommandList( IKeCommandList* command_list, int restore_state );
	KEMETHOD RestoreImmediateContext();
    _KEMETHOD(bool) CreateProgram( const char* vertex_shader, const char* fragment_shader, const char* geometry_shader, const char* tesselation_shader, KeVertexAttribute* vertex_attributes, IKeGpuProgram** gpu_program );
    KEMETHOD DeleteProgram( IKeGpuProgram* gpu_program );
    KEMETHOD SetProgram( IKeGpuProgram* gpu_program );
    KEMETHOD SetProgramConstant1FV( const char* location, int count, float* value );
    KEMETHOD SetProgramConstant2FV( const char* location, int count, float* value );
    KEMETHOD SetProgramConstant3FV( const char* location, int count, float* value );
    KEMETHOD SetProgramConstant4FV( const char* location, int count, float* value );
    KEMETHOD SetProgramConstant1IV( const char* location, int count, int* value );
    KEMETHOD SetProgramConstant2IV( const char* location, int count, int* value );
    KEMETHOD SetProgramConstant3IV( const char* location, int count, int* value );
    KEMETHOD SetProgramConstant4IV( const char* location, int count, int* value );
    KEMETHOD GetProgramConstantFV( const char* location, float* value );
    KEMETHOD GetProgramConstantIV( const char* location, int* value );
	_KEMETHOD(bool) CreateConstantBuffer( KeConstantBufferDesc* desc, IKeConstantBuffer** constant_buffer, void* data = NULL );
	KEMETHOD DeleteConstantBuffer( IKeConstantBuffer* constant_buffer );
	_KEMETHOD(bool) SetConstantBufferData( void* data, IKeConstantBuffer* constant_buffer );
	KEMETHOD SetConstantBuffer( int slot, int shader_type, IKeConstantBuffer* constant_buffer );
    _KEMETHOD(bool) CreateTexture1D( uint32_t target, int width, int mipmaps, uint32_t format, uint32_t data_type, IKeTexture** texture, void* pixels = NULL );
    _KEMETHOD(bool) CreateTexture2D( uint32_t target, int width, int height, int mipmaps, uint32_t format, uint32_t data_type, IKeTexture** texture, void* pixels = NULL );
    _KEMETHOD(bool) CreateTexture3D( uint32_t target, int width, int height, int depth, int mipmaps, uint32_t format, uint32_t data_type, IKeTexture** texture, void* pixels = NULL );
    KEMETHOD DeleteTexture( IKeTexture* texture );
    KEMETHOD SetTextureData1D( int offsetx, int width, int miplevel, void* pixels, IKeTexture* texture );
    KEMETHOD SetTextureData2D( int offsetx, int offsety, int width, int height, int miplevel, void* pixels, IKeTexture* texture );
    KEMETHOD SetTextureData3D( int offsetx, int offsety, int offsetz, int width, int height, int depth, int miplevel, void* pixels, IKeTexture* texture );
    _KEMETHOD(bool) CreateRenderTarget( int width, int height, int depth, uint32_t flags, IKeRenderTarget** rendertarget );
    KEMETHOD DeleteRenderTarget( IKeRenderTarget* rendertarget );
    KEMETHOD BindRenderTarget( IKeRenderTarget* rendertarget );
    KEMETHOD SetTexture( int stage, IKeTexture* texture );
	_KEMETHOD(bool) CreateRenderStateBuffer( KeState* state_params, int state_count, IKeRenderStateBuffer** state_buffer );
	_KEMETHOD(bool) CreateTextureSamplerBuffer( KeState* state_params, int state_count, IKeTextureSamplerBuffer** state_buffer );
	_KEMETHOD(bool) SetRenderStateBuffer( IKeRenderStateBuffer* state_buffer );
	_KEMETHOD(bool) SetTextureSamplerBuffer( int stage, IKeTextureSamplerBuffer* state_buffer );
	KEMETHOD SetRenderStates( KeState* states );
	KEMETHOD SetSamplerStates( int stage, KeState* states );
	KEMETHOD DrawVerticesIM( uint32_t primtype, uint32_t stride, KeVertexAttribute* vertex_attributes, int first, int count, void* vertex_data );
    KEMETHOD DrawIndexedVerticesIM( uint32_t primtype, uint32_t stride, KeVertexAttribute* vertex_attributes, int count, void* vertex_data, void* index_data );
    KEMETHOD DrawIndexedVerticesRangeIM( uint32_t primtype, uint32_t stride, KeVertexAttribute* vertex_attributes, int start, int end, int count, void* vertex_data, void* index_data );
    KEMETHOD DrawVertices( uint32_t primtype, uint32_t stride, int first, int count );
    KEMETHOD DrawIndexedVertices( uint32_t primtype, uint32_t stride, int count );
    KEMETHOD DrawIndexedVerticesRange( uint32_t primtype, uint32_t stride, int start, int end, int count );
    
    _KEMETHOD(bool) GetFramebufferRegion( int x, int y, int width, int height, uint32_t flags, int* bpp, void** pixels );
    
    /* Matrix/viewport related */
    KEMETHOD SetViewport( int x, int y, int width, int height );
    KEMETHOD SetViewportV( int* viewport );
    KEMETHOD GetViewport( int* x, int* y, int* width, int* height );
    KEMETHOD GetViewportV( int* viewport );
    KEMETHOD SetPerspectiveMatrix( float fov, float aspect, float near_z, float far_z );
    KEMETHOD SetViewMatrix( const nv::matrix4f* view );
    KEMETHOD SetWorldMatrix( const nv::matrix4f* world );
    KEMETHOD SetModelviewMatrix( const nv::matrix4f* modelview );
    KEMETHOD SetProjectionMatrix( const nv::matrix4f* projection );
	KEMETHOD GetViewMatrix( nv::matrix4f* view );
    KEMETHOD GetWorldMatrix( nv::matrix4f* world );
    KEMETHOD GetModelviewMatrix( nv::matrix4f* modelview );
    KEMETHOD GetProjectionMatrix( nv::matrix4f* projection );
    
    /* Synchronization */
    KEMETHOD BlockUntilVerticalBlank();
    KEMETHOD SetSwapInterval( int swap_interval );
    virtual int GetSwapInterval();
	KEMETHOD BlockUntilIdle();
	KEMETHOD Kick();
	_KEMETHOD(bool) CreateFence( IKeFence** fence, uint32_t flags );
    
    /* Misc */
    KEMETHOD GpuMemoryInfo( uint32_t* total_memory, uint32_t* free_memory );
	KEMETHOD Trim();

protected:
#ifdef _UWP
	bool PVT_InitializeDirect3DUWP();
#else
	bool PVT_InitializeDirect3DWin32();
#endif
	bool PVT_SdkLayersAvaiable();
	void PVT_UpdateColourSpace( bool enable_hdr, DXGI_FORMAT rtfmt );

protected:
	SDL_Window*						window;
	D3D_DRIVER_TYPE					driver_type;
	D3D_FEATURE_LEVEL				feature_level;
	DXGI_COLOR_SPACE_TYPE			colour_space;		/* Keep track of the colour space we are using (HDR10, sRGB, etc.) */
	CD3D11Device					d3ddevice;
	CD3D11DeviceContext				immediate_context;	/* Our actual immediate device context */
	ID3D11DeviceContext*			d3ddevice_context;	/* The pointer to the currently active device context (can be changed to a deferred context) */
#ifdef _UWP
	CDXGISwapChain3					dxgi_swap_chain;
#else
	CDXGISwapChain					dxgi_swap_chain; 
#endif
	CDXGIOutput						dxgi_output;
	CD3D11RenderTargetView			d3d_render_target_view;
	CD3D11Texture2D					d3d_depth_stencil_buffer;
	CD3D11DepthStencilView			d3d_depth_stencil_view;
#ifdef _UWP
	DXGI_SWAP_CHAIN_DESC1			swapchain_desc;
#else
	DXGI_SWAP_CHAIN_DESC			swapchain_desc;
#endif
	int								swap_interval;
#ifndef _UWP
	CDirectDraw7					dd;
#else
	CDXGIDevice3					dxgi_device;
	CDXGIAdapter					dxgi_adapter;
	CDXGIFactory2					dxgi_factory;
#endif
	uint32_t						im_cache_size;
    IKeGeometryBuffer*				im_gb;
	IKeDirect3D11RenderStateBuffer*	rsb_default;
};

#endif /* defined(__ke_d3d11_renderdevice__) */
