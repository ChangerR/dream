#ifndef __DREAM_CATTRIBUTES__H
#define __DREAM_CATTRIBUTES__H
#include "IAttributes.h"
#include "attrHashMap.h"
#include "SColor.h"
#include "position2d.h"
#include "vector2d.h"
#include "vector3d.h"
#include "rectangle.h"
#include "dimension2d.h"
#include "matrix4.h"

extern u32 default_hash(const c8* str);
class CAttributes :public IAttributes {
public:
	CAttributes():_hash_map(default_hash) {}
	//! Returns amount of attributes in this collection of attributes.
	virtual u32 getAttributeCount() const;

	//! Returns the type of an attribute
	//! \param attributeName: Name for the attribute
	virtual E_ATTRIBUTE_TYPE getAttributeType(const c8* attributeName);

	//! Returns if an attribute with a name exists
	virtual bool existsAttribute(const c8* attributeName);

	//! Removes all attributes
	virtual void clear();

	//! Reads attributes from a xml file.
	//! \param reader The XML reader to read from
	//! \param readCurrentElementOnly If set to true, reading only works if current element has the name 'attributes' or
	//! the name specified using elementName.
	//! \param elementName The surrounding element name. If it is null, the default one, "attributes" will be taken.
	//! If set to false, the first appearing list of attributes are read.
	virtual bool read(IReadFile* reader, bool readCurrentElementOnly=false, const wchar_t* elementName=0);

	/*

		Integer Attribute

	*/

	//! Adds an attribute as integer
	virtual void addInt(const c8* attributeName, s32 value);

	//! Sets an attribute as integer value
	virtual void setAttribute(const c8* attributeName, s32 value);

	//! Gets an attribute as integer value
	//! \param attributeName: Name of the attribute to get.
	//! \return Returns value of the attribute previously set by setAttribute()
	virtual s32 getAttributeAsInt(const c8* attributeName) const;

	/*

		Float Attribute

	*/

	//! Adds an attribute as float
	virtual void addFloat(const c8* attributeName, f32 value);

	//! Sets a attribute as float value
	virtual void setAttribute(const c8* attributeName, f32 value);

	//! Gets an attribute as float value
	//! \param attributeName: Name of the attribute to get.
	//! \return Returns value of the attribute previously set by setAttribute()
	virtual f32 getAttributeAsFloat(const c8* attributeName);


	/*

		String Attribute

	*/

	//! Adds an attribute as string
	virtual void addString(const c8* attributeName, const c8* value);

	//! Sets an attribute value as string.
	//! \param attributeName: Name for the attribute
	//! \param value: Value for the attribute. Set this to 0 to delete the attribute
	virtual void setAttribute(const c8* attributeName, const c8* value);

	//! Gets an attribute as string.
	//! \param attributeName: Name of the attribute to get.
	//! \return Returns value of the attribute previously set by setAttribute()
	//! or 0 if attribute is not set.
	virtual stringc getAttributeAsString(const c8* attributeName);

	//! Gets an attribute as string.
	//! \param attributeName Name of the attribute to get.
	//! \param target Buffer where the string is copied to.
	virtual void getAttributeAsString(const c8* attributeName, c8* target);

	/*

		Binary Data Attribute

	*/

	//! Adds an attribute as binary data
	virtual void addBinary(const c8* attributeName, void* data, s32 dataSizeInBytes);

	//! Sets an attribute as binary data
	virtual void setAttribute(const c8* attributeName, void* data, s32 dataSizeInBytes );

	//! Gets an attribute as binary data
	/** \param attributeName: Name of the attribute to get.
	\param outData Pointer to buffer where data shall be stored.
	\param maxSizeInBytes Maximum number of bytes to write into outData.
	*/
	virtual void getAttributeAsBinaryData(const c8* attributeName, void* outData, s32 maxSizeInBytes);


	
	/*

		Bool Attribute

	*/

	//! Adds an attribute as bool
	virtual void addBool(const c8* attributeName, bool value);

	//! Sets an attribute as boolean value
	virtual void setAttribute(const c8* attributeName, bool value);

	//! Gets an attribute as boolean value
	//! \param attributeName: Name of the attribute to get.
	//! \return Returns value of the attribute previously set by setAttribute()
	virtual bool getAttributeAsBool(const c8* attributeName);

	
	/*

		SColor Attribute

	*/

	//! Adds an attribute as color
	virtual void addColor(const c8* attributeName, SColor value);


	//! Sets a attribute as color
	virtual void setAttribute(const c8* attributeName, SColor color);

	//! Gets an attribute as color
	//! \param attributeName: Name of the attribute to get.
	//! \return Returns value of the attribute previously set by setAttribute()
	virtual SColor getAttributeAsColor(const c8* attributeName);


	/*

		SColorf Attribute

	*/

	//! Adds an attribute as floating point color
	virtual void addColorf(const c8* attributeName, SColorf value);

	//! Sets a attribute as floating point color
	virtual void setAttribute(const c8* attributeName, SColorf color);

	//! Gets an attribute as floating point color
	//! \param attributeName: Name of the attribute to get.
	//! \return Returns value of the attribute previously set by setAttribute()
	virtual SColorf getAttributeAsColorf(const c8* attributeName);

	/*

		Vector3d Attribute

	*/

	//! Adds an attribute as 3d vector
	virtual void addVector3d(const c8* attributeName, vector3df value);

	//! Sets a attribute as 3d vector
	virtual void setAttribute(const c8* attributeName, vector3df v);

	//! Gets an attribute as 3d vector
	//! \param attributeName: Name of the attribute to get.
	//! \return Returns value of the attribute previously set by setAttribute()
	virtual vector3df getAttributeAsVector3d(const c8* attributeName);

	/*

		Vector2d Attribute

	*/

	//! Adds an attribute as 2d vector
	virtual void addVector2d(const c8* attributeName, vector2df value);

	//! Sets a attribute as 2d vector
	virtual void setAttribute(const c8* attributeName, vector2df v);

	//! Gets an attribute as vector
	//! \param attributeName: Name of the attribute to get.
	//! \return Returns value of the attribute previously set by setAttribute()
	virtual vector2df getAttributeAsVector2d(const c8* attributeName);

	/*

		Position2d Attribute

	*/

	//! Adds an attribute as 2d position
	virtual void addPosition2d(const c8* attributeName, position2d<s32> value);

	//! Sets a attribute as 2d position
	virtual void setAttribute(const c8* attributeName, position2d<s32> v);

	//! Gets an attribute as position
	//! \param attributeName: Name of the attribute to get.
	//! \return Returns value of the attribute previously set by setAttribute()
	virtual position2d<s32> getAttributeAsPosition2d(const c8* attributeName);

	/*

		Rectangle Attribute

	*/

	//! Adds an attribute as rectangle
	virtual void addRect(const c8* attributeName, rectangle<s32> value);

	//! Sets an attribute as rectangle
	virtual void setAttribute(const c8* attributeName, rectangle<s32> v);

	//! Gets an attribute as rectangle
	//! \param attributeName: Name of the attribute to get.
	//! \return Returns value of the attribute previously set by setAttribute()
	virtual rectangle<s32> getAttributeAsRect(const c8* attributeName);


	/*

		Dimension2d Attribute

	*/

	//! Adds an attribute as dimension2d
	virtual void addDimension2d(const c8* attributeName, dimension2d<u32> value);

	//! Sets an attribute as dimension2d
	virtual void setAttribute(const c8* attributeName, dimension2d<u32> v);

	//! Gets an attribute as dimension2d
	//! \param attributeName: Name of the attribute to get.
	//! \return Returns value of the attribute previously set by setAttribute()
	virtual dimension2d<u32> getAttributeAsDimension2d(const c8* attributeName);


	/*
		matrix attribute
	*/

	//! Adds an attribute as matrix
	virtual void addMatrix(const c8* attributeName, const matrix4& v);

	//! Sets an attribute as matrix
	virtual void setAttribute(const c8* attributeName, const matrix4& v);

	//! Gets an attribute as a matrix4
	//! \param attributeName: Name of the attribute to get.
	//! \return Returns value of the attribute previously set by setAttribute()
	virtual matrix4 getAttributeAsMatrix(const c8* attributeName);


private:
	attrHashMap _hash_map;
};

#endif