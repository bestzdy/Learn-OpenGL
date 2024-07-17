//////////////////////////////////////////////////////////////////////////////
//
//  --- LoadShaders.cxx ---
//
//////////////////////////////////////////////////////////////////////////////


#include "Common.h"
bool isFileExists_ifstream(string& name) {
    ifstream f(name.c_str());
    return f.good();
}
bool isShaderFileExists(string& name) {
    bool isvertexist = isFileExists_ifstream(name);
    if (isvertexist != true) {
        name = "..\\common\\" + name;
        isvertexist = isFileExists_ifstream(name);
        if (isvertexist != true)
        {
            name = "..\\" + name;
            isvertexist = isFileExists_ifstream(name);
            if (isvertexist != true)
            {
                cout << name << endl;
                cout << "can not find shaders!!!\nprogram over" << endl;
            }
        }
    }
    return isvertexist;
}
static const GLchar* ReadShader(const string filename)
{
    ifstream fin(filename.c_str(), ios::in);

    if (!fin.good()) {
    #ifdef _DEBUG
        std::cerr << "无法打开文件 '" << filename << "'" << std::endl;
    #endif /* DEBUG */
        return NULL;
    }
    fin.seekg(0, fin.end);
    unsigned long long filesize= fin.tellg();
    fin.seekg(0, fin.beg);
    GLchar* source = new GLchar[filesize + 1];
    memset(source, 0, filesize + 1);
    fin.read(source, filesize);
    fin.close();
    return const_cast<const GLchar*>(source);
}
GLuint
LoadShaders( ShaderInfo* shaders )
{
    if ( shaders == NULL ) { return 0; }

    GLuint program = glCreateProgram();

    ShaderInfo* entry = shaders;
    while ( entry->type != GL_NONE ) {
        GLuint shader = glCreateShader( entry->type );

        entry->shader = shader;

        //const GLchar* source = ReadShader( entry->filename );
        const GLchar* source = ReadShader(string(entry->filename));
        if ( source == NULL ) {
            for ( entry = shaders; entry->type != GL_NONE; ++entry ) {
                glDeleteShader( entry->shader );
                entry->shader = 0;
            }

            return 0;
        }

        glShaderSource( shader, 1, &source, NULL );
        delete [] source;

        glCompileShader( shader );

        GLint compiled;
        glGetShaderiv( shader, GL_COMPILE_STATUS, &compiled );
        if ( !compiled ) {
#ifdef _DEBUG
            GLsizei len;
            glGetShaderiv( shader, GL_INFO_LOG_LENGTH, &len );

            GLchar* log = new GLchar[len+1];
            glGetShaderInfoLog( shader, len, &len, log );
            std::cerr << "Shader compilation failed: " << log << std::endl;
            delete [] log;
#endif /* DEBUG */

            return 0;
        }

        glAttachShader( program, shader );
        
        ++entry;
    }


    glLinkProgram( program );

    GLint linked;
    glGetProgramiv( program, GL_LINK_STATUS, &linked );
    if ( !linked ) {
#ifdef _DEBUG
        GLsizei len;
        glGetProgramiv( program, GL_INFO_LOG_LENGTH, &len );

        GLchar* log = new GLchar[len+1];
        glGetProgramInfoLog( program, len, &len, log );
        std::cerr << "Shader linking failed: " << log << std::endl;
        delete [] log;
#endif /* DEBUG */

        for ( entry = shaders; entry->type != GL_NONE; ++entry ) {
            glDeleteShader( entry->shader );
            entry->shader = 0;
        }
        
        return 0;
    }

    return program;
}
