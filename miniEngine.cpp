#include "common.h"
#include "miniEngine.h"


#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>

#include <GL/gl.h>
#include <GL/glext.h>
#include "GL/glx.h"

#include <cstring> 

#if defined(HOST_MACOS)
    #define ESCAPE_KEYCODE 61
#elif defined(HOST_LINUX)
    #define ESCAPE_KEYCODE 9
#else
    #error not defined HOST_LINUX or HOST_MACOS
#endif

std::string vertex_base = R"(
attribute vec3 pos;
attribute vec2 uv;
uniform   mat4 mView;
uniform   mat4 projection;

varying   vec2  v_uv;
void main() 
{
    v_uv = uv;
    gl_Position = vec4(pos,1) * mView * projection;
})";
static std::string _err()
{
    auto res = (GL_NO_ERROR == glGetError()) ? "OK    " : "FAILED";
    return res;
}
static struct 
{
    Display  * display;
    Window     window;
    Window     root;    
    int        screen;
    Visual   * visual; 
    GLXContext context;
    mat        camera;
} data;

uint miniEngine::getWidth()  const {return width;}
uint miniEngine::getHeight() const {return height;}
void miniEngine::setUpdateHandler(const updateHandler &hdl) {onUpdate = hdl;}
void miniEngine::setKeyHandler(const keyHandler &hdl)       {onKey    = hdl;}

miniEngine::~miniEngine()
{
    logger::info("%s close mini engine", timer::timestamp().c_str());
    glXDestroyContext(data.display, data.context);
    XDestroyWindow(data.display, data.window);
}

miniEngine::miniEngine()
{
    logger::info("%s create mini engine", timer::timestamp().c_str());
    int glx11_attr[] = 
    {
        GLX_USE_GL, True,
        GLX_RGBA,   True,
        GLX_DOUBLEBUFFER,
        GLX_BUFFER_SIZE,32,
        GLX_DEPTH_SIZE, 24,
        None
    };
    XWindowAttributes    x11_attr;
    XSetWindowAttributes set_attr; 
   
    data.display = XOpenDisplay(NULL);
    if (!data.display) 
       logger::fatal("can't open X11 display");
    data.root   = XDefaultRootWindow(data.display);
    data.screen = XDefaultScreen(data.display);
    data.visual = XDefaultVisual(data.display, data.screen);
    XGetWindowAttributes(data.display,DefaultRootWindow(data.display),&x11_attr);
    width = x11_attr.width;
    height = x11_attr.height;
    data.window = XCreateSimpleWindow(data.display,data.root,0,0,width, height,0,0,0);
    auto visual = glXChooseVisual(data.display, data.screen, glx11_attr);
    if(!visual)
       logger::fatal("[GLX] unable to find visual");
    data.context = glXCreateContext(data.display, visual, NULL, True);
    if(!data.context)
       logger::fatal("[GLX] unable to create window context");
    glXMakeCurrent(data.display, data.window, data.context);

    std::memset(&set_attr,0,sizeof(set_attr));
    set_attr.event_mask = StructureNotifyMask|ButtonPressMask|ButtonReleaseMask|Button1MotionMask|KeyPressMask;
    set_attr.background_pixel   = 0xFFFF0000;
    XWithdrawWindow(data.display,data.window, data.screen);  
    XChangeWindowAttributes(data.display,data.window,CWBackPixel|CWOverrideRedirect|CWSaveUnder|CWEventMask|CWBorderPixel, &set_attr);
    XResizeWindow(data.display,data.window,width,height);
    XRaiseWindow(data.display, data.window);
    XMapWindow(data.display,data.window);
    XMoveWindow(data.display,data.window,0,0);
    XFlush(data.display);

    glViewport(0, 0, static_cast<GLsizei>(width), static_cast<GLsizei>(height));
    glEnable(GL_BLEND);
    glDepthFunc(GL_LEQUAL); 
    glEnable(GL_DEPTH_TEST);
    glCullFace(GL_BACK);    
    glEnable(GL_CULL_FACE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    data.camera = mat::perspective(45,(float)width/(float)height, 0.1, 100);
    logger::info("create mini engine ... %s",_err().c_str());
}

bool miniEngine::update()
{
    XEvent evt;
    for (int i = 0; i < XPending(data.display); i++)
    { 
        XNextEvent(data.display, &evt);
        switch (evt.type)
        {
            case KeyPress:
            {
                auto event = reinterpret_cast<XKeyEvent *>(&evt);
                if(event->keycode == ESCAPE_KEYCODE) return false;
                if(onKey) onKey(event->keycode);
                break;
            }
            default:
                break;
        }
    }
    auto currTime = timer::get();
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    if(onUpdate) onUpdate(currTime-deltaTime);
    glXSwapBuffers(data.display,data.window);
    deltaTime = currTime;
    return true;
}

void miniEngine::getX11Info()
{
    logger::info("X11 width : %d", width);
    logger::info("X11 height: %d", height);
}

int getInt(const GLenum &e) {GLint val = 0; glGetIntegerv(e, &val); return val;}


#define GET_GLX(e) logger::info("[OK    ] %s : %s",#e, glXGetClientString(data.display,e)) 
#define GET_INT(e) logger::info("[%s] %s : %d",_err().c_str(), #e, getInt(e)); 
#define GET_STR(e) logger::info("[%s] %s : %s",_err().c_str(), #e, glGetString(e)); 

void miniEngine::getOGLInfo()
{
    
    auto ogl_ext = std::string((const char *)glGetString(GL_EXTENSIONS));
    auto glx_ext = std::string(glXGetClientString(data.display,GLX_EXTENSIONS));
    replace<std::string>(ogl_ext, " ","\n    "); ogl_ext = "    " + ogl_ext;
    replace<std::string>(glx_ext, " ","\n    "); glx_ext = "    " + glx_ext;
    GET_GLX(GLX_VENDOR); 
    GET_GLX(GLX_VERSION);
    GET_STR(GL_VENDOR); 
    GET_STR(GL_VERSION); 
    GET_STR(GL_RENDERER);  
    GET_STR(GL_SHADING_LANGUAGE_VERSION); 
    GET_INT(GL_SHADER_COMPILER);
    GET_INT(GL_ALPHA_BITS);
    GET_INT(GL_DEPTH_BITS);
    GET_INT(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS);
    GET_INT(GL_MAX_TEXTURE_BUFFER_SIZE);
    GET_INT(GL_MAX_TEXTURE_IMAGE_UNITS);
    GET_INT(GL_MAX_TEXTURE_SIZE);
    GET_INT(GL_MAX_CUBE_MAP_TEXTURE_SIZE);
    GET_INT(GL_NUM_COMPRESSED_TEXTURE_FORMATS);
    GET_INT(GL_NUM_SHADER_BINARY_FORMATS);
    GET_INT(GL_PACK_ALIGNMENT);
    GET_INT(GL_SAMPLE_BUFFERS);
    GET_INT(GL_MAX_RENDERBUFFER_SIZE);
    GET_INT(GL_MAX_VARYING_VECTORS); 
    GET_INT(GL_MAX_VERTEX_ATTRIBS);
    GET_INT(GL_MAX_VERTEX_UNIFORM_VECTORS);
    GET_INT(GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS);
    GET_INT(GL_MAX_VIEWPORT_DIMS);
    GET_INT(GL_MINOR_VERSION);
    GET_INT(GL_MAX_DRAW_BUFFERS);
    GET_INT(GL_MAX_ELEMENTS_INDICES);
    GET_INT(GL_MAX_ELEMENTS_VERTICES);
    GET_INT(GL_MAX_FRAGMENT_UNIFORM_COMPONENTS);
    GET_INT(GL_MAX_GEOMETRY_TEXTURE_IMAGE_UNITS);
    GET_INT(GL_MAX_GEOMETRY_UNIFORM_BLOCKS);
    GET_INT(GL_MAX_SERVER_WAIT_TIMEOUT);
    GET_INT(GL_MAX_VARYING_FLOATS);
    GET_INT(GL_MAX_VIEWPORTS);
    GET_INT(GL_MAX_VARYING_COMPONENTS);
    GET_INT(GL_MAX_RECTANGLE_TEXTURE_SIZE);
    GET_INT(GL_MAX_3D_TEXTURE_SIZE);
    GET_INT(GL_MAX_ARRAY_TEXTURE_LAYERS);
    GET_INT(GL_MAX_COLOR_TEXTURE_SAMPLES);
    GET_INT(GL_MAX_COMBINED_UNIFORM_BLOCKS);
    GET_INT(GL_MAX_COMBINED_VERTEX_UNIFORM_COMPONENTS);
    GET_INT(GL_NUM_PROGRAM_BINARY_FORMATS);
    GET_INT(GL_CONTEXT_FLAGS);
    GET_INT(GL_DOUBLEBUFFER);
    logger::info("\nOpenGL extensions :\n\n%s", ogl_ext.c_str()); 
    logger::info("\nGLX extensions :\n\n%s", glx_ext.c_str()); 
}

static void check(int id)
{
    int compiled = 0;
    glGetShaderiv (static_cast<GLuint>(id), GL_COMPILE_STATUS, &compiled);
    if(!compiled)
    {
        GLint       lenght;
        std::string message;
        glGetShaderiv (static_cast<GLuint>(id), GL_INFO_LOG_LENGTH, &lenght);
        if (lenght > 1)
        {
            message.resize(static_cast<size_t>(lenght));
            glGetShaderInfoLog(id, lenght, NULL, &message[0]);
            glDeleteShader (id);
            logger::fatal("can't compile shader: %s",message.c_str());
        }
    }
}

material::material() {}
material::material(const std::string &fsh) : 
    v_shader(vertex_base),
    f_shader(fsh)
{
    bind();
}
material::~material()
{}

void material::bind()   
{
   
    id = glCreateProgram(); 
    auto vsh = glCreateShader(GL_VERTEX_SHADER);    
    auto fsh = glCreateShader(GL_FRAGMENT_SHADER);  
    if(vsh == 0 || fsh == 0)
        logger::fatal("can't create shader, perhaps GL context not created");
    auto v_src = v_shader.c_str();
    auto f_src = f_shader.c_str();
    glShaderSource(vsh, 1, &v_src, NULL); 
    glShaderSource(fsh, 1, &f_src, NULL); 
    glCompileShader(vsh); check(vsh);
    glCompileShader(fsh); check(fsh);
  
    glAttachShader(id, vsh);
    glAttachShader(id, fsh);
    glLinkProgram(id); 
  
    pos  = glGetAttribLocation(id,"pos");
    uv   = glGetAttribLocation(id,"uv");
    mv   = glGetUniformLocation(id,"mView");
    prj = glGetUniformLocation(id,"projection");
    time = glGetUniformLocation(id,"time");
    logger::info("bind material %d ... %s",id,_err().c_str());
}
void material::unbind() {}
GLuint material::getID() const {return id;}


object::object(const std::vector<float> &v, const std::vector<uint16_t> &ndx, material::cref m) :
        _material(m),
        vertexes(v),
        indecies(ndx) 
{
    _transform*=mat::translate(vec(0,0,-20));
    bind();
}

object::~object() {};
   
void object::bind()     
{
    glGenBuffers(2, id);
    glBindBuffer(GL_ARRAY_BUFFER, id[0]);
    glBufferData(GL_ARRAY_BUFFER, vertexes.size() * sizeof(float) , &vertexes[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,id[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indecies.size() * sizeof(uint16_t), &indecies[0], GL_STATIC_DRAW);
}
void object::unbind() 
{
    glDeleteBuffers(2,id);
}   

void object::render() 
{
    if(!_material) logger::fatal("try render without material. its stupid."); 
    glUseProgram(_material->getID());
    if(_material->mv   !=-1) glUniformMatrix4fv(_material->mv, 1,GL_FALSE,_transform.data);
    if(_material->time !=-1) glUniform1f(_material->time, timer::get());
    if(_material->prj  !=-1) glUniformMatrix4fv(_material->prj, 1,GL_FALSE,data.camera.data);
    glBindBuffer(GL_ARRAY_BUFFER,id[0]);
    glVertexAttribPointer(_material->pos,3,GL_FLOAT,GL_FALSE,sizeof(vertex),reinterpret_cast<const void *>(0));
    glVertexAttribPointer(_material->uv, 2,GL_FLOAT,GL_FALSE,sizeof(vertex),reinterpret_cast<const void *>(sizeof(float)*3));
    glEnableVertexAttribArray(_material->pos);
    glEnableVertexAttribArray(_material->uv);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,id[1]);
    glDrawElements(GL_TRIANGLES, indecies.size(),GL_UNSIGNED_SHORT,0); 
}
void object::rotate(vec::ref r)    {_transform *= mat::rotate(DEG2RAD *r);}
void object::translate(vec::ref t) {_transform *= mat::translate(t);}
void object::scale(vec::ref s)     {_transform *= mat::scale(s);}
void object::position(vec::ref v)  {_transform.position(v);}  
void object::transform(mat::ref t) {_transform = t;}

vec      object::position()  const  {return _transform.position();} 
mat::ref object::transform() const  {return _transform;}

object::ptr object::createSphere(float radius, int slices, material::cref m)
{
    float step = (TWO_PI) / (static_cast<float>(slices));
    int parallels = slices / 2;
    float r = radius / 1.5;
    std::vector<float>    vertexes;
    std::vector<uint16_t> indecies;  
    for (int i = 0; i < parallels + 1; i++)
    {
        for (int j = 0; j < slices + 1; j++)
        {
                vertexes.push_back(r * std::sin(step * static_cast<float>(i)) * std::sin(step * static_cast<float>(j)));
                vertexes.push_back(r * std::cos(step * static_cast<float>(i)));
                vertexes.push_back(r * std::sin(step * static_cast<float>(i)) * std::cos(step * static_cast<float>(j)));    
                vertexes.push_back(static_cast<float>(j) / static_cast<float>(slices)); 
                vertexes.push_back(static_cast<float>(i) / static_cast<float>(parallels));
        }
    }
    for (uint8_t i = 0; i < slices / 2; i++)
    {

        for (uint8_t j = 0; j < slices; j++)
        {
            indecies.push_back((i * (slices + 1) + j));
            indecies.push_back(((i + 1) * (slices + 1) + j));
            indecies.push_back(((i + 1) * (slices + 1) + (j + 1)));
            indecies.push_back((i * (slices + 1) + j));
            indecies.push_back(((i + 1) * (slices + 1) + (j + 1)));
            indecies.push_back((i * (slices + 1) + (j + 1)));
        }
    }
    return std::make_shared<object>(vertexes, indecies, m);
}
