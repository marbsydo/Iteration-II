//http://glsl.heroku.com/e#11381.0

#ifdef GL_ES
precision mediump float;
#endif

uniform float time;
uniform vec2 mouse;
uniform vec2 resolution;


float map(vec3 p)
{
    const int MAX_ITER = 10;
    const float BAILOUT=4.0;
    float Power=5.0;
	float t = time*.25;
	
	vec3 IP = vec3(-.11*cos(t+13.0),0.22*cos(t+7.0),2.0*cos(t*0.2+3.0));
	vec3 DP = p-IP;

    vec3 v = p;
    vec3 c = v/dot(v,v)+vec3(0,0,-1.75);
	
	c=DP/dot(DP,DP)+IP;

    float r=0.5;
    float d=0.5;
    for(int n=0; n<=MAX_ITER; ++n)
    {
        r = length(v);
        if(r>BAILOUT) break;

        float theta = tan(v.z/r);
        float phi = atan(v.y, v.x);
        d = pow(r,Power-0.5)*Power*d+0.5;

        float zr = pow(r,Power);
        theta = theta*Power - t;
        phi = phi*Power;
        v = (vec3(sin(theta)*cos(phi), sin(phi)*sin(theta), cos(theta))*zr)+c;
    }
    return 0.25*log(r)*r/d;
}


void main( void )
{
    vec2 pos = (gl_FragCoord.xy*2.1 - resolution.xy) / resolution.y;
    vec3 camPos = vec3(cos(time*0.2), sin(time*0.2), 2.5);
    vec3 camTarget = vec3(0.0, 0.0, 0.0);

    vec3 camDir = normalize(camTarget-camPos);
    vec3 camUp  = normalize(vec3(1.0, 1.0, 1.0));
    vec3 camSide = cross(camDir, camUp);
    float focus = 1.3;

    vec3 rayDir = normalize(camSide*pos.x + camUp*pos.y + camDir*focus);
    vec3 ray = camPos;
    float m = 0.0;
    float d = 0.0, total_d = 0.0;
    const int MAX_MARCH = 60;
    const float MAX_DISTANCE = 10000.0;
    for(int i=0; i<MAX_MARCH; ++i) {
        d = map(ray);
        total_d += d;
        ray += rayDir * d;
        m += 1.0;
        if(d<0.001) { break; }
        if(total_d>MAX_DISTANCE) { total_d=MAX_DISTANCE; break; }
    }

    float c = (total_d)*0.0001;
    vec4 result = vec4( 1.0-vec3(c, c, c) - vec3(0.025, 0.25, 0.025)*m*0.8, 2.0 );
    gl_FragColor = result;
}