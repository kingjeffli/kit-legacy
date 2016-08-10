#include "Kit/VertexShader.hpp"
#include "Kit/Exception.hpp"

#include <fstream>
#include <string>
#include <sstream>
#include <iostream>

kit::VertexShader::VertexShader()
{
  this->m_glHandle = glCreateShader(GL_VERTEX_SHADER);
  KIT_ASSERT(this->m_glHandle);
}

kit::VertexShader::~VertexShader()
{
  KIT_GL(glDeleteShader(this->m_glHandle));
}

kit::VertexShader::Ptr kit::VertexShader::create()
{
  return std::make_shared<kit::VertexShader>();
}

bool kit::VertexShader::sourceFromFile(const std::string&filename)
{
  std::string line;
  std::string source = "";

  std::ifstream handle(filename);

  if(!handle.is_open())
  {
    KIT_ERR("Could not open file");
    return false;
  }

  while(std::getline(handle,line))
  {
    source.append(line);
    source.append("\n");
  }

  this->m_source = source;

  const GLchar * src = (const GLchar *)this->m_source.c_str();
  KIT_GL(glShaderSource(this->m_glHandle, 1, &src, 0));

  return true;
}

void kit::VertexShader::sourceFromString(const std::string&s)
{
  this->m_source = s;

  const GLchar *source = (const GLchar *)this->m_source.c_str();
  KIT_GL(glShaderSource(this->m_glHandle, 1, &source, 0));
}

void kit::VertexShader::clearSource()
{
  this->m_source = "";

  std::string emptys = "";
  const GLchar *source = (const GLchar *)emptys.c_str();
  KIT_GL(glShaderSource(this->m_glHandle, 1, &source, 0));
}

bool kit::VertexShader::compile()
{
  // Attempt to compile the shader
  KIT_GL(glCompileShader(this->m_glHandle));

  // Retrieve the compilation status
  GLint status;
  KIT_GL(glGetShaderiv(this->m_glHandle, GL_COMPILE_STATUS, &status));

  // If compilation failed, dump source and return false
  if (!status)
  {
    GLint blen = 0;
    GLsizei slen = 0;
    KIT_GL(glGetShaderiv(this->m_glHandle, GL_INFO_LOG_LENGTH , &blen));
    if (blen > 1)
    {
      GLchar* compiler_log = new GLchar[blen];
      KIT_GL(glGetShaderInfoLog(this->m_glHandle, blen, &slen, compiler_log));

      std::stringstream ss;
      ss << "Vertexshader compilation failed: " << compiler_log;
      KIT_ERR(ss.str());
      delete[] compiler_log;
      
      std::cout << "Dumping vertexshader source..." << std::endl;
      std::cout << "------------------------" << std::endl;
      
      std::stringstream sourcestream(this->m_source);
      unsigned int linecount = 1;
      std::string currline;
      while (std::getline(sourcestream, currline))
      {
        std::cout << linecount << ":\t" << currline << std::endl;
        linecount++;
      }

      std::cout << "------------------------" << std::endl;
    }

    return false;
  }

  return true;
}

GLuint kit::VertexShader::getHandle()
{
  return this->m_glHandle;
}
