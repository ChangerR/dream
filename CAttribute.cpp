#include "CAttributes.h"
u32 default_hash(const c8* str) {
	u32 ret = 0;
	while (*str)
	{
		ret = ((ret << 25)|(ret >>7));
		ret += *str;
		str++;
	}
	return ret;
}

u32 CAttributes::getAttributeCount() const{
	return _hash_map.size();
}

E_ATTRIBUTE_TYPE CAttributes::getAttributeType(const c8* attributeName) {
	stringc tmp = attributeName;
	return _hash_map.getElementType(tmp);
}


bool CAttributes::existsAttribute(const c8* attributeName) {
	stringc tmp = attributeName;
	return _hash_map.exist(tmp);
}

void CAttributes::clear() {
	_hash_map.clear();
}

bool CAttributes::read(IReadFile* reader, bool readCurrentElementOnly, const wchar_t* elementName) {
	return false;
}

void CAttributes::addInt(const c8* attributeName, s32 value){
	stringc tmp = attributeName;
	_hash_map.add(tmp,EAT_INT,&value,4);
}

void CAttributes::setAttribute(const c8* attributeName, s32 value) {
	addInt(attributeName,value);
}

s32 CAttributes::getAttributeAsInt(const c8* attributeName) const {
	stringc tmp = attributeName;
	s32 ret = 0;
	_hash_map.getElementbuf(tmp,&ret,4);
	return ret;
}

void CAttributes::addFloat(const c8* attributeName, f32 value) {
	stringc tmp = attributeName;
	_hash_map.add(tmp,EAT_FLOAT,&value,4);
}

void CAttributes::setAttribute(const c8* attributeName, f32 value) {
	addFloat(attributeName,value);
}

f32 CAttributes::getAttributeAsFloat(const c8* attributeName) {
	stringc tmp = attributeName;
	float ret = 0;
	_hash_map.getElementbuf(tmp,&ret,4);
	return ret;
}

void CAttributes::addString(const c8* attributeName, const c8* value) {
	stringc tmp = attributeName;
	int len = 0;
	const char *p = value;
	while(*p++)len++;
	_hash_map.add(tmp,EAT_FLOAT,value,len >= 80? 79:len);
}

void CAttributes::setAttribute(const c8* attributeName, const c8* value) {
	addString(attributeName,value);
}

stringc CAttributes::getAttributeAsString(const c8* attributeName) {
	stringc tmp = attributeName;
	char buf[80];
	_hash_map.getElementbuf(tmp,buf,80);
	return stringc(buf);
}

void CAttributes::getAttributeAsString(const c8* attributeName, c8* target) {
	stringc s = getAttributeAsString(attributeName);
	s.copy_string_to_buf(target,s.size());
}

void CAttributes::addBinary(const c8* attributeName, void* data, s32 dataSizeInBytes) {
	stringc tmp = attributeName;
	_hash_map.add(tmp,EAT_BINARY,data,dataSizeInBytes);
}

void CAttributes::setAttribute(const c8* attributeName, void* data, s32 dataSizeInBytes ) {
	addBinary(attributeName,data,dataSizeInBytes);
}

void CAttributes::getAttributeAsBinaryData(const c8* attributeName, void* outData, s32 maxSizeInBytes) {
	stringc tmp = attributeName;
	_hash_map.getElementbuf(tmp,outData,maxSizeInBytes);
}

void CAttributes::addBool(const c8* attributeName, bool value) {
	stringc tmp = attributeName;
	_hash_map.add(tmp,EAT_BOOL,&value,sizeof(bool));
}

void CAttributes::setAttribute(const c8* attributeName, bool value) {
	addBool(attributeName,value);
}

bool CAttributes::getAttributeAsBool(const c8* attributeName) {
	stringc tmp = attributeName;
	bool ret;
	_hash_map.getElementbuf(tmp,&ret,sizeof(bool));
	return ret;
}

void CAttributes::addColor(const c8* attributeName, SColor value) {
	stringc tmp = attributeName;
	_hash_map.add(tmp,EAT_COLOR,&value,sizeof(SColor));
}

void CAttributes::setAttribute(const c8* attributeName, SColor color) {
	addColor(attributeName,color);
}

SColor CAttributes::getAttributeAsColor(const c8* attributeName) {
	stringc tmp = attributeName;
	SColor ret ;
	_hash_map.getElementbuf(tmp,&ret,sizeof(SColor));
	return ret;
}

void CAttributes::addColorf(const c8* attributeName, SColorf value) {
	stringc tmp = attributeName;
	_hash_map.add(tmp,EAT_COLORF,&value,sizeof(SColorf));
}

void CAttributes::setAttribute(const c8* attributeName, SColorf color) {
	addColorf(attributeName,color);
}

SColorf CAttributes::getAttributeAsColorf(const c8* attributeName) {
	stringc tmp = attributeName;
	SColorf ret;
	_hash_map.getElementbuf(tmp,&ret,sizeof(SColorf));
	return ret;
}

void CAttributes::addVector3d(const c8* attributeName, vector3df value) {
	stringc tmp = attributeName;
	_hash_map.add(tmp,EAT_VECTOR3D,&value,sizeof(vector3df));
}

void CAttributes::setAttribute(const c8* attributeName, vector3df v) {
	addVector3d(attributeName,v);
}

vector3df CAttributes::getAttributeAsVector3d(const c8* attributeName) {
	stringc tmp = attributeName;
	vector3df ret;
	_hash_map.getElementbuf(tmp,&ret,sizeof(vector3df));
	return ret;
}

void CAttributes::addVector2d(const c8* attributeName, vector2df value) {
	stringc tmp = attributeName;
	_hash_map.add(tmp,EAT_VECTOR2D,&value,sizeof(vector2df));
}

void CAttributes::setAttribute(const c8* attributeName, vector2df v) {
	addVector2d(attributeName,v);
}

vector2df CAttributes::getAttributeAsVector2d(const c8* attributeName) {
	stringc tmp = attributeName;
	vector2df ret;
	_hash_map.getElementbuf(tmp,&ret,sizeof(vector2df));
	return ret;
}

void CAttributes::addPosition2d(const c8* attributeName, position2d<s32> value) {
	stringc tmp = attributeName;
	_hash_map.add(tmp,EAT_POSITION2D,&value,sizeof(position2d<s32>));
}

void CAttributes::setAttribute(const c8* attributeName, position2d<s32> v) {
	addPosition2d(attributeName,v);
}

position2d<s32> CAttributes::getAttributeAsPosition2d(const c8* attributeName) {
	stringc tmp = attributeName;
	position2d<s32> pos;
	_hash_map.getElementbuf(tmp,&pos,sizeof(position2d<s32>));
	return pos;
}

void CAttributes::addRect(const c8* attributeName, rectangle<s32> value) {
	stringc tmp = attributeName;
	_hash_map.add(tmp,EAT_RECT,&value,sizeof(rectangle<s32>));
}

void CAttributes::setAttribute(const c8* attributeName, rectangle<s32> v) {
	addRect(attributeName,v);
}

rectangle<s32> CAttributes::getAttributeAsRect(const c8* attributeName) {
	stringc tmp = attributeName;
	rectangle<s32> rect;
	_hash_map.getElementbuf(tmp,&rect,sizeof(rectangle<s32>));
	return rect;
}

void CAttributes::addDimension2d(const c8* attributeName, dimension2d<u32> value) {
	stringc tmp = attributeName;
	_hash_map.add(tmp,EAT_DIMENSION2D,&value,sizeof(dimension2d<u32>));
}

void CAttributes::setAttribute(const c8* attributeName, dimension2d<u32> v) {
	addDimension2d(attributeName,v);
}

dimension2d<u32> CAttributes::getAttributeAsDimension2d(const c8* attributeName) {
	stringc tmp = attributeName;
	dimension2d<u32> ret;
	_hash_map.getElementbuf(tmp,&ret,sizeof(dimension2d<s32>));
	return ret;
}

void CAttributes::addMatrix(const c8* attributeName, const matrix4& v) {
	stringc tmp = attributeName;
	_hash_map.add(tmp,EAT_MATRIX,&v,sizeof(matrix4));
}

void CAttributes::setAttribute(const c8* attributeName, const matrix4& v) {
	addMatrix(attributeName,v);
}

matrix4 CAttributes::getAttributeAsMatrix(const c8* attributeName) {
	stringc tmp = attributeName;
	matrix4 ret;
	_hash_map.getElementbuf(tmp,&ret,sizeof(matrix4));
	return ret;
}