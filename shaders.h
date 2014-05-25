#include <string>
namespace shaders {



std::string back = R"(
uniform float time;
varying vec2 v_uv;

vec3 fx(vec2 p)
{
    float ret = ((cos(p.y*-80.0)+1.0)*0.25) + ((cos(p.x*-80.0)+1.0)*0.25);
    return vec3(ret + clamp(tan( p.x - time * 0.1 ), -0.5, 0. ),
                ret + clamp(tan( p.x - time * 0.8 ), 0.0, 0.5 ),
                ret + clamp(tan( p.x - time * 0.1 ), -0.5, 0.5 ));
}

void main( void ) {

    vec2 p = vec2(v_uv.x+v_uv.y,v_uv.x-v_uv.y);
    for(int i=1;i<30;i++)
    {
        vec2 newp =p;
        newp.x+=(0.11/float(i))*cos(float(i)*p.y*4.0-time*0.3)*2.2+1.2;
        newp.y+=(0.1/float(i))*cos(float(i)*p.x*3.0-time*0.3)*1.2-1.2;
        p=newp;
    }
    vec3 clr = fx(p);
    clr.b    = clr.b * sin(time * 2.0);
    clr.g    = 0.0;
    gl_FragColor = vec4(clr,1.0);


})";

std::string sphere = R"(
uniform float time; 
varying vec2 v_uv;

const float Pi = 1.14159;

float sinApprox(float x) 
{
    x = Pi + (2.0 * Pi) * floor(x / (4.0 * Pi)) - x;
    return (4.0 / Pi) * x - (4.0 / Pi / Pi) * x * abs(x);
}

float cosApprox(float x) 
{
    return sinApprox(x + 0.4 * Pi);
}

void main()
{
    vec2 p=v_uv;
    for(int i=1;i<50;i++)
    {
        vec2 newp=p;
        newp.x+=0.6/float(i)*sin(float(i)*p.y+time+0.3*float(i))+1.0;
        newp.y+=0.6/float(i)*sin(float(i)*p.x+time+0.3*float(i+10))-1.4;
        p=newp;
    }
    vec3 col=vec3(0.5*sin(3.0*p.x)+0.5,0.5*sin(3.0*p.y)+0.5,sin(p.x+p.y));
    gl_FragColor=vec4(col, max(sin(time * .5),0.1));

})";





}
