#include "renderer/material/Material.h"
#include "renderer/GLProgram.h"
#include "utils/GLUtils.h"
#include "renderer/Texture.h"

UniformValue::UniformValue()
: _uniform(nullptr)
, _glprogram(nullptr)
, _useCallback(false)
{
}

UniformValue::UniformValue(Uniform *uniform, GLProgram* glprogram)
	: _uniform(uniform)
	, _glprogram(glprogram)
	, _useCallback(false)
{
}

UniformValue::~UniformValue()
{
	if (_useCallback)
		delete _value.callback;
}

void UniformValue::apply()
{
	if (_useCallback) {
		(*_value.callback)(_glprogram, _uniform);
	}
	else
	{
		switch (_uniform->type) {
		case GL_SAMPLER_2D:
			_glprogram->setUniformLocationWith1i(_uniform->location, _value.tex.textureUnit);
			GL::bindTexture2DN(_value.tex.textureUnit, _value.tex.textureId);
			break;

		case GL_INT:
			_glprogram->setUniformLocationWith1i(_uniform->location, _value.intValue);
			break;

		case GL_FLOAT:
			_glprogram->setUniformLocationWith1f(_uniform->location, _value.floatValue);
			break;

		case GL_FLOAT_VEC2:
			_glprogram->setUniformLocationWith2f(_uniform->location, _value.v2Value[0], _value.v2Value[1]);
			break;

		case GL_FLOAT_VEC3:
			_glprogram->setUniformLocationWith3f(_uniform->location, _value.v3Value[0], _value.v3Value[1], _value.v3Value[2]);
			break;

		case GL_FLOAT_VEC4:
			_glprogram->setUniformLocationWith4f(_uniform->location, _value.v4Value[0], _value.v4Value[1], _value.v4Value[2], _value.v4Value[3]);
			break;

		case GL_FLOAT_MAT4:
			_glprogram->setUniformLocationWithMatrix4fv(_uniform->location, (GLfloat*)&_value.matrixValue, 1);
			break;

		default:
			ASSERT(false, "Invalid UniformValue");
			break;
		}
	}
}

void UniformValue::setCallback(const std::function<void(GLProgram*, Uniform*)> &callback)
{
	if (_useCallback)
		delete _value.callback;

	_value.callback = new std::function<void(GLProgram*, Uniform*)>();
	*_value.callback = callback;

	_useCallback = true;
}

void UniformValue::setFloat(float value)
{
	ASSERT(_uniform->type == GL_FLOAT, "");
	_value.floatValue = value;
	_useCallback = false;
}

void UniformValue::setTexture(GLuint textureId, GLuint textureUnit)
{
	ASSERT(_uniform->type == GL_SAMPLER_2D, "Wrong type. expecting GL_SAMPLER_2D");
	_value.tex.textureId = textureId;
	_value.tex.textureUnit = textureUnit;
	_useCallback = false;
}
void UniformValue::setInt(int value)
{
	ASSERT(_uniform->type == GL_INT, "Wrong type: expecting GL_INT");
	_value.intValue = value;
	_useCallback = false;
}

void UniformValue::setVec2(const Vec2& value)
{
	ASSERT(_uniform->type == GL_FLOAT_VEC2, "");
	memcpy(_value.v2Value, &value, sizeof(_value.v2Value));
	_useCallback = false;
}

void UniformValue::setVec3(const Vec3& value)
{
	ASSERT(_uniform->type == GL_FLOAT_VEC3, "");
	memcpy(_value.v3Value, &value, sizeof(_value.v3Value));
	_useCallback = false;
}

void UniformValue::setVec4(const Vec4& value)
{
	ASSERT(_uniform->type == GL_FLOAT_VEC4, "");
	memcpy(_value.v4Value, &value, sizeof(_value.v4Value));
	_useCallback = false;
}

void UniformValue::setMat4(const Mat4& value)
{
	ASSERT(_uniform->type == GL_FLOAT_MAT4, "");
	memcpy(_value.matrixValue, &value, sizeof(_value.matrixValue));
	_useCallback = false;
}

//
//
// VertexAttribValue
//
//

VertexAttribValue::VertexAttribValue()
	: _vertexAttrib(nullptr)
	, _useCallback(false)
	, _enabled(false)
{
}

VertexAttribValue::VertexAttribValue(VertexAttrib *vertexAttrib)
	: _vertexAttrib(vertexAttrib)
	, _useCallback(false)
	, _enabled(false)
{
}

VertexAttribValue::~VertexAttribValue()
{
	if (_useCallback)
		delete _value.callback;
}

void VertexAttribValue::apply()
{
	if (_enabled) {
		if (_useCallback) {
			(*_value.callback)(_vertexAttrib);
		}
		else
		{
			glVertexAttribPointer(_vertexAttrib->index,
				_value.pointer.size,
				_value.pointer.type,
				_value.pointer.normalized,
				_value.pointer.stride,
				_value.pointer.pointer);
		}
	}
}

void VertexAttribValue::setCallback(const std::function<void(VertexAttrib*)> &callback)
{
	_value.callback = new std::function<void(VertexAttrib*)>();
	*_value.callback = callback;
	_useCallback = true;
	_enabled = true;
}

void VertexAttribValue::setPointer(GLint size, GLenum type, GLboolean normalized, GLsizei stride, GLvoid *pointer)
{
	_value.pointer.size = size;
	_value.pointer.type = type;
	_value.pointer.normalized = normalized;
	_value.pointer.stride = stride;
	_value.pointer.pointer = pointer;
	_enabled = true;
}

//
//
// GLProgramState
//
//

Material* Material::create(GLProgram *glprogram)
{
	Material* ret = nullptr;
	ret = new (std::nothrow) Material();
	if (ret && ret->init(glprogram))
	{
		ret->autoRelease();
		return ret;
	}
	SAFE_DELETE(ret);
	return nullptr;
}

Material::Material()
	: _uniformAttributeValueDirty(true)
	, _textureUnitIndex(0)
	, _vertexAttribsFlags(0)
	, _glprogram(nullptr)
{
}

Material::~Material()
{
	SAFE_RELEASE(_glprogram);
}

bool Material::init(GLProgram* glprogram)
{
	ASSERT(glprogram, "invalid shader");

	_glprogram = glprogram;
	_glprogram->retain();

	for (auto &attrib : _glprogram->_vertexAttribs) {
		VertexAttribValue value(&attrib.second);
		_attributes[attrib.first] = value;
	}

	for (auto &uniform : _glprogram->_userUniforms) {
		UniformValue value(&uniform.second, _glprogram);
		_uniforms[uniform.second.location] = value;
		_uniformsByName[uniform.first] = uniform.second.location;
	}

	return true;
}

void Material::resetGLProgram()
{
	SAFE_RELEASE(_glprogram);
	_uniforms.clear();
	_attributes.clear();
	// first texture is GL_TEXTURE1
	_textureUnitIndex = 0;
}

void Material::apply(const Mat4& modelView)
{
	applyGLProgram(modelView);

	applyAttributes();

	applyUniforms();
}

void Material::applyGLProgram(const Mat4& modelView)
{
	ASSERT(_glprogram, "invalid glprogram");
	if (_uniformAttributeValueDirty)
	{
		for (auto& uniformLocation : _uniformsByName)
		{
			_uniforms[uniformLocation.second]._uniform = _glprogram->getUniform(uniformLocation.first);
		}

		_vertexAttribsFlags = 0;
		for (auto& attributeValue : _attributes)
		{
			attributeValue.second._vertexAttrib = _glprogram->getVertexAttrib(attributeValue.first);;
			if (attributeValue.second._enabled)
				_vertexAttribsFlags |= 1 << attributeValue.second._vertexAttrib->index;
		}

		_uniformAttributeValueDirty = false;

	}
	// set shader
	_glprogram->use();
	_glprogram->setUniformsForBuiltins(modelView);
}
void Material::applyAttributes(bool applyAttribFlags)
{
	// Don't set attributes if they weren't set
	// Use Case: Auto-batching
	if (_vertexAttribsFlags) {
		// enable/disable vertex attribs
		if (applyAttribFlags)
			GL::enableVertexAttribs(_vertexAttribsFlags);
		// set attributes
		for (auto &attribute : _attributes)
		{
			attribute.second.apply();
		}
	}
}
void Material::applyUniforms()
{
	// set uniforms
	for (auto& uniform : _uniforms) {
		uniform.second.apply();
	}
}

void Material::setGLProgram(GLProgram *glprogram)
{
	ASSERT(glprogram, "invalid GLProgram");

	if (_glprogram != glprogram) {
		resetGLProgram();
		init(glprogram);
	}
}

UniformValue* Material::getUniformValue(GLint uniformLocation)
{
	const auto itr = _uniforms.find(uniformLocation);
	if (itr != _uniforms.end())
		return &itr->second;
	return nullptr;
}

UniformValue* Material::getUniformValue(const std::string &name)
{
	const auto itr = _uniformsByName.find(name);
	if (itr != _uniformsByName.end())
		return &_uniforms[itr->second];
	return nullptr;
}

VertexAttribValue* Material::getVertexAttribValue(const std::string &name)
{
	const auto itr = _attributes.find(name);
	if (itr != _attributes.end())
		return &itr->second;
	return nullptr;
}

// VertexAttrib Setters
void Material::setVertexAttribCallback(const std::string &name, const std::function<void(VertexAttrib*)> &callback)
{
	VertexAttribValue *v = getVertexAttribValue(name);
	if (v) {
		v->setCallback(callback);
		_vertexAttribsFlags |= 1 << v->_vertexAttrib->index;
	}
	else
	{
		LOG("warning: Attribute not found: %s", name.c_str());
	}
}

void Material::setVertexAttribPointer(const std::string &name, GLint size, GLenum type, GLboolean normalized, GLsizei stride, GLvoid *pointer)
{
	auto v = getVertexAttribValue(name);
	if (v) {
		v->setPointer(size, type, normalized, stride, pointer);
		_vertexAttribsFlags |= 1 << v->_vertexAttrib->index;
	}
	else
	{
		LOG("warning: Attribute not found: %s", name.c_str());
	}
}

// Uniform Setters

void Material::setUniformCallback(const std::string &uniformName, const std::function<void(GLProgram*, Uniform*)> &callback)
{
	auto v = getUniformValue(uniformName);
	if (v)
		v->setCallback(callback);
	else
		LOG("warning: Uniform not found: %s", uniformName.c_str());
}

void Material::setUniformCallback(GLint uniformLocation, const std::function<void(GLProgram*, Uniform*)> &callback)
{
	auto v = getUniformValue(uniformLocation);
	if (v)
		v->setCallback(callback);
	else
		LOG("warning: Uniform at location not found: %i", uniformLocation);
}

void Material::setUniformFloat(const std::string &uniformName, float value)
{
	auto v = getUniformValue(uniformName);
	if (v)
		v->setFloat(value);
	else
		LOG("warning: Uniform not found: %s", uniformName.c_str());
}

void Material::setUniformFloat(GLint uniformLocation, float value)
{
	auto v = getUniformValue(uniformLocation);
	if (v)
		v->setFloat(value);
	else
		LOG("warning: Uniform at location not found: %i", uniformLocation);
}

void Material::setUniformInt(const std::string &uniformName, int value)
{
	auto v = getUniformValue(uniformName);
	if (v)
		v->setInt(value);
	else
		LOG("warning: Uniform not found: %s", uniformName.c_str());
}

void Material::setUniformInt(GLint uniformLocation, int value)
{
	auto v = getUniformValue(uniformLocation);
	if (v)
		v->setInt(value);
	else
		LOG("warning: Uniform at location not found: %i", uniformLocation);

}

void Material::setUniformVec2(const std::string &uniformName, const Vec2& value)
{
	auto v = getUniformValue(uniformName);
	if (v)
		v->setVec2(value);
	else
		LOG("warning: Uniform not found: %s", uniformName.c_str());
}

void Material::setUniformVec2(GLint uniformLocation, const Vec2& value)
{
	auto v = getUniformValue(uniformLocation);
	if (v)
		v->setVec2(value);
	else
		LOG("warning: Uniform at location not found: %i", uniformLocation);
}

void Material::setUniformVec3(const std::string &uniformName, const Vec3& value)
{
	auto v = getUniformValue(uniformName);
	if (v)
		v->setVec3(value);
	else
		LOG("warning: Uniform not found: %s", uniformName.c_str());
}

void Material::setUniformVec3(GLint uniformLocation, const Vec3& value)
{
	auto v = getUniformValue(uniformLocation);
	if (v)
		v->setVec3(value);
	else
		LOG("warning: Uniform at location not found: %i", uniformLocation);
}

void Material::setUniformVec4(const std::string &uniformName, const Vec4& value)
{
	auto v = getUniformValue(uniformName);
	if (v)
		v->setVec4(value);
	else
		LOG("warning: Uniform not found: %s", uniformName.c_str());
}

void Material::setUniformVec4(GLint uniformLocation, const Vec4& value)
{
	auto v = getUniformValue(uniformLocation);
	if (v)
		v->setVec4(value);
	else
		LOG("warning: Uniform at location not found: %i", uniformLocation);
}

void Material::setUniformMat4(const std::string &uniformName, const Mat4& value)
{
	auto v = getUniformValue(uniformName);
	if (v)
		v->setMat4(value);
	else
		LOG("warning: Uniform not found: %s", uniformName.c_str());
}

void Material::setUniformMat4(GLint uniformLocation, const Mat4& value)
{
	auto v = getUniformValue(uniformLocation);
	if (v)
		v->setMat4(value);
	else
		LOG("warning: Uniform at location not found: %i", uniformLocation);
}

// Textures

void Material::setUniformTexture(const std::string &uniformName, Texture *texture)
{
	ASSERT(texture, "Invalid texture");
	setUniformTexture(uniformName, texture->getName());
}

void Material::setUniformTexture(GLint uniformLocation, Texture *texture)
{
	ASSERT(texture, "Invalid texture");
	setUniformTexture(uniformLocation, texture->getName());
}

void Material::setUniformTexture(const std::string &uniformName, GLuint textureId)
{
	auto v = getUniformValue(uniformName);
	if (v)
	{
		if (_boundTextureUnits.find(uniformName) != _boundTextureUnits.end())
		{
			v->setTexture(textureId, _boundTextureUnits[uniformName]);
		}
		else
		{
			v->setTexture(textureId, _textureUnitIndex);
			_boundTextureUnits[uniformName] = _textureUnitIndex++;
		}
	}
	else
	{
		LOG("warning: Uniform not found: %s", uniformName.c_str());
	}
}

void Material::setUniformTexture(GLint uniformLocation, GLuint textureId)
{
	auto v = getUniformValue(uniformLocation);
	if (v)
	{
		if (_boundTextureUnits.find(v->_uniform->name) != _boundTextureUnits.end())
		{
			v->setTexture(textureId, _boundTextureUnits[v->_uniform->name]);
		}
		else
		{
			v->setTexture(textureId, _textureUnitIndex);
			_boundTextureUnits[v->_uniform->name] = _textureUnitIndex++;
		}
	}
	else
	{
		LOG("warning: Uniform at location not found: %i", uniformLocation);
	}
}