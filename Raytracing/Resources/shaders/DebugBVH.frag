#version 330 core

#define PI              3.1415926
#define INF             114514.0
#define SIZE_TRIANGLE   6
#define SIZE_BVHNODE    4
#define SIZE_MATERIAL   6
#define INF             114514.0
#define MAX_DEPTH       10
#define MAX_SSP         1

float WIDTH = 1920;
float HEIGHT = 1080;


//uniform vec3 lightPos;
in vec3 fragNormal;
out vec4 FragColor;
in vec2 TexCoords;
in vec3 FragPos;

//uniform float width;
//uniform float height;
uniform mat4 view_inverse;
uniform mat4 projection_inverse;
float ao = 1.0f;

uniform samplerCube irradianceMap;


uniform uint frameCounter;
uint frameCounter1 = uint(0);



// ndc
vec3 pix = vec3((gl_FragCoord.xy /vec2(WIDTH,HEIGHT) ) * 2 - vec2(1),gl_FragCoord.z*2-1);



struct Material {
    vec3 emissive;
    vec3 baseColor ;
    float subsurface;
    float metallic ;
    float specular;
    float specularTint;
    float roughness ;
    float anisotropic ;
    float sheen;
    float sheenTint ;
    float clearcoat;
    float clearcoatGloss ;
    float IOR;
    float transmission ;
};
uniform Material material; 

uniform samplerBuffer triangles;
uniform samplerBuffer nodes;
uniform samplerBuffer materials;

uniform int ntriangles;

uniform vec3 eye_pos;



uniform mat4 inverse_projection;
uniform mat4 inverse_view;


struct Triangle {
    vec3 p1, p2, p3;    // ????????
    vec3 n1, n2, n3;    // ??????
};

struct BVHNode {
    int left;           // ??????
    int right;          // ??????
    int n;              // ?????????????
    int index;          // ??????????
    vec3 AA, BB;        // ?????
};

struct HitResult {
    bool isHit;             // ???????
    bool isInside;          // ???????????
    float distance;         // ????????
    vec3 hitPoint;          // ???????快?
    vec3 normal;            // ???快???
    vec3 viewDir;           // ???我??????????
    Material material;      // ???快????????
};

struct Ray {
    vec3 startPoint;
    vec3 direction;
};



HitResult hitTriangle(Triangle triangle, Ray ray) {
    HitResult res;
    res.distance = INF;
    res.isHit = false;
    res.isInside = false;

    vec3 p1 = triangle.p1;
    vec3 p2 = triangle.p2;
    vec3 p3 = triangle.p3;

    //p1 = vec3(-1,1,0);
    //p2 = vec3(-1,-1,0);
    //p3 = vec3(1,-1,0);

    vec3 S = ray.startPoint;   
    vec3 d = ray.direction;     
    vec3 N = normalize(cross(p2-p1, p3-p1));   

    
    if (dot(N, d) > 0.0f) {
        N = -N;
        res.isInside = true;
    }

    
    if (abs(dot(N, d)) < 0.00001f) return res;

    
    float t = (dot(N, p1) - dot(S, N)) / dot(d, N);
    if (t < 0.0005f) return res;    // ?????????????????

    
    vec3 P = S + d * t;

    
    vec3 c1 = cross(p2 - p1, P - p1);
    vec3 c2 = cross(p3 - p2, P - p2);
    vec3 c3 = cross(p1 - p3, P - p3);
    bool r1 = (dot(c1, N) > 0 && dot(c2, N) > 0 && dot(c3, N) > 0);
    bool r2 = (dot(c1, N) < 0 && dot(c2, N) < 0 && dot(c3, N) < 0);

    
    if (r1 || r2) {
        res.isHit = true;
        res.hitPoint = P;
        res.distance = t;
        res.normal = N;
        res.viewDir = d;
        
        float alpha = (-(P.x-p2.x)*(p3.y-p2.y) + (P.y-p2.y)*(p3.x-p2.x)) / (-(p1.x-p2.x-0.00005)*(p3.y-p2.y+0.00005) + (p1.y-p2.y+0.00005)*(p3.x-p2.x+0.00005));
        float beta  = (-(P.x-p3.x)*(p1.y-p3.y) + (P.y-p3.y)*(p1.x-p3.x)) / (-(p2.x-p3.x-0.00005)*(p1.y-p3.y+0.00005) + (p2.y-p3.y+0.00005)*(p1.x-p3.x+0.00005));
        float gama  = 1.0 - alpha - beta;
        vec3 Nsmooth = alpha * triangle.n1 + beta * triangle.n2 + gama * triangle.n3;
        Nsmooth = normalize(Nsmooth);
        res.normal = (res.isInside) ? (-Nsmooth) : (Nsmooth);
    }

    return res;

}

float hitAABB(Ray r, vec3 AA, vec3 BB) {
    vec3 invdir = 1.0 / r.direction;

    vec3 f = (BB - r.startPoint) * invdir;
    vec3 n = (AA - r.startPoint) * invdir;

    vec3 tmax = max(f, n);
    vec3 tmin = min(f, n);

    float t1 = min(tmax.x, min(tmax.y, tmax.z));
    float t0 = max(tmin.x, max(tmin.y, tmin.z));

    return (t1 >= t0) ? ((t0 > 0.0) ? (t0) : (t1)) : (-1.0);
}


BVHNode getBVHNode(int i) {
    BVHNode node;

    
    int offset = i * SIZE_BVHNODE;
    ivec3 childs = ivec3(texelFetch(nodes, offset + 0).xyz);
    ivec3 leafInfo = ivec3(texelFetch(nodes, offset + 1).xyz);
    node.left = int(childs.x);
    node.right = int(childs.y);
    node.n = int(leafInfo.x);
    node.index = int(leafInfo.y);

    
    node.AA = texelFetch(nodes, offset + 2).xyz;
    node.BB = texelFetch(nodes, offset + 3).xyz;

    return node;
}

Triangle getTriangle(int i) {
    int offset = i * SIZE_TRIANGLE;
    Triangle t;

    
    t.p1 = texelFetch(triangles, offset + 0).xyz;
    t.p2 = texelFetch(triangles, offset + 1).xyz;
    t.p3 = texelFetch(triangles, offset + 2).xyz;
    
    t.n1 = texelFetch(triangles, offset + 3).xyz;
    t.n2 = texelFetch(triangles, offset + 4).xyz;
    t.n3 = texelFetch(triangles, offset + 5).xyz;

    return t;
}


Material getMaterial(int i) {

    Material m;

    int offset = i * SIZE_MATERIAL;
    vec3 param1 = texelFetch(materials, offset + 2).xyz;
    vec3 param2 = texelFetch(materials, offset + 3).xyz;
    vec3 param3 = texelFetch(materials, offset + 4).xyz;
    vec3 param4 = texelFetch(materials, offset + 5).xyz;

    m.emissive = texelFetch(materials, offset + 0).xyz;
    m.baseColor = texelFetch(materials, offset + 1).xyz;
    //m.baseColor = vec3(0.8,0,0);
    m.subsurface = param1.x;
    m.metallic = param1.y;
    m.specular = param1.z;
    m.specularTint = param2.x;
    m.roughness = param2.y;
    m.anisotropic = param2.z;
    m.sheen = param3.x;
    m.sheenTint = param3.y;
    m.clearcoat = param3.z;
    m.clearcoatGloss = param4.x;
    m.IOR = param4.y;
    m.transmission = param4.z;

    return m;
}


HitResult hitArray(Ray ray, int l, int r) {
    HitResult res;
    res.isHit = false;
    res.distance = INF;
    for(int i=l; i<=r; i++) {
        Triangle triangle = getTriangle(i);
        HitResult r = hitTriangle(triangle, ray);
        if(r.isHit && r.distance<res.distance) {
            res = r;
            //res.material = material;
            res.material = getMaterial(i);
        }
    }
    return res;
}


HitResult hitBVH(Ray ray) {
    HitResult res;
    res.isHit = false;
    res.distance = INF;

    // ?
    //int stack[256];
    int stack[512];
    int sp = 0;

    stack[sp++] = 0;
    while(sp>0) {
        int top = stack[--sp];
        BVHNode node = getBVHNode(top);

        
        if(node.n>0) {
            int L = node.index;
            int R = node.index + node.n - 1;
            HitResult r = hitArray(ray, L, R);
            if(r.isHit && r.distance<res.distance) res = r;
            continue;
        }

        
        float d1 = INF; 
        float d2 = INF; 
        if(node.left>0) {
            BVHNode leftNode = getBVHNode(node.left);
            d1 = hitAABB(ray, leftNode.AA, leftNode.BB);
        }
        if(node.right>0) {
            BVHNode rightNode = getBVHNode(node.right);
            d2 = hitAABB(ray, rightNode.AA, rightNode.BB);
        }

        
        if(d1>0 && d2>0) {
            if(d1<d2) { // d1<d2, ?????
                        stack[sp++] = node.right;
                        stack[sp++] = node.left;
            } else {    // d2<d1, ?????
                        stack[sp++] = node.left;
                        stack[sp++] = node.right;
            }
        } else if(d1>0) {   // ?????????
                            stack[sp++] = node.left;
        } else if(d2>0) {   // ?????????
                            stack[sp++] = node.right;
        }
    }

    return res;
}

HitResult castRay(Ray ray,int depth)
{
    HitResult res;
    res.isHit = false;


    if(depth > MAX_DEPTH )
    {
        return res;
    }
    vec3 colorRes = vec3(0);
    res = hitBVH(ray);
    
    if(res.isHit)
    {
        res.material.emissive *= pow(0.2,float(depth));
        return res;
    }else{
        res.material.emissive = vec3(0,0,0);
        return res;
    }
}





vec4 getWorldPos(vec3 target)
{
    // ndc
    vec4 ndcPosition = vec4(target ,1.0);
    // clip
    vec4 clipPosition;
    clipPosition.xyz = ndcPosition.xyz / gl_FragCoord.w;
    clipPosition.w = 1.0/gl_FragCoord.w;
    // world
    vec4 worldPos = view_inverse*projection_inverse*clipPosition;
    worldPos.xyz /= worldPos.w;
    return worldPos;
}


void main()
{
    Ray ray;

    vec4 worldPos = getWorldPos(pix);
    ray.direction = normalize(worldPos.xyz - eye_pos);
    ray.startPoint = eye_pos;

    vec3 result = vec3(0);
    
    HitResult res = hitArray(ray,0,ntriangles-1);
    
    if(res.ishit)FragColor = vec4(0.6,0.3,0.3,1.0);
    else FragColor = vec4(0,0,0,1);
}