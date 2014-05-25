#ifndef _SYNQERA_MINI_ENGINE_
#define _SYNQERA_MINI_ENGINE_

#include "common.h"
#include "miniMath.h"

class miniEngine
{
    using updateHandler = std::function<void(float)>;
    using mouseHandler  = std::function<void(uint,uint)>; 
    using keyHandler    = std::function<void(uint)>; 
    
public:
    miniEngine();
    ~miniEngine();

    bool update();
    
    void getX11Info();
    void getOGLInfo();
    
    void setUpdateHandler(const updateHandler &);
    void setKeyHandler(const keyHandler &);
    uint getWidth()  const;
    uint getHeight() const;
    
private:
    uint  width;
    uint  height; 
    float deltaTime = 0;     
    updateHandler onUpdate {};
    keyHandler    onKey    {};  
};

class material 
{
public: 
    using ptr  = std::shared_ptr<material>;
    using cref = const ptr &; 
    int pos,uv,time,mv,prj; 
    material();
    material(const std::string &);
    virtual ~material();
    uint getID() const;
private:
    void bind();   
    void unbind();
    uint id;
    std::string v_shader;
    std::string f_shader;
};

class object 
{
    using ptr  = std::shared_ptr<object>;
    using cref = const ptr &; 
    typedef float vertex[5]; 
public: 
   
    object(const std::vector<float> &, const std::vector<uint16_t> &, material::cref);
    virtual ~object();
   
    void render(); 
    void rotate(vec::ref);
    void translate(vec::ref);
    void scale(vec::ref);
    void position(vec::ref); 
    void transform(mat::ref); 

    vec      position()  const;
    mat::ref transform() const;
    static ptr createSphere(float radius, int slices,material::cref);
  
protected:
    material::ptr _material;
    mat           _transform {}; 

private:
    uint                  id[2];
    std::vector<float>    vertexes;
    std::vector<uint16_t> indecies;        
    void bind();     
    void unbind(); 
    
};  
#endif