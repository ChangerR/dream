// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __I_ATTRIBUTES_H_INCLUDED__
#define __I_ATTRIBUTES_H_INCLUDED__
#include "dtype.h"
#include "dstring.h"
#include "vector2d.h"
#include "vector3d.h"
#include "EAttributes.h"
#include "IReadFile.h"
#include "matrix4.h"
#include "SColor.h"
//! Provides a generic interface for attributes and their values and the possiblity to serialize them
class IAttributes : public virtual IReferenceCounted
{
public:

	//! Returns amount of attributes in this collection of attributes.
	virtual u32 getAttributeCount() const = 0;

	//! Returns the type of an attribute
	//! \param attributeName: Name for the attribute
	virtual E_ATTRIBUTE_TYPE getAttributeType(const c8* attributeName) = 0;

	//! Returns if an attribute with a name exists
	virtual bool existsAttribute(const c8* attributeName) = 0;

	//! Removes all attributes
	virtual void clear() = 0;

	//! Reads attributes from a xml file.
	//! \param reader The XML reader to read from
	//! \param readCurrentElementOnly If set to true, reading only works if current element has the name 'attributes' or
	//! the name specified using elementName.
	//! \param elementName The surrounding element name. If it is null, the default one, "attributes" will be taken.
	//! If set to false, the first appearing list of attributes are read.
	virtual bool read(IReadFile* reader, bool readCurrentElementOnly=false, const wchar_t* elementName=0) = 0;

	/*

		Integer Attribute

	*/

	//! Adds an attribute as integer
	virtual void addInt(const c8* attributeName, s32 value) = 0;

	//! Sets an attribute as integer value
	virtual void setAttribute(const c8* attributeName, s32 value) = 0;

	//! Gets an attribute as integer value
	//! \param attributeName: Name of the attribute to get.
	//! \return Returns value of the attribute previously set by setAttribute()
	virtual s32 getAttributeAsInt(const c8* attributeName) const =0;

	/*

		Float Attribute

	*/

	//! Adds an attribute as float
	virtual void addFloat(const c8* attributeName, f32 value) = 0;

	//! Sets a attribute as float value
	virtual void setAttribute(const c8* attributeName, f32 value) = 0;

	//! Gets an attribute as float value
	//! \param attributeName: Name of the attribute to get.
	//! \return Returns value of the attribute previously set by setAttribute()
	virtual f32 getAttributeAsFloat(const c8* attributeName) = 0;


	/*

		String Attribute

	*/

	//! Adds an attribute as string
	virtual void addString(const c8* attributeName, const c8* value) = 0;

	//! Sets an attribute value as string.
	//! \param attributeName: Name for the attribute
	//! \param value: Value for the attribute. Set this to 0 to delete the attribute
	virtual void setAttribute(const c8* attributeName, const c8* value) = 0;

	//! Gets an attribute as string.
	//! \param attributeName: Name of the attribute to get.
	//! \return Returns value of the attribute previously set by setAttribute()
	//! or 0 if attribute is not set.
	virtual stringc getAttributeAsString(const c8* attributeName) = 0;

	//! Gets an attribute as string.
	//! \param attributeName Name of the attribute to get.
	//! \param target Buffer where the string is copied to.
	virtual void getAttributeAsString(const c8* attributeName, c8* target) = 0;

	/*

		Binary Data Attribute

	*/

	//! Adds an attribute as binary data
	virtual void addBinary(const c8* attributeName, void* data, s32 dataSizeInBytes) = 0;

	//! Sets an attribute as binary data
	virtual void setAttribute(const c8* attributeName, void* data, s32 dataSizeInBytes ) = 0;

	//! Gets an attribute as binary data
	/** \param attributeName: Name of the attribute to get.
	\param outData Pointer to buffer where data shall be stored.
	\param maxSizeInBytes Maximum number of bytes to write into outData.
	*/
	virtual void getAttributeAsBinaryData(const c8* attributeName, void* outData, s32 maxSizeInBytes) = 0;


	
	/*

		Bool Attribute

	*/

	//! Adds an attribute as bool
	virtual void addBool(const c8* attributeName, bool value) = 0;

	//! Sets an attribute as boolean value
	virtual void setAttribute(const c8* attributeName, bool value) = 0;

	//! Gets an attribute as boolean value
	//! \param attributeName: Name of the attribute to get.
	//! \return Returns value of the attribute previously set by setAttribute()
	virtual bool getAttributeAsBool(const c8* attributeName) = 0;

	
	/*

		SColor Attribute

	*/

	//! Adds an attribute as color
	virtual void addColor(const c8* attributeName, SColor value) = 0;


	//! Sets a attribute as color
	virtual void setAttribute(const c8* attributeName, SColor color) = 0;

	//! Gets an attribute as color
	//! \param attributeName: Name of the attribute to get.
	//! \return Returns value of the attribute previously set by setAttribute()
	virtual SColor getAttributeAsColor(const c8* attributeName) = 0;


	/*

		SColorf Attribute

	*/

	//! Adds an attribute as floating point color
	virtual void addColorf(const c8* attributeName, SColorf value) = 0;

	//! Sets a attribute as floating point color
	virtual void setAttribute(const c8* attributeName, SColorf color) = 0;

	//! Gets an attribute as floating point color
	//! \param attributeName: Name of the attribute to get.
	//! \return Returns value of the attribute previously set by setAttribute()
	virtual SColorf getAttributeAsColorf(const c8* attributeName) = 0;

	/*

		Vector3d Attribute

	*/

	//! Adds an attribute as 3d vector
	virtual void addVector3d(const c8* attributeName, vector3df value) = 0;

	//! Sets a attribute as 3d vector
	virtual void setAttribute(const c8* attributeName, vector3df v) = 0;

	//! Gets an attribute as 3d vector
	//! \param attributeName: Name of the attribute to get.
	//! \return Returns value of the attribute previously set by setAttribute()
	virtual vector3df getAttributeAsVector3d(const c8* attributeName) = 0;

	/*

		Vector2d Attribute

	*/

	//! Adds an attribute as 2d vector
	virtual void addVector2d(const c8* attributeName, vector2df value) = 0;

	//! Sets a attribute as 2d vector
	virtual void setAttribute(const c8* attributeName, vector2df v) = 0;

	//! Gets an attribute as vector
	//! \param attributeName: Name of the attribute to get.
	//! \return Returns value of the attribute previously set by setAttribute()
	virtual vector2df getAttributeAsVector2d(const c8* attributeName) = 0;

	/*

		Position2d Attribute

	*/

	//! Adds an attribute as 2d position
	virtual void addPosition2d(const c8* attributeName, position2d<s32> value) = 0;

	//! Sets a attribute as 2d position
	virtual void setAttribute(const c8* attributeName, position2d<s32> v) = 0;

	//! Gets an attribute as position
	//! \param attributeName: Name of the attribute to get.
	//! \return Returns value of the attribute previously set by setAttribute()
	virtual position2d<s32> getAttributeAsPosition2d(const c8* attributeName) = 0;

	/*

		Rectangle Attribute

	*/

	//! Adds an attribute as rectangle
	virtual void addRect(const c8* attributeName, rectangle<s32> value) = 0;

	//! Sets an attribute as rectangle
	virtual void setAttribute(const c8* attributeName, rectangle<s32> v) = 0;

	//! Gets an attribute as rectangle
	//! \param attributeName: Name of the attribute to get.
	//! \return Returns value of the attribute previously set by setAttribute()
	virtual rectangle<s32> getAttributeAsRect(const c8* attributeName) = 0;


	/*

		Dimension2d Attribute

	*/

	//! Adds an attribute as dimension2d
	virtual void addDimension2d(const c8* attributeName, dimension2d<u32> value) = 0;

	//! Sets an attribute as dimension2d
	virtual void setAttribute(const c8* attributeName, dimension2d<u32> v) = 0;

	//! Gets an attribute as dimension2d
	//! \param attributeName: Name of the attribute to get.
	//! \return Returns value of the attribute previously set by setAttribute()
	virtual dimension2d<u32> getAttributeAsDimension2d(const c8* attributeName) = 0;


	/*
		matrix attribute
	*/

	//! Adds an attribute as matrix
	virtual void addMatrix(const c8* attributeName, const matrix4& v) = 0;

	//! Sets an attribute as matrix
	virtual void setAttribute(const c8* attributeName, const matrix4& v) = 0;

	//! Gets an attribute as a matrix4
	//! \param attributeName: Name of the attribute to get.
	//! \return Returns value of the attribute previously set by setAttribute()
	virtual matrix4 getAttributeAsMatrix(const c8* attributeName) = 0;

};

#endif
