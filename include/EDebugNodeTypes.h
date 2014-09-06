#ifndef __E_DEBUG_NODE_TYPES_H_INCLUDED__
#define __E_DEBUG_NODE_TYPES_H_INCLUDED__

//! An enumeration for all types of debug data for built-in scene nodes (flags)
enum E_DEBUG_NODE_TYPE
{
	//! No Debug Data ( Default )
	EDN_OFF = 0,

	//! Show Bounding Boxes of SceneNode
	EDN_BBOX = 1,

	//! Show Vertex Normals
	EDN_NORMALS = 2,

	//! Shows Skeleton/Tags
	EDN_SKELETON = 4,

	//! Overlays Mesh Wireframe
	EDN_MESH_WIRE_OVERLAY = 8,

	//! Temporary use transparency Material Type
	EDN_HALF_TRANSPARENCY = 16,

	//! Show Bounding Boxes of all MeshBuffers
	EDN_BBOX_BUFFERS = 32,

	//! EDN_BBOX | EDN_BBOX_BUFFERS
	EDN_BBOX_ALL = EDN_BBOX | EDN_BBOX_BUFFERS,

	//! Show all debug infos
	EDN_FULL = 0xffffffff
};



#endif // __E_DEBUG_NODE_TYPES_H_INCLUDED__

