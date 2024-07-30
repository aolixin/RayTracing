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


//��Դλ��
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

//���������ͼ
uniform samplerCube irradianceMap;


//--------------------------------------------------------------------------------
//�����
uniform uint frameCounter;
uint frameCounter1 = uint(0);



// ndc����
vec3 pix = vec3((gl_FragCoord.xy /vec2(WIDTH,HEIGHT) ) * 2 - vec2(1),gl_FragCoord.z*2-1);

uint seed = uint(
    uint((pix.x * 0.5 + 0.5) * WIDTH)  * uint(1973) + 
    uint((pix.y * 0.5 + 0.5) * HEIGHT) * uint(9277) + 
    uint(frameCounter) * uint(26699)) | uint(1);

uint getSeed()
{
    uint seed = uint(
    uint((pix.x * 0.5 + 0.5) * WIDTH)  * uint(1973) + 
    uint((pix.y * 0.5 + 0.5) * HEIGHT) * uint(9277) + 
    uint(frameCounter1) * uint(26699)) | uint(1);
    frameCounter1++;
    return seed;
}

uint wang_hash(inout uint seed) {
            
    seed = uint(seed ^ uint(61)) ^ uint(seed >> uint(16));
    seed *= uint(9);
    seed = seed ^ (seed >> 4);
    seed *= uint(0x27d4eb2d);
    seed = seed ^ (seed >> 15);
    return seed;
}
 
float rand() {
    seed = getSeed();
    return float(wang_hash(seed)) / 4294967296.0;
}

// ������Ȳ���
vec3 SampleHemisphere() {
            
    float z = rand();
    float r = max(0, sqrt(1.0 - z*z));
    float phi = 2.0 * PI * rand();
    return vec3(r * cos(phi), r * sin(phi), z);
}

// ������ v ͶӰ�� N �ķ������
vec3 toNormalHemisphere(vec3 v, vec3 N) {
            
    vec3 helper = vec3(1, 0, 0);
    if(abs(N.x)>0.999) helper = vec3(0, 0, 1);
    vec3 tangent = normalize(cross(N, helper));
    vec3 bitangent = normalize(cross(N, tangent));
    return v.x * tangent + v.y * bitangent + v.z * N;
}

//--------------------------------------------------------------------------------


float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}
// ----------------------------------------------------------------------------
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}
float SchlickFresnel(float u) {
            
    float m = clamp(1-u, 0, 1);
    float m2 = m*m;
    return m2*m2*m; // pow(m,5)
}
// ����һ���ṹ��
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
uniform Material material; // ����һ���ṹ�����͵�uniform

uniform samplerBuffer triangles;
uniform samplerBuffer nodes;
uniform samplerBuffer materials;

uniform vec3 eye_pos;
//uniform int width;
//uniform int height;



uniform mat4 inverse_projection;
uniform mat4 inverse_view;


// Triangle ���ݸ�ʽ
struct Triangle {
    vec3 p1, p2, p3;    // ��������
    vec3 n1, n2, n3;    // ���㷨��
};

// BVH ���ڵ�
struct BVHNode {
    int left;           // ������
    int right;          // ������
    int n;              // ������������Ŀ
    int index;          // ����������
    vec3 AA, BB;        // ��ײ��
};

// �����󽻽��
struct HitResult {
    bool isHit;             // �Ƿ�����
    bool isInside;          // �Ƿ���ڲ�����
    float distance;         // �뽻��ľ���
    vec3 hitPoint;          // �������е�
    vec3 normal;            // ���е㷨��
    vec3 viewDir;           // ���иõ�Ĺ��ߵķ���
    Material material;      // ���е�ı������
};
// ����
struct Ray {
    vec3 startPoint;
    vec3 direction;
};



// ���ߺ��������� 
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

    vec3 S = ray.startPoint;    // �������
    vec3 d = ray.direction;     // ���߷���
    vec3 N = normalize(cross(p2-p1, p3-p1));    // ������

    // �������α���ģ���ڲ�������
    if (dot(N, d) > 0.0f) {
        N = -N;   
        res.isInside = true;
    }

    // ������ߺ�������ƽ��
    if (abs(dot(N, d)) < 0.00001f) return res;

    // ����
    float t = (dot(N, p1) - dot(S, N)) / dot(d, N);
    if (t < 0.0005f) return res;    // ����������ڹ��߱���

    // �������
    vec3 P = S + d * t;

    // �жϽ����Ƿ�����������
    vec3 c1 = cross(p2 - p1, P - p1);
    vec3 c2 = cross(p3 - p2, P - p2);
    vec3 c3 = cross(p1 - p3, P - p3);
    bool r1 = (dot(c1, N) > 0 && dot(c2, N) > 0 && dot(c3, N) > 0);
    bool r2 = (dot(c1, N) < 0 && dot(c2, N) < 0 && dot(c3, N) < 0);

    // ���У���װ���ؽ��
    if (r1 || r2) {
        res.isHit = true;
        res.hitPoint = P;
        res.distance = t;
        res.normal = N;
        res.viewDir = d;
        // ���ݽ���λ�ò�ֵ���㷨��
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

    return (t1 >= t0) ? ((t0 > 0.0) ? (t0) : (t1)) : (-1);
}

// ��ȡ�� i �±�� BVHNode ����
BVHNode getBVHNode(int i) {
    BVHNode node;

    // ��������
    int offset = i * SIZE_BVHNODE;
    ivec3 childs = ivec3(texelFetch(nodes, offset + 0).xyz);
    ivec3 leafInfo = ivec3(texelFetch(nodes, offset + 1).xyz);
    node.left = int(childs.x);
    node.right = int(childs.y);
    node.n = int(leafInfo.x);
    node.index = int(leafInfo.y);

    // ��Χ��
    node.AA = texelFetch(nodes, offset + 2).xyz;
    node.BB = texelFetch(nodes, offset + 3).xyz;

    return node;
}

Triangle getTriangle(int i) {
    int offset = i * SIZE_TRIANGLE;
    Triangle t;

    // ��������
    t.p1 = texelFetch(triangles, offset + 0).xyz;
    t.p2 = texelFetch(triangles, offset + 1).xyz;
    t.p3 = texelFetch(triangles, offset + 2).xyz;
    // ����
    t.n1 = texelFetch(triangles, offset + 3).xyz;
    t.n2 = texelFetch(triangles, offset + 4).xyz;
    t.n3 = texelFetch(triangles, offset + 5).xyz;

    return t;
}

// ��ȡ�� i �±�������εĲ���
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

// ���� BVH ��
HitResult hitBVH(Ray ray) {
    HitResult res;
    res.isHit = false;
    res.distance = INF;

    // ջ
    //int stack[256];
    int stack[512];
    int sp = 0;

    stack[sp++] = 0;
    while(sp>0) {
        int top = stack[--sp];
        BVHNode node = getBVHNode(top);
        
        // ��Ҷ�ӽڵ㣬���������Σ����������
        if(node.n>0) {
            int L = node.index;
            int R = node.index + node.n - 1;
            HitResult r = hitArray(ray, L, R);
            if(r.isHit && r.distance<res.distance) res = r;
            continue;
        }
        
        // �����Һ��� AABB ��
        float d1 = INF; // ����Ӿ���
        float d2 = INF; // �Һ��Ӿ���
        if(node.left>0) {
            BVHNode leftNode = getBVHNode(node.left);
            d1 = hitAABB(ray, leftNode.AA, leftNode.BB);
        }
        if(node.right>0) {
            BVHNode rightNode = getBVHNode(node.right);
            d2 = hitAABB(ray, rightNode.AA, rightNode.BB);
        }

        // ������ĺ���������
        if(d1>0 && d2>0) {
            if(d1<d2) { // d1<d2, �����
                stack[sp++] = node.right;
                stack[sp++] = node.left;
            } else {    // d2<d1, �ұ���
                stack[sp++] = node.left;
                stack[sp++] = node.right;
            }
        } else if(d1>0) {   // ���������
            stack[sp++] = node.left;
        } else if(d2>0) {   // �������ұ�
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
    //ײ����
    if(res.isHit)
    {
        res.material.emissive *= pow(0.2,depth);
        return res;
    }else{
        res.material.emissive = vec3(0,0,0);
        return res;
    }
}

vec3 RayTracing(Ray ray)
{
    vec3 colorRes = vec3(0);

    //ջģ��ݹ�
    Ray stack[4];
    int sp = 0;

    HitResult firstHit;
    firstHit = castRay(ray,0);

    if(firstHit.isHit)
    {

        HitResult res;

        Ray newRay;
        newRay.startPoint = firstHit.hitPoint;
        newRay.direction = ray.direction + 2 * firstHit.normal;
        stack[sp++] = newRay;
        int depth = 1;
        while(sp>0)
        {
            Ray nowRay = stack[--sp];
            res = castRay(nowRay,depth++);
            //ײ����
            if(res.isHit)
            {
                //������ɫ
                colorRes += res.material.emissive;
                //colorRes += vec3(0,0,0);
                //colorRes = res.material.baseColor;
                //return colorRes;
                //�����µ�����
                Ray newRay;
                newRay.startPoint = res.hitPoint;
                newRay.direction = nowRay.direction + 2 * res.normal;
                stack[sp++] = newRay;
            }
        }
    }

    vec3 F0 = vec3(0.04); 
    F0 = mix(F0,firstHit.material.baseColor, firstHit.material.metallic);
    vec3 kS = fresnelSchlick(max(dot(firstHit.normal, -ray.direction), 0.0), F0);
    vec3 kD = 1.0 - kS;

    kD *= 1.0 - firstHit.material.metallic;	  
    vec3 irradiance = texture(irradianceMap, firstHit.normal).rgb;
    vec3 diffuse      = irradiance * firstHit.material.baseColor;
    vec3 ambient = (kD * diffuse) * ao;

    vec3 color = ambient;

    // HDR tonemapping
    color = color / (color + vec3(1.0));
    // gamma correct
    //color = pow(color, vec3(1.0/2.2)); 
    //return colorRes;
    //return vec3(0,0,1);
    return colorRes + color;
    //return   color;
}




vec3 BRDF_Evaluate(vec3 V, vec3 N, vec3 L, in Material material) {
            
    float NdotL = dot(N, L);
    float NdotV = dot(N, V);
    if(NdotL < 0 || NdotV < 0) return vec3(0);

    vec3 H = normalize(L + V);
    float NdotH = dot(N, H);
    float LdotH = dot(L, H);

    
    vec3 Cdlin = material.baseColor;
	
	// ������
	float Fd90 = 0.5 + 2.0 * LdotH * LdotH * material.roughness;
	float FL = SchlickFresnel(NdotL);
	float FV = SchlickFresnel(NdotV);
	float Fd = mix(1.0, Fd90, FL) * mix(1.0, Fd90, FV);
	
	vec3 diffuse = Fd * Cdlin / PI;
	return diffuse  * (1.0 - material.metallic);
}

vec3 pathTracing(Ray ray)
{
    float prob = 0.6;
    vec3 Lo = vec3(0);
    //����˥��
    vec3 history = vec3(1);

    vec3 colorRes = vec3(0);

    //ջģ��ݹ�
    Ray stack[4];
    int sp = 0;

    HitResult firstHit;
    firstHit = castRay(ray,0);

    if(firstHit.isHit)
    {
        Lo += firstHit.material.emissive;
        //������跽��
        vec3 wi = toNormalHemisphere(SampleHemisphere(),firstHit.normal);
        HitResult res;

        Ray newRay;
        newRay.startPoint = firstHit.hitPoint;
        newRay.direction = wi;
        stack[sp++] = newRay;
        int depth = 0;
        while(sp>0)
        {
            //����˹���̾����Ƿ����
            if( rand() > prob)break;
            Ray nowRay = stack[--sp];
            res = hitBVH(nowRay);
            //ײ����
            if(res.isHit && depth++ < MAX_DEPTH)
            {
                //������ɫ
                //colorRes += res.material.emissive;
                vec3 V = -res.viewDir;
                vec3 N = res.normal;
                vec3 L = toNormalHemisphere(SampleHemisphere(),firstHit.normal);

                float pdf = 1.0 / (2.0 * PI);                                   // ������Ȳ��������ܶ�
                float cosine_o = max(0, dot(-res.viewDir, res.normal));         // �����ͷ��߼н�����
                float cosine_i = max(0, dot(L, res.normal));  // �����ͷ��߼н�����
                vec3 f_r = BRDF_Evaluate(V,N,L,res.material);
                vec3 Le = res.material.emissive;

                Lo +=  history * Le * f_r * cosine_i / pdf;
                history *= f_r * cosine_i / pdf;  // �ۻ���ɫ

                //�����µ�����
                Ray newRay;
                newRay.startPoint = res.hitPoint;
                newRay.direction = L;
                stack[sp++] = newRay;
            }
        }
    }

    //Lo = vec3(0,0,0.3);
    return Lo;
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
    for(int i =0;i<MAX_SSP;i++)
    {
        vec2 AA = vec2((rand()-0.5)/float(WIDTH), (rand()-0.5)/float(HEIGHT));
        worldPos = getWorldPos(vec3( pix.xy + AA,pix.z));
        ray.direction = normalize(worldPos.xyz - eye_pos);
        result += pathTracing(ray) / MAX_SSP;
        //result += vec3(0,0,0.3);
    }
    //result = RayTracing(ray);
    result = pow(result,vec3(1.0/2.2));
    FragColor = vec4(result, 1.0);
}