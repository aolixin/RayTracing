#version 460 core

#define PI              3.1415926
#define INF             114514.0
#define SIZE_TRIANGLE   7
#define SIZE_BVHNODE    4
#define SIZE_MATERIAL   6
#define INF             114514.0
#define MAX_DEPTH       10
#define MAX_SSP         1




out vec4 FragColor;

in vec3 pix;
in vec2 TexCoords;

uniform int FrameCounter;
uniform int Width;
uniform int Height;
uniform vec3 CameraPos;
uniform mat4 CameraRotate;


struct Material {
    vec3 emissive;
    vec3 baseColor;
    float subsurface;
    float metallic;
    float specular;
    float specularTint;
    float roughness;
    float anisotropic;
    float sheen;
    float sheenTint;
    float clearcoat;
    float clearcoatGloss;
    float IOR;
    float transmission;
};
uniform Material material;

uniform samplerBuffer triangles;
uniform int nTriangles;

uniform samplerBuffer nodes;
uniform samplerBuffer materials;

uniform samplerCube environmentMap;
uniform sampler2D lastFrame;

struct Triangle {
    vec3 p1, p2, p3;
    vec3 n1, n2, n3;
    int materialId;
};

struct BVHNode {
    int left;
    int right;
    int n;
    int index;
    vec3 AA, BB;
};

struct HitResult {
    bool isHit;
    bool isInside;
    float distance;
    vec3 hitPoint;
    vec3 normal;
    vec3 viewDir;
    Material material;
};

struct Ray {
    vec3 startPoint;
    vec3 direction;
};


HitResult HitTriangle(Triangle triangle, Ray ray) {
    HitResult res;
    res.distance = INF;
    res.isHit = false;
    res.isInside = false;

    vec3 p1 = triangle.p1;
    vec3 p2 = triangle.p2;
    vec3 p3 = triangle.p3;

    vec3 S = ray.startPoint;
    vec3 d = ray.direction;
    vec3 N = normalize(cross(p2 - p1, p3 - p1));

    if (dot(N, d) > 0.0f) {
        N = -N;
        res.isInside = true;
    }

    if (abs(dot(N, d)) < 0.00001f) return res;

    float t = (dot(N, p1) - dot(S, N)) / dot(d, N);
    if (t < 0.0005f) return res;

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

        float alpha = (-(P.x - p2.x) * (p3.y - p2.y) + (P.y - p2.y) * (p3.x - p2.x)) / (-(p1.x - p2.x - 0.00005) * (p3.y - p2.y + 0.00005) + (p1.y - p2.y + 0.00005) * (p3.x - p2.x + 0.00005));
        float beta = (-(P.x - p3.x) * (p1.y - p3.y) + (P.y - p3.y) * (p1.x - p3.x)) / (-(p2.x - p3.x - 0.00005) * (p1.y - p3.y + 0.00005) + (p2.y - p3.y + 0.00005) * (p1.x - p3.x + 0.00005));
        float gama = 1.0 - alpha - beta;
        vec3 Nsmooth = alpha * triangle.n1 + beta * triangle.n2 + gama * triangle.n3;
        Nsmooth = normalize(Nsmooth);
        res.normal = (res.isInside) ? (-Nsmooth) : (Nsmooth);
    }

    return res;

}

float HitAABB(Ray r, vec3 AA, vec3 BB) {
    vec3 invdir = 1.0 / r.direction;

    vec3 f = (BB - r.startPoint) * invdir;
    vec3 n = (AA - r.startPoint) * invdir;

    vec3 tmax = max(f, n);
    vec3 tmin = min(f, n);

    float t1 = min(tmax.x, min(tmax.y, tmax.z));
    float t0 = max(tmin.x, max(tmin.y, tmin.z));

    return (t1 >= t0) ? ((t0 > 0.0) ? (t0) : (t1)) : (-1.0);
}


BVHNode GetBVHNode(int i) {
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

Triangle GetTriangle(int i) {
    int offset = i * SIZE_TRIANGLE;
    Triangle t;


    t.p1 = texelFetch(triangles, offset + 0).xyz;
    t.p2 = texelFetch(triangles, offset + 1).xyz;
    t.p3 = texelFetch(triangles, offset + 2).xyz;

    t.n1 = texelFetch(triangles, offset + 3).xyz;
    t.n2 = texelFetch(triangles, offset + 4).xyz;
    t.n3 = texelFetch(triangles, offset + 5).xyz;


    t.materialId = int(texelFetch(triangles, offset + 6).x);

    return t;
}


Material GetMaterial(int i) {

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


HitResult HitArray(Ray ray, int l, int r) {
    HitResult res;
    res.isHit = false;
    res.distance = INF;
    for (int i = l; i <= r; i++) {
        Triangle triangle = GetTriangle(i);
        HitResult r = HitTriangle(triangle, ray);
        if (r.isHit && r.distance < res.distance) {
            res = r;
            //res.material = material;
            res.material = GetMaterial(triangle.materialId);
        }
    }
    return res;
}


HitResult HitBVH(Ray ray) {
    HitResult res;
    res.isHit = false;
    res.distance = INF;

    //int stack[256];
    int stack[512];
    int sp = 0;

    stack[sp++] = 0;
    while (sp > 0) {
        int top = stack[--sp];
        BVHNode node = GetBVHNode(top);


        if (node.n > 0) {
            int L = node.index;
            int R = node.index + node.n - 1;
            HitResult r = HitArray(ray, L, R);
            if (r.isHit && r.distance < res.distance) res = r;
            continue;
        }


        float d1 = INF;
        float d2 = INF;
        if (node.left > 0) {
            BVHNode leftNode = GetBVHNode(node.left);
            d1 = HitAABB(ray, leftNode.AA, leftNode.BB);
        }
        if (node.right > 0) {
            BVHNode rightNode = GetBVHNode(node.right);
            d2 = HitAABB(ray, rightNode.AA, rightNode.BB);
        }


        if (d1 > 0 && d2 > 0) {
            if (d1 < d2) {
                stack[sp++] = node.right;
                stack[sp++] = node.left;
            } else {
                stack[sp++] = node.left;
                stack[sp++] = node.right;
            }
        } else if (d1 > 0) {
            stack[sp++] = node.left;
        } else if (d2 > 0) {
            stack[sp++] = node.right;
        }
    }

    return res;
}

HitResult CastRay(Ray ray, int depth)
{
    HitResult res;
    res.isHit = false;


    if (depth > MAX_DEPTH)
    {
        return res;
    }
    vec3 colorRes = vec3(0);
    res = HitBVH(ray);

    if (res.isHit)
    {
        res.material.emissive *= pow(0.2, float(depth));
        return res;
    } else {
        res.material.emissive = vec3(0, 0, 0);
        return res;
    }
}

uint seed = uint(
    uint((pix.x * 0.5 + 0.5) * Width) * uint(1973) +
    uint((pix.y * 0.5 + 0.5) * Height) * uint(9277) +
    uint(FrameCounter) * uint(26699)) | uint(1);

uint wang_hash(inout uint seed) {
    seed = uint(seed ^ uint(61)) ^ uint(seed >> uint(16));
    seed *= uint(9);
    seed = seed ^ (seed >> 4);
    seed *= uint(0x27d4eb2d);
    seed = seed ^ (seed >> 15);
    return seed;
}

float rand() {
    return float(wang_hash(seed)) / 4294967296.0;
}

vec3 SampleHemisphere() {
    float z = rand();
    float r = max(0.0, sqrt(1.0 - z * z));
    float phi = 2.0 * PI * rand();
    return vec3(r * cos(phi), r * sin(phi), z);
}

vec2 SampleSphericalMap(vec3 v) {
    vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
    uv /= vec2(2.0 * PI, PI);
    uv += 0.5;
    uv.y = 1.0 - uv.y;
    return uv;
}

//vec3 SampleHdr(vec3 v) {
//    vec2 uv = SampleSphericalMap(normalize(v));
//    vec3 color = texture2D(hdrMap, uv).rgb;
//    color = min(color, vec3(10));
//    return color;
//}

vec3 SampleEnvCubeMap(vec3 v) {
    vec3 envColor = texture(environmentMap, v).rgb;
    return envColor;
}

vec3 toNormalHemisphere(vec3 v, vec3 N) {
    vec3 helper = vec3(1, 0, 0);
    if (abs(N.x) > 0.999) helper = vec3(0, 0, 1);
    vec3 tangent = normalize(cross(N, helper));
    vec3 bitangent = normalize(cross(N, tangent));
    return v.x * tangent + v.y * bitangent + v.z * N;
}

vec3 PathTracing(HitResult hit, int maxBounce) {

    vec3 Lo = vec3(0);
    vec3 history = vec3(1);

    for (int bounce = 0; bounce < maxBounce; bounce++) {
        vec3 wi = toNormalHemisphere(SampleHemisphere(), hit.normal);

        Ray randomRay;
        randomRay.startPoint = hit.hitPoint;
        randomRay.direction = wi;
        HitResult newHit = HitBVH(randomRay);

        float pdf = 1.0 / (2.0 * PI);
        float cosine_o = max(0.0, dot(-hit.viewDir, hit.normal));
        float cosine_i = max(0.0, dot(randomRay.direction, hit.normal));
        vec3 f_r = hit.material.baseColor / PI;

        if (!newHit.isHit) {
            //            vec3 skyColor = sampleHdr(randomRay.direction);
            vec3 skyColor = SampleEnvCubeMap(randomRay.direction);
            Lo += history * skyColor * f_r * cosine_i / pdf;
            break;
        }

        vec3 Le = newHit.material.emissive;
        Lo += history * Le * f_r * cosine_i / pdf;
        break;
        hit = newHit;
        history *= f_r * cosine_i / pdf;
    }

    return Lo;
}


void main()
{
    Ray ray;
    ray.startPoint = CameraPos;

    vec4 dir = CameraRotate * vec4(pix.xy, -1.5, 1.0);
    dir.x = dir.x * float(Width) / float(Height);
    vec2 AA = vec2((rand() - 0.5) / float(Width), (rand() - 0.5) / float(Height));
    dir.xy += AA;
    ray.direction = normalize(dir.xyz);

    // primary hit
    HitResult res = HitBVH(ray);
    vec3 color;
    //        if (res.isHit) {
    //            FragColor = vec4(res.normal, 1.0);
    //        }
    //        return;

    if (!res.isHit) {
        color = SampleEnvCubeMap(ray.direction);
    } else {
        //        color = res.normal;
        vec3 Le = res.material.emissive;
        vec3 Li = PathTracing(res, 2);
        color = Le + Li;
    }

    vec3 lastColor = texture2D(lastFrame, pix.xy * 0.5 + 0.5).rgb;
    color = mix(lastColor, color, 1.0 / float(FrameCounter + 1));

    FragColor = vec4(color, 1.0);
}